/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#define _GNU_SOURCE
#include "threading.h"
#include "instr_data.h"
#include "rave2prv.h"
#include "state.h"
#include "utils.h"
#include "profiling.h"
#include "scalar_blocks.h"
#include <sched.h>

inline uint64_t synch_threads(thread_state_t * state){
	int cpu_index = state->cpu_index;
	if (cpu_index >= N_THREADS){ //Wait for the thread to be properly initialized
		sched_yield();
		return -1;
	}
	//Core info
	uint64_t thread_timestamp = state->timestamp;

	if (N_THREADS>1){	
		if (state->need_align){
			//Target is global when align=1, is parallle barrier when align=2
			uint64_t target_time = state->need_align==1 ? thread_timestamp : parallel_region.barrier_time;
			if (thread_timestamp < target_time){ //Jump only forward
				if (TRACE_ENABLED && PRINT_PRV && !MUSA){
					set_lock(write_lock);
					trace_row(FD_PRV,mpi_rank, cpu_index, state->last_row, thread_timestamp);
					clean_event(FD_PRV); 
					release_lock(write_lock);
				}
				thread_timestamp=target_time; 
				state->print_first_scalar = 1;
			}
			state->need_align = 0;
		}
	}

	return thread_timestamp;
}

inline void detect_loop_start(profile_t * loop_profile, uint64_t PC){
	if (loop_profile->jump_PC!=-1){
		if (PC != loop_profile->jump_PC){ //Loop not taken
			update_PC(loop_profile);
			//reset it so next one-it loop is not that far off...
			loop_profile -> loop_weight = 0;
			for(int i=0; i<NUM_VREGS; ++i) loop_profile -> used_vreg[i] = 0;
		}else{ //Loop taken
			if (loop_profile->loop_its == 1){ //If we took the first branch, reset counters and do actual measures
				loop_profile->loop_weight = 0;
				loop_profile->loop_instr = 0;
				for(int i=0; i<NUM_VREGS; ++i) loop_profile -> used_vreg[i] = 0;
			}else if (loop_profile->loop_its == 2){ //If we took the second iteration, assume iteration 0 was equal to iteration 1.
				loop_profile->loop_weight *= 2;
				loop_profile->loop_instr *= 2;
			}
			loop_profile->loop_its++;
		}
		loop_profile->jump_PC=-1;
	}
}

inline void detect_func_start(calltrace_t * call_trace, uint64_t PC){
	if (call_trace->next_is_func == 1){
		call_trace->next_is_func = 0;
		add_to_calltrace(call_trace, PC);
	}
}

void scalar_block_exec(thread_state_t * state, scalar_block_data_t * data){

	uint64_t thread_timestamp = synch_threads(state);
	if (thread_timestamp==-1) return;
	profile_t * loop_profile = &state->loop_profile;
	int n_instr = data->instr;

	if (TRACE_ENABLED){

		//LOOP PROFILER CONTROL
		if (PRINT_PROFILE){
			for(int i=0; i<n_instr; ++i){
				//Detect loop beginning: 
				detect_loop_start(loop_profile, data->PCs[i]);

				//Detect loop end
				if (data->PCs[i] == data->PC_branch){
					if (data->PCs[i] != loop_profile->curr_loop_PC){ //New loop
						loop_profile->loop_its = 1;
						loop_profile->loop_instr = (data->PC_branch-data->PC_loop)/4; //Approximation
					}
					loop_profile->curr_loop_PC = data->PC_branch; 
					loop_profile->jump_PC = data->PC_loop; 
				}
				loop_profile->loop_instr++;
				loop_profile->loop_weight++;
			}
		}

		//CALL TRACE CONTROL
		if (PRINT_CALLTRACE){
#if 1
			//If we are on a new function, add it to the call trace
			detect_func_start(&state->call_trace, data->PCs[0]);
			//If block ends with a jump, next block will be a new function
			if (data->has_func_jump) state->call_trace.next_is_func = 1; //Next block will be on a new funcion!
#endif
		}

		//LOGFILE
		if (PRINT_LOGFILE && TRACE_SCALAR){
			set_lock(write_lock);
			for(int i=0; i<n_instr; ++i){
				qemu_plugin_outs(data->strings[i]);
				qemu_plugin_outs("\n");
			}
			release_lock(write_lock);
		}

		//PRV GENERATION
		if (PRINT_PRV){
			int cpu_index = state->cpu_index;
			char row = SCALAR_ROW;
			char row_change = (row != state->last_row) ?1:0;
			if (row_change && !MUSA){ 
				set_lock(write_lock);
				trace_row(FD_PRV,mpi_rank, cpu_index, state->last_row, thread_timestamp);
				clean_event(FD_PRV);
				release_lock(write_lock);
			}
			//Scalar instructions should always be printed when: row changed(1), type changed (2), is first scalar in the trace (3)
			if (row_change || state->last_was_vsetvl || state->print_first_scalar){	
				set_lock(write_lock);
				trace_row(FD_PRV,mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(FD_PRV,event_instruction,PRV_SCALAR*!MUSA);
				trace_event_value(FD_PRV,event_class,T_SCALAR);
				trace_event_value(FD_PRV,event_pc, data->PCs[0]);
				if (MUSA){ //for MUSA
					int prev_dst = state->prev_dst;
					state->prev_dst = 0;
					trace_event_value(FD_PRV,event_scalb, 0);
					if (TRACE_ADDR) trace_event_value(FD_PRV,event_addr, 0);
					trace_event_value(FD_PRV,event_dst, prev_dst);
					trace_event_value(FD_PRV,event_src1, 0);
					trace_event_value(FD_PRV,event_src2, 0);
					trace_event_value(FD_PRV,event_vl, 0);
					trace_event_value(FD_PRV,event_sew, 0);
					trace_event_value(FD_PRV,event_lmul, 0);
				}
				release_lock(write_lock);
			}
		}
	}
	
	//Counters
	state->accum_counters.scalar_instr += n_instr;
	state->accum_counters.scalarflops += data->flops;
	state->accum_counters.moved_bytes_s += data->moved_bytes;

	//Update state
	thread_timestamp += n_instr;
	if (thread_timestamp > timestamp) timestamp = thread_timestamp;
	state->scalar_instr_since_vector += n_instr;
	state->timestamp = thread_timestamp;
	state->last_row = SCALAR_ROW;
	state->print_first_scalar = 0;
	state->last_was_vsetvl = 0; 
}

void rolling_scalar_block(uint32_t opcode, uint64_t PC, scalar_block_data_t * data){
		int16_t type = instr_set_scalar_type(opcode);

		if (TRACE_ENABLED && PRINT_PROFILE){
			if (is_subtype(type, T_BRANCH)){
				int64_t offset = get_loop_offset(opcode);
				if (offset <= 0){
					data->PC_branch = PC;
					data->PC_loop = PC + offset;
				}
			}
		}
		if (PRINT_CALLTRACE){
#if 1
			if (data->has_func_jump){
				fprintf(stderr, "JUMP not on last instruction in scalar block, aborting\n");
				exit(-1);
			}
			if (is_subtype(type, T_JUMP)){
				//printf("  Block contains jump\n");
				data->has_func_jump = 1;	
			}
#endif
		}
		if (is_subsubsubtype(type, T_FUSED)) data->flops += 2;
		else if (is_subsubsubtype(type, T_SINGLE)) data->flops += 1;
		else if (is_subtype(type, T_MEMORY)){
			int quadrant = (opcode &0x3);
			if (quadrant == 3){
				int width = (opcode >> 12)&0x3; //3 instead of 7 to %4
				data->moved_bytes += (1<<(width));
			}else if ((quadrant == 0) || (quadrant == 2)){
				int width = (opcode >> 13)&0x3; //3 instead of 7 to %4
				data->moved_bytes += (width==1 || width==3) ? 8 : 4; 
				//Double: 001, 011, 101, 111
				//Single: 010, 110
			}
		}
		++data->instr;
}

//Vector and vsetvli instructions
void insn_exec(thread_state_t * state, instr_data * instr){

	uint64_t thread_timestamp = synch_threads(state);
	if (thread_timestamp==-1) return;

	profile_t * loop_profile = &state->loop_profile;
	if (TRACE_ENABLED){
		if (PRINT_PROFILE){
			detect_loop_start(loop_profile, instr->PC);
		}
		if (PRINT_CALLTRACE){
#if 1
			detect_func_start(&state->call_trace, instr->PC); 
#endif
		}
	}

	int row = SCALAR_ROW;
	uint64_t vl=0, vtype, sew=3, lmul=1, rvl=0;
	//double lmul_value;
	uint64_t addr = 0;
	int stride = 0;

	int8_t * indexes_8 = NULL;
	int16_t * indexes_16 = NULL;
	int32_t * indexes_32 = NULL;
	int64_t * indexes_64 = NULL;

	if (is_type(instr->type, T_VECTOR)){ //VECTOR
		loop_profile -> used_vreg[((instr->instr32)>>7)&0x1F] = 1;
		row = VECTOR_ROW;
		vl = qemu_get_vl(state); 
		vtype = qemu_get_vtype(state);
#ifdef RVV_07
		sew = (vtype >> 2)&0x7;
		lmul = vtype&0x3;
#else
		sew = (vtype >> 3)&0x7;
		lmul = vtype&0x7;

#endif
		if (is_subtype(instr->type, T_MEMORY)){
			if (TRACE_ADDR){
				int src1 = (instr->instr32>>15)&0x1F;
				addr = qemu_get_xreg(state,src1);
			}

			if (is_subsubtype(instr->type, T_STRIDE)){
				int src2 = (instr->instr32>>20)&0x1F;
				stride = qemu_get_xreg(state,src2);
			}
#ifndef RVV_07
			int width = (instr->instr32 >> 12)&0x3; 
			sew = width;
#endif

			if(TRACE_INDEXES && is_subsubtype(instr->type, T_INDEX)){
				int src2 = (instr->instr32>>20)&0x1F;
				char * contents = qemu_get_vreg(state,src2,vl*(2<<(sew)));
				if (sew==0) indexes_8 = (int8_t*)contents;
				else if (sew==1) indexes_16 = (int16_t*)contents;
				else if (sew==2) indexes_32 = (int32_t*)contents;
				else if (sew==3) indexes_64 = (int64_t*)contents;
			}

#ifndef RVV_07
			if ((((instr->instr32>>20)&0xFF) == 0x28)){
				//Whole register load/stre
				sew = 0; //sew: 1 byte (8 bits)
				vl = RAVE_VLMAX / 8; //vl
			}
		}else if ( ((instr->instr32&0x7F)==0x57) && (((instr->instr32>>26)&0x3F)==0x27) && (((instr->instr32>>12)&0x07)==0x03)) {
			//Whole register move
			int NFIELDS = (instr->instr32>>15)&0x1F;
			lmul = NFIELDS==7?3 : NFIELDS==3?2 : NFIELDS==1?1 : 0;
			sew = 0; //sew: 1 byte (8 bits)
			vl = RAVE_VLMAX / 8; //vl
#endif
		}
	}else if (is_type(instr->type, T_VSETVL)){ 
		int src1 = (instr->instr32>>15)&0x1F;
		rvl = qemu_get_xreg(state,src1);
	}

	//  Logfile  //
	if (TRACE_ENABLED){
		if (PRINT_LOGFILE){
			if (!TRACE_SCALAR && state->scalar_instr_since_vector>0){
				char * string;
				if (-1 == asprintf(&string,"%d scalar instructions\n", state->scalar_instr_since_vector)){
					printf("Error allocating string buffer\n");
					exit(-1);
				}
				set_lock(write_lock);
				qemu_plugin_outs(string);
				free(string);
				release_lock(write_lock);
			}
			set_lock(write_lock);
			qemu_plugin_outs(instr->asm_string);
			qemu_plugin_outs("\n");
			release_lock(write_lock);
		}

		//  PRV  //
		if (PRINT_PRV){
			int cpu_index = state->cpu_index;
			char row_change = (row != state->last_row) ?1:0;
			if (row_change && !MUSA){ 
				set_lock(write_lock);
				trace_row(FD_PRV,mpi_rank, cpu_index, state->last_row, thread_timestamp);
				clean_event(FD_PRV);
				release_lock(write_lock);
			}
			//Scalar instructions should always be printed when: row changed(1), type changed (2), is first scalar in the trace (3)
			if (is_type(instr->type, T_VSETVL)){
				set_lock(write_lock);
				trace_row(FD_PRV,mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(FD_PRV,event_class,instr->type);
				trace_event_value(FD_PRV,event_pc, instr->PC);
				trace_event_value(FD_PRV,event_instruction, instr->paraver_code);
				if (MUSA){
					int prev_dst = state->prev_dst;
					state->prev_dst = instr->dst;
					trace_event_value(FD_PRV,event_dst, prev_dst);
				}else{
					trace_event_value(FD_PRV,event_dst, instr->dst);
				}
				trace_event_value(FD_PRV,event_src1, instr->src1);
				trace_event_value(FD_PRV,event_rvl, rvl); 
				release_lock(write_lock);
			}else{ //VECTOR 
				set_lock(write_lock);
				trace_row(FD_PRV,mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(FD_PRV,event_class,instr->type);
				trace_event_value(FD_PRV,event_pc, instr->PC);
				trace_event_value(FD_PRV,event_scalb, state->scalar_instr_since_vector);
				if (TRACE_ADDR) trace_event_value(FD_PRV,event_addr, addr);
				if (TRACE_INDEXES){
					if (indexes_8 != NULL){
						for(int i=0; i<vl; ++i) trace_event_value(FD_PRV, event_indexes+i, (int)indexes_8[i]);
						free(indexes_8);
					}else if (indexes_16 != NULL){
						for(int i=0; i<vl; ++i)  trace_event_value(FD_PRV, event_indexes+i, (int)indexes_16[i]);
						free(indexes_16);
					}else if (indexes_32 != NULL){
						for(int i=0; i<vl; ++i)  trace_event_value(FD_PRV, event_indexes+i, (int)indexes_32[i]);
						free(indexes_32);
					}else if (indexes_64 != NULL){
						for(int i=0; i<vl; ++i)  trace_event_value(FD_PRV, event_indexes+i, indexes_64[i]);
						free(indexes_64);
					}
				}
				if (MUSA){
					int prev_dst = state->prev_dst;
					state->prev_dst = instr->dst;
					trace_event_value(FD_PRV,event_dst, prev_dst);
				}else{
					trace_event_value(FD_PRV,event_dst, instr->dst);
				}
				trace_event_value(FD_PRV,event_src1, instr->src1);
				trace_event_value(FD_PRV,event_src2, instr->src2);
				trace_event_value(FD_PRV,event_instruction, instr->paraver_code);
				trace_event_value(FD_PRV,event_vl, vl);
				if (MUSA){
					trace_event_value(FD_PRV,event_sew, 1<<(3+sew));
					int musa_lmul = lmul<4 ? 1<<lmul : lmul==5?18 : lmul==6?14 : lmul==7?12 : 0;
					trace_event_value(FD_PRV,event_lmul, musa_lmul);
				}else{
					trace_event_value(FD_PRV,event_sew, sew);
					trace_event_value(FD_PRV,event_lmul, lmul);
				}
				if (is_type(instr->type, T_VECTOR) && is_subtype(instr->type, T_MEMORY) && is_subsubtype(instr->type, T_STRIDE)){
					trace_event_value(FD_PRV,event_stride, stride);
					state->reset_stride = 1;
				}else if (state->reset_stride){
					trace_event_value(FD_PRV,event_stride, 0);
					state->reset_stride = 0;
				}	
				release_lock(write_lock);
			}
		}
	}

	// Counters //
	loop_profile->loop_instr++; 
	if (is_type(instr->type, T_VECTOR)) {
		loop_profile->loop_weight += vl;

		state->scalar_instr_since_vector=0;
		++state->accum_counters.vector_instr[sew];
		state->accum_counters.velem[sew] += vl;
		if (is_subtype(instr->type, T_ARITH)){
			if (is_subsubtype(instr->type, T_FP)){
				++state->accum_counters.vfp_instr[sew];
				state->accum_counters.velem_arith[sew] += vl; 
				if (is_subsubsubtype(instr->type, T_FUSED)) state->accum_counters.vectorflops += 2*vl; 
				else state->accum_counters.vectorflops += vl;
			}else if (is_subsubtype(instr->type, T_INT)){
				++state->accum_counters.vint_instr[sew];
				state->accum_counters.velem_arith[sew] += vl; 
			}
		}else if (is_subtype(instr->type, T_REDUCTION)){
			state->accum_counters.velem_reductions[sew] += vl;
			if (is_subsubtype(instr->type, T_FP)){
				++state->accum_counters.vfp_reductions[sew];
			}else if (is_subsubtype(instr->type, T_INT)){
				++state->accum_counters.vint_reductions[sew];
			}
		}else if (is_subtype(instr->type, T_MASK)){
			++state->accum_counters.vmask_instr[sew];
			state->accum_counters.velem_mask[sew] += vl; 
		}else if (is_subtype(instr->type, T_MEMORY)){
			state->accum_counters.moved_bytes_v += vl*(1<<(sew));
			state->accum_counters.velem_mem[sew] += vl; 
			if (is_subsubtype(instr->type, T_UNIT)) ++state->accum_counters.vunit_instr[sew];
			else if (is_subsubtype(instr->type, T_STRIDE)){
				++state->accum_counters.vstride_instr[sew];
				state->accum_counters.agg_strides[sew] += stride;
			}
			else if (is_subsubtype(instr->type, T_INDEX)) ++state->accum_counters.vidx_instr[sew];
			else if (is_subsubtype(instr->type, T_SPILL)) ++state->accum_counters.vspill_instr[sew];
		}
	}else if (is_type(instr->type, T_VSETVL)){ 
		loop_profile->loop_weight += 1;
		++state->accum_counters.vsetvl_instr;
	}

	thread_timestamp++;
	//TODO: Use an atomic here?
	if (thread_timestamp > timestamp) timestamp = thread_timestamp;

	//Update state
	state->timestamp = thread_timestamp;
	state->last_row = row;
	state->print_first_scalar = 0;
	state->last_was_vsetvl = is_type(instr->type, T_VSETVL);

}

