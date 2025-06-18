/*
 * Copyright (C) 2021, Alexandre Iooss <erdnaxe@crans.org>
 *
 * Log instruction execution with memory access.
 *
 * License: GNU GPL, version 2 or later.
 *   See the COPYING file in the top-level directory.
 */
#include <glib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qemu-plugin.h>
#include <fcntl.h>
#include <sys/file.h>


//#define TIMEDEBUG
#ifdef TIMEDEBUG
uint64_t getmicros(){
	#if 0
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return tp.tv_sec *1e6 + tp.tv_usec;
	#else
	uint64_t a,d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));
	return (a | (d << 32));
	#endif
}
uint64_t time_trans=0;
uint64_t time_vcpu_exe=0;
uint64_t time_vcpu_control=0;
int num_trans=0;
int num_vcpu_exe=0;
int num_vcpu_control=0;
#endif

////////////////////////////////    Control variables    /////////////////////////////////
int RAVE_VLMAX = 0;
int RAVE_ELEN = 64;
char PRINT_LOGFILE = 0;
char PRINT_SCALAR = 0;
char PRINT_ADDR = 0;
char PRINT_PRV = 0;
char PRINT_REPORT = 0;
char PRINT_CSV = 0;
char TRACE_ENABLED = 1; //Enabled by default 
char DETECT_SYMBOLS = 0;
char * filename = NULL; 
FILE * FD_PRV;
FILE * FD_PCF;
FILE * FD_ROW;
FILE * FD_CSV;
FILE * FD_COMM;
FILE * FD_REPORT;

//#define EPI_07

#ifdef EPI_07
	#undef EPI_10
#else
	#define EPI_10
#endif

#include "rave_counters.h"
#include "rave2prv.h"
#include "instr_data.h"


int mpi_rank = 0;
int mpi_size = 1;
volatile int N_THREADS = 0; //This increases when a new CPU is online
int expected_threads = 0;
int alloc_threads = 0;

struct parallel_region_t{
	volatile int n_threads; //Counts threads in region
	volatile int in_barrier; //Counts threads waiting in barrier
	volatile int crossed_barrier; //Counts threads that passed the last barrier
	volatile uint64_t barrier_time; //Max. timestamp from all threads at the barrier (to sync)

	volatile int lock;
	int master_thread;
	int first_barrier;
	rave_counters parallel_region_counter;
	rave_counters last_barrier_counters;
};
typedef struct parallel_region_t parallel_region_t;
parallel_region_t parallel_region;

///////////////////////////////////////////////////////////////////////////////////////////


char contains_string(char * str, const char * find){
		int slen = strlen(str);
		int flen = strlen(find);
		int progress = 0;
		for(int i=0; i<slen; ++i){
				if (str[i] == find[progress]) ++progress;
				else if (str[i] == find[0]) progress=1;
				else progress = 0;
				if (progress == flen) return 1;
		}
		return 0;
}


//////////////////////////////////////////////////////////////////////////////////////

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

/* Store last executed instruction on each vCPU as a GString */

/**
 * Log instruction execution
 */

//include/hw/core/cpu.h (0.7 :307 (def) ||||| 1.0 : 323 (def CPUState) //Util for knowing OFFSET REGS
#define sizeof_ulong sizeof(uint64_t)

//qemu_get_cpu returns an ArchCPU, which has a CPURISCVState (typdef of CPUArchState), 
//ArchCPU is defined in target/riscv/cpu.h (l:277 for 0.7, l:444 for 1.0)
//CPUArchState is defined in target/riscv/cpu.h (l:114 for 0.7, l:161 for 1.0)
//In accel/tcg/plugin-gen.c (l:175 for 0.7, l:165 for 1.0) is a good place to put : printf("Offset is %ld\n",offsetof(ArchCPU, env));
#ifdef EPI_07
#define OFFSET_CPUState (33552) //For 0.7
#define OFFSET_REGS (sizeof_ulong*32 + sizeof(uint64_t)*32 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#else
//#define OFFSET_CPUState (832) //For 1.0
#define OFFSET_CPUState (10176) //For 1.0
#define OFFSET_REGS (sizeof_ulong*32*2 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#endif

#define RV_VLEN_MAX (256*64)

int64_t qemu_get_vl(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*2);
}
int64_t qemu_get_vtype(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*4);
}
int64_t qemu_get_pc(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*5);
}
int64_t qemu_get_xreg(uint8_t * cpu, int reg){
		return *(int64_t*)(cpu + OFFSET_CPUState + sizeof_ulong*reg); 
}

void *qemu_get_cpu(int index);

void trace_row(int process, int cpu, int pipeline, uint64_t timestamp){
	fprintf(FD_PRV, "\n2:1:%d:%d:%d:%lu", process+1, cpu+1, pipeline+1, timestamp);
}
void trace_event_value(int event, uint64_t value){
	fprintf(FD_PRV, ":%d:%lu", event,value);
}
#define SCALAR_ROW 0
#define VECTOR_ROW 1

uint64_t timestamp=0;


//Per-thread info
struct thread_state_t{
	int last_row;
	int reset_stride;
	int last_instr_symbol;
	int last_was_vsetvl;
	int scalar_instr_since_vector;
	int print_first_scalar;
	char need_align;
	uint64_t timestamp;
	rave_counters accum_counters;
};
typedef struct thread_state_t thread_state_t;
thread_state_t * cpus_state;


void reset_thread(thread_state_t * state){
	state -> last_row = 0;
	state -> reset_stride = 0;
	state -> last_instr_symbol = -1;
	state -> last_was_vsetvl = 0;
	state -> scalar_instr_since_vector = 0;
	state -> print_first_scalar = 1;
	state -> need_align = 1;
	state -> timestamp = 0;
	reset_counters(&(state->accum_counters));
}


volatile int write_lock = 0;
#define set_lock(lock) if(N_THREADS>1){ while (! __sync_bool_compare_and_swap(&lock, 0, 1)){sched_yield();}}//Wait until lock is 0, then put it to 1
#define release_lock(lock) if (N_THREADS>1) { __sync_val_compare_and_swap(&lock, 1, 0);} //Unlock 
#define file_lock(fd, action) flock(fd,action); 
//#define set_lock(lock) ; 
//#define release_lock(lock) ; 
//#define file_lock(fd, action) {printf("%d: flock " #fd #action "\n", mpi_rank); flock(fd,action); printf("done\n");}


//Symbols:
int found_main=0;
int SYMBOL_MAIN = -1;
#define SYMBOL_EMPTY 0
int SYMBOL_DL = -1;
int print_sym = 1;


static void vcpu_insn_exec(unsigned int cpu_index, void *udata){

	if (cpu_index >= N_THREADS){ //Wait for the thread to be properly initialized
		sched_yield();
		return;
	}
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif
	//
	//Core info
	uint64_t thread_timestamp = cpus_state[cpu_index].timestamp;

	if (N_THREADS>1){	
		if (cpus_state[cpu_index].need_align){
			//Target is global when align=1, is parallle barrier when align=2
			uint64_t target_time = cpus_state[cpu_index].need_align==1 ? thread_timestamp : parallel_region.barrier_time;

			if (thread_timestamp < target_time){ //Jump only forward
				if (TRACE_ENABLED && PRINT_PRV){
					set_lock(write_lock);
					trace_row(mpi_rank, cpu_index, cpus_state[cpu_index].last_row, thread_timestamp);
					clean_event(FD_PRV); 
					release_lock(write_lock);
				}
				thread_timestamp=target_time; 
				cpus_state[cpu_index].print_first_scalar = 1;
			}
			cpus_state[cpu_index].need_align = 0;
		}
	}

	instr_data * instr = (instr_data*)udata;

	if (DETECT_SYMBOLS){
		int symbol = instr->symbol_id;
		if (cpus_state[cpu_index].last_instr_symbol != symbol){
			if (!found_main && symbol==SYMBOL_MAIN){
				found_main=1;
			}
			if (found_main){
				if (symbol!=SYMBOL_EMPTY){
					cpus_state[cpu_index].last_instr_symbol = symbol;
					if (print_sym && TRACE_ENABLED){
						if (PRINT_PRV){
							set_lock(write_lock);
							trace_row(mpi_rank, cpu_index, SCALAR_ROW, thread_timestamp);
							trace_event_value(1001,symbol);
							trace_row(mpi_rank, cpu_index, VECTOR_ROW, thread_timestamp);
							trace_event_value(1001,symbol);
							release_lock(write_lock);
						}
					}
					if (print_sym && symbol==SYMBOL_DL) print_sym=0;
					else if (!print_sym && symbol==SYMBOL_DL) print_sym=1;
				}
			}
		}
	}

	int row = SCALAR_ROW;
	uint64_t vl=0, vtype, sew, lmul;
	//double lmul_value;
	uint64_t addr = 0;
	int stride = 0;

	if (is_type(instr->type, T_VECTOR)){ //VECTOR
		row = VECTOR_ROW;
		uint8_t *cpu = qemu_get_cpu(cpu_index);
		vl = qemu_get_vl(cpu); 
		vtype = qemu_get_vtype(cpu);
#ifdef EPI_07
		sew = (vtype >> 2)&0x7;
		lmul = vtype&0x3;
		//lmul_value = (double)(1<<lmul); 
#else
		sew = (vtype >> 3)&0x7;
		lmul = vtype&0x7;
		//lmul_value = (lmul < 4) ? (double)(1<<lmul) : (lmul==7)? 0.5 : (lmul==6)? 0.25 : 0.125;
#endif
		if (is_subtype(instr->type, T_MEMORY)){

			if (PRINT_ADDR){
				int src1 = (instr->instr32>>15)&0x1F;
				addr = qemu_get_xreg(cpu,src1);
			}

		 	if (is_subsubtype(instr->type, T_STRIDE)){
				int src2 = (instr->instr32>>20)&0x1F;
				stride = qemu_get_xreg(cpu,src2);
				//printf("vlse with stride %d\n",stride);
			}
			#ifndef EPI_07
			int width = (instr->instr32 >> 12)&0x3; 
			sew = width;

			if ((((instr->instr32>>20)&0xFF) == 0x28)){
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
	}

	//  Logfile  //
	if (TRACE_ENABLED){
		if (PRINT_LOGFILE){
			if (!PRINT_SCALAR && !is_type(instr->type, T_SCALAR) && cpus_state[cpu_index].scalar_instr_since_vector>0){
				char * string = g_strdup_printf("%d scalar instructions\n", cpus_state[cpu_index].scalar_instr_since_vector); 
				set_lock(write_lock);
				qemu_plugin_outs(string);
				if (!is_type(instr->type, T_SCALAR) || PRINT_SCALAR){ 
					qemu_plugin_outs(instr->asm_string);
					qemu_plugin_outs("\n");
				}
				release_lock(write_lock);
				free(string);
			}else{
				if (!is_type(instr->type, T_SCALAR) || PRINT_SCALAR){ 
					set_lock(write_lock);
					qemu_plugin_outs(instr->asm_string);
					qemu_plugin_outs("\n");
					release_lock(write_lock);
				}
			}
		}

		//  PRV  //
		if (PRINT_PRV){
			char row_change = (row != cpus_state[cpu_index].last_row) ?1:0;
			if (row_change){ 
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, cpus_state[cpu_index].last_row, thread_timestamp);
				clean_event(FD_PRV); 
				release_lock(write_lock);
			}
			//Scalar instructions should always be printed when: row changed(1), type changed (2), is first scalar in the trace (3)
			if (is_type(instr->type, T_SCALAR) && !PRINT_SCALAR){
				if (row_change || cpus_state[cpu_index].last_was_vsetvl || cpus_state[cpu_index].print_first_scalar){	
					set_lock(write_lock);
					trace_row(mpi_rank, cpu_index, row, thread_timestamp);
					trace_event_value(event_instruction,1000);
					trace_event_value(event_class,instr->type);
					trace_event_value(event_pc, instr->PC);
					release_lock(write_lock);
				}
			}else if (is_type(instr->type, T_VSETVL)){
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(event_class,instr->type);
				trace_event_value(event_pc, instr->PC);
				trace_event_value(event_instruction, instr->paraver_code);
				trace_event_value(event_dst, instr->dst);
				trace_event_value(event_src1, instr->src1);
				release_lock(write_lock);
			}else{ //PRINT_SCALAR || (instr!=SCALAR && instr!=VSETVL)
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(event_class,instr->type);
				trace_event_value(event_pc, instr->PC);
				trace_event_value(event_scalb, cpus_state[cpu_index].scalar_instr_since_vector);
				if (PRINT_ADDR) trace_event_value(event_addr, addr);
				trace_event_value(event_dst, instr->dst);
				trace_event_value(event_src1, instr->src1);
				trace_event_value(event_src2, instr->src2);
				trace_event_value(event_instruction, instr->paraver_code);
				trace_event_value(event_vl, vl);
				trace_event_value(event_sew, sew);
				trace_event_value(event_lmul, lmul);

				if (is_type(instr->type, T_VECTOR) && is_subtype(instr->type, T_MEMORY) && is_subsubtype(instr->type, T_STRIDE)){
					trace_event_value(event_stride, stride);
					cpus_state[cpu_index].reset_stride = 1;
				}else if (cpus_state[cpu_index].reset_stride){
					trace_event_value(event_stride, 0);
					cpus_state[cpu_index].reset_stride = 0;
				}	
				release_lock(write_lock);
			}
		}
	}

	// Counters //


	if (is_type(instr->type, T_SCALAR)) {
		cpus_state[cpu_index].scalar_instr_since_vector++;
		++cpus_state[cpu_index].accum_counters.scalar_instr;

		int opcode = (instr->instr32 & 0x3F);
		if (opcode == 0b0000011 || opcode == 0b0100011 || opcode == 0b0000111 || opcode == 0b0100111){ 
			int width = (instr->instr32 >> 12)&0x3; //3 instead of 7 to %4
			cpus_state[cpu_index].accum_counters.moved_bytes_s += (1<<(width));
			//B 0, 4
			//H 1, 5
			//W 2, 6
			//D 3
		} 
	}else if (is_type(instr->type, T_VECTOR)) {
		cpus_state[cpu_index].scalar_instr_since_vector=0;
		++cpus_state[cpu_index].accum_counters.vector_instr[sew];
		cpus_state[cpu_index].accum_counters.velem[sew] += vl; 
		if (is_subsubtype(instr->type, T_FP)){
			++cpus_state[cpu_index].accum_counters.vfp_instr[sew];
		}else if (is_subsubtype(instr->type, T_INT)){ ++cpus_state[cpu_index].accum_counters.vint_instr[sew];
		}else if (is_subtype(instr->type, T_MASK)){ ++cpus_state[cpu_index].accum_counters.vmask_instr[sew];
		}else if (is_subtype(instr->type, T_MEMORY)){
			cpus_state[cpu_index].accum_counters.moved_bytes_v += vl*(1<<(sew));
			if (is_subsubtype(instr->type, T_UNIT)) ++cpus_state[cpu_index].accum_counters.vunit_instr[sew];
			else if (is_subsubtype(instr->type, T_STRIDE)){
				++cpus_state[cpu_index].accum_counters.vstride_instr[sew];
				cpus_state[cpu_index].accum_counters.agg_strides[sew] += stride;
			}
			else if (is_subsubtype(instr->type, T_INDEX)) ++cpus_state[cpu_index].accum_counters.vidx_instr[sew];
		}
	}else if (is_type(instr->type, T_VSETVL)){ 
		++cpus_state[cpu_index].accum_counters.vsetvl_instr;
	}

	thread_timestamp++;
	//TODO: Use an atomic here?
	if (thread_timestamp > timestamp) timestamp = thread_timestamp;

	//Update state
	cpus_state[cpu_index].timestamp = thread_timestamp;
	cpus_state[cpu_index].last_row = row;
	cpus_state[cpu_index].print_first_scalar = 0;
	cpus_state[cpu_index].last_was_vsetvl = is_type(instr->type, T_VSETVL);


#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_vcpu_exe += time2-time1;
	num_vcpu_exe++;
#endif

}



static void vcpu_rave_event(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)insn_opcode_void;
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif
	if (!TRACE_ENABLED) return;

	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;


	int qemu_trace_event = qemu_get_xreg(cpu,src1);
	int qemu_trace_value = qemu_get_xreg(cpu,src2);

	set_lock(write_lock);
	rave_eventandcounters(qemu_trace_event, qemu_trace_value, cpu_index, &cpus_state[cpu_index].accum_counters);
	if (PRINT_PRV){
		/*
		if (parallel_region.master_thread == -1){ //Not in a parallel region -> Propagate event to all threads
			for(int cpu_id = 0; cpu_id < alloc_threads; ++cpu_id){
				trace_row(mpi_rank, cpu_id, SCALAR_ROW, timestamp);
				trace_event_value(qemu_trace_event,qemu_trace_value);
				if (!PRINT_SCALAR) trace_event_value(event_instruction, 1000);
				trace_row(mpi_rank, cpu_id, VECTOR_ROW, timestamp);
				trace_event_value(qemu_trace_event,qemu_trace_value);
			}
		}else{ //In a parallel region -> Event is local to this thread
		*/
			uint64_t thread_timestamp = cpus_state[cpu_index].timestamp;
			trace_row(mpi_rank, cpu_index, SCALAR_ROW, thread_timestamp);
			trace_event_value(qemu_trace_event,qemu_trace_value);
			if (!PRINT_SCALAR) trace_event_value(event_instruction, 1000);
			trace_row(mpi_rank, cpu_index, VECTOR_ROW, thread_timestamp);
			trace_event_value(qemu_trace_event,qemu_trace_value);
		//}
	}
	release_lock(write_lock);

#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_vcpu_control += time2-time1;
	num_vcpu_control++;
#endif
}


//Warning: This assumes only 1 Thread is reading a name
int qemu_name_offset=-1; //-1: wait for name
char qemu_event_name[128];
int qemu_event_number=-1;
int qemu_value_number=-1;
char qemu_event_name_first_digit=1;

static void vcpu_rave_name_event_toggle(unsigned int cpu_index, void * udata){

		if (qemu_name_offset>0){//End of name
			qemu_event_name[qemu_name_offset]='\0';
			if(qemu_value_number!=-1) add_value_to_event(qemu_event_number,qemu_value_number,qemu_event_name);
			else add_event(qemu_event_number,qemu_event_name);
			qemu_name_offset =-1;
			qemu_event_number=-1;
			qemu_value_number=-1;
		}else{ //Start of name
			qemu_event_name_first_digit=1;
			qemu_name_offset = 0;
		}
}

static void vcpu_rave_name_event_value(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)insn_opcode_void;
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	qemu_event_number = qemu_get_xreg(cpu,src1);
	qemu_value_number = qemu_get_xreg(cpu,src2);
}

static void vcpu_rave_name_char(unsigned int cpu_index, uint32_t insn_opcode){

	int value = (insn_opcode>>12)&0x0FFFFF;
		if (qemu_event_name_first_digit==1){
			qemu_event_name[qemu_name_offset] = (char)value;	
			qemu_event_name_first_digit=0;
		}else{
			qemu_event_name[qemu_name_offset++] += (char)(value<<4);
			qemu_event_name_first_digit=1;
		}
}

static void vcpu_parallel_end(unsigned int cpu_index, void * udata){

	//Wait for everyone to cross the last barrier
	while (__sync_val_compare_and_swap(&parallel_region.crossed_barrier, 0, 0) != 0) {;} 
	
	//Substract counters and add to master's
	update_counters(&parallel_region.parallel_region_counter, &parallel_region.last_barrier_counters); //region_c = last_b - region_c
	add_counters(&cpus_state[cpu_index].accum_counters, &parallel_region.parallel_region_counter);// master_thread += region_c
	parallel_region.master_thread = -1;
}

static void vcpu_parallel_begin(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)insn_opcode_void;
	
	//Atomicity assumed (only on thread active when this happens -> No nested parallel regions
	//TODO: Check this assumption, act accordingly
	parallel_region.master_thread = cpu_index;
	
	//Build barrier
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int parallelism = qemu_get_xreg(cpu,src1);
	parallel_region.n_threads = parallelism;

	//Allocate more threads if needed (It shouldn't cause a race condition here)
	if (parallelism > alloc_threads){
		alloc_threads = parallelism;
		cpus_state = (thread_state_t*)realloc(cpus_state, sizeof(thread_state_t)*alloc_threads);
	}

	parallel_region.in_barrier = 0;
	parallel_region.crossed_barrier = 0; 
	parallel_region.barrier_time = 0;
	parallel_region.first_barrier = 1;
}

static void vcpu_parallel_barrier(unsigned int cpu_index, void * udata){

	//Wait if the previous barrier has not been crossed by other threads
	while (__sync_val_compare_and_swap(&parallel_region.crossed_barrier, 0, 0) != 0) {sched_yield();} 

	//Counters
	if (cpu_index == parallel_region.master_thread){
		//Master sets to 0 the last_barrier_counters (the other threads will accumulate when they exit the barrier)
		reset_counters(&parallel_region.last_barrier_counters);
	}

	//Set max barrier time
	while (1) {
		int old_tmax = parallel_region.barrier_time; // Read the current tmax
		if (cpus_state[cpu_index].timestamp<= old_tmax) break; // No need to update if the thread's t is not greater than tmax

		// Atomically update tmax if it has not changed
		if (__sync_val_compare_and_swap(&parallel_region.barrier_time, old_tmax, cpus_state[cpu_index].timestamp) == old_tmax) break; // Successful update, exit loop
	}
	
	//Add counters to parallel region
	if (cpu_index != parallel_region.master_thread){
		//Critical region! Locking
		set_lock(parallel_region.lock);
		add_counters(&parallel_region.last_barrier_counters, &cpus_state[cpu_index].accum_counters);
		release_lock(parallel_region.lock);
	}
	
	//Increase the barrier (+1)
	__sync_fetch_and_add(&parallel_region.in_barrier, 1);

	//Wait until the barrier is equal to n_threads
	while (__sync_val_compare_and_swap(&parallel_region.in_barrier, parallel_region.n_threads, parallel_region.n_threads) != parallel_region.n_threads) {sched_yield();} 


	//Cross the barrier (+1). If everyone crossed it, enter if: 
	if (__sync_add_and_fetch(&parallel_region.crossed_barrier, 1) == parallel_region.n_threads){
		//If first barrier, set its counters
		if (parallel_region.first_barrier){
			parallel_region.first_barrier = 0;
			copy_counters(&parallel_region.parallel_region_counter, &parallel_region.last_barrier_counters);
		}

		//**Afterwards** Unlock barrier, so next can start
		parallel_region.crossed_barrier = 0;
		parallel_region.in_barrier = 0;
	}
	cpus_state[cpu_index].need_align = 2;
}


static void vcpu_restart_trace(unsigned int cpu_index, void *udata){
	//restart prv
	if (PRINT_PRV){
		FD_PRV = freopen(NULL, "w+", FD_PRV);
		if (N_THREADS > expected_threads) expected_threads = N_THREADS;
		write_prv(FD_PRV, 1, &expected_threads, 2); 
		trace_row(0, 0, SCALAR_ROW, 0);
		trace_event_value(event_VLEN,RAVE_VLMAX);
		trace_event_value(event_ELEN,RAVE_ELEN);

	}
	//restart global region
	//global_region -> closed = 0;
	//reset_counters(&global_region->counters);

	timestamp=0;
	for(int i=0; i<N_THREADS; ++i){
		reset_thread(&cpus_state[i]);
	}
	TRACE_ENABLED=1;
}

static void vcpu_start_trace(unsigned int cpu_index, void *udata){
	for(int i=0; i<N_THREADS; ++i){
		cpus_state[i].print_first_scalar = 1;
	}
	TRACE_ENABLED=1;
}

static void vcpu_stop_trace(unsigned int cpu_index, void *udata){
	TRACE_ENABLED=0;
	if (PRINT_PRV){
		trace_row(mpi_rank, cpu_index, SCALAR_ROW, timestamp);
		clean_event(FD_PRV); 
		trace_row(mpi_rank, cpu_index, VECTOR_ROW, timestamp);
		clean_event(FD_PRV); 
	}
}


//API
char is_rave_api(uint32_t insn_opcode, struct qemu_plugin_insn * insn){
	if (insn_opcode == 0xffe00013){ //li x0, -2 (restart trace)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_restart_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
	}else if (insn_opcode == 0xffd00013){ //li x0, -3 (start trace)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_start_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
	}else if (insn_opcode == 0xffc00013){ //li x0, -4 (stop trace)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_stop_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
	}else if (insn_opcode == 0xffb00013){ //li x0, -5 (parallel_barrier)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_barrier, QEMU_PLUGIN_CB_NO_REGS, NULL);
	}else if (((insn_opcode&0xFFF)==0x033) && (((insn_opcode>>12)&0x7) == 0x4)){ //xor x0, ..., ... (parallel begin)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_begin, QEMU_PLUGIN_CB_R_REGS, (void *)insn_opcode);
	}else if (insn_opcode == 0xffa00013){ //li x0, -6 (parallel_end)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_end, QEMU_PLUGIN_CB_NO_REGS, NULL);
	}else if (((insn_opcode&0xFFF)==0x033) && (((insn_opcode>>12)&0x7) == 0x7)){ //and x0, ..., ... (name event value)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_name_event_value, QEMU_PLUGIN_CB_NO_REGS, (void *)insn_opcode);
	}else if (insn_opcode==0xfff00013){ //li x0, -1 (start/stop name)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_name_event_toggle, QEMU_PLUGIN_CB_NO_REGS, NULL);
	}else if ((insn_opcode&0xFFF)==0x037){ //lui x0, ... (name[i])
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_name_char, QEMU_PLUGIN_CB_NO_REGS, (void *)insn_opcode);
	}else if (((insn_opcode&0xFFF)==0x033) && (((insn_opcode>>12)&0x7) == 0x6)){ // or x0, ..., ... (qemu event)
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_event, QEMU_PLUGIN_CB_NO_REGS, (void *)insn_opcode);
	}else{
		return 0;
	}
	return 1;
}

//TODO:  Make cache bigger than 1 element
//TODO: Thread safety!
int last_cache_length = 0;
char * last_cache_symbol = NULL; //[32]={' ', ' '};
int last_cache_symbol_id = -1;

int reached_main = 0;

int ignore_symbols = 0;
int unlock_length = 0;
char * unlock_ignore = NULL; //[32];

#define update_saved_string(saved,oldlen,new)\
{\
	int newlen = strlen(new)+1;\
	if (newlen > oldlen){\
		if (last_cache_symbol!=NULL) free(saved);\
		oldlen = newlen;\
		saved = malloc(newlen);\
	}\
	strcpy(saved,new);\
}

int add_symbol(char * mangled){
	if (mangled == NULL) return SYMBOL_EMPTY;

	//Remember last symbol
	if (last_cache_symbol != NULL && !strcmp(mangled,last_cache_symbol)){
	 	return last_cache_symbol_id;
	}
	//For cache
	update_saved_string(last_cache_symbol,last_cache_length,mangled);

	int id;
	id = add_value_name_to_event(1001, mangled);
	if (SYMBOL_MAIN==-1 && !strcmp(mangled,"main")) SYMBOL_MAIN = id;
	if (SYMBOL_DL ==-1 && !strcmp(mangled,"_dl_fixup")) SYMBOL_DL = id;

	last_cache_symbol_id = id;
	return id;
}


/**
 * On translation block new translation
 *
 * QEMU convert code by translation block (TB). By hooking here we can then hook
 * a callback on each instruction and memory access.
 */

#include "my_decode.h"

static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif

	struct qemu_plugin_insn *insn;
	uint64_t insn_vaddr;
	uint32_t insn_opcode;
	char *insn_disas;

	size_t n = qemu_plugin_tb_n_insns(tb);

	if (!PRINT_PRV && !PRINT_LOGFILE && !PRINT_REPORT && !PRINT_CSV){
		return;
	}

	for (size_t i = 0; i < n; i++) {
		/*
		 * `insn` is shared between translations in QEMU, copy needed data here.
		 * `output` is never freed as it might be used multiple times during
		 * the emulation lifetime.
		 * We only consider the first 32 bits of the instruction, this may be
		 * a limitation for CISC architectures.
		 */
		insn = qemu_plugin_tb_get_insn(tb, i);
		insn_vaddr = qemu_plugin_insn_vaddr(insn);
		insn_opcode = *((uint32_t *)qemu_plugin_insn_data(insn));
		insn_disas = qemu_plugin_insn_disas(insn);

		char is_illegal = contains_string(insn_disas,"ill");

		//Dissassembly
		char my_disas[64];
		char is_vector=0;
		if (is_illegal){ //illegal instruction (vector, if we are on 0.7) 
			MyDissasembler(my_disas, insn_opcode);
			free(insn_disas);
			insn_disas = my_disas;
			is_vector=1;
		}else{
			#ifdef EPI_07
			is_vector = contains_string(insn_disas," v");
			#else
			is_vector = insn_disas[0] == 'v';
			#endif
		}

		if (is_vector){ //This includes vsetvl
			instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
			if (DETECT_SYMBOLS) insn_struct -> symbol_id = add_symbol(qemu_plugin_insn_symbol(insn));
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
		}else if (!is_rave_api(insn_opcode, insn)){
			if (PRINT_SCALAR){
				instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
				if (DETECT_SYMBOLS) insn_struct -> symbol_id = add_symbol(qemu_plugin_insn_symbol(insn));
				qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
			}else{ 
				instr_basic_data * insn_struct = (instr_basic_data *)malloc(sizeof(instr_basic_data));
				insn_struct->type = T_SCALAR; insn_struct->instr32  = insn_opcode; insn_struct->PC = insn_vaddr; 
				if (DETECT_SYMBOLS) insn_struct -> symbol_id = add_symbol(qemu_plugin_insn_symbol(insn));
				qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
			}
		}
	}
#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_trans += time2-time1;
	num_trans++;
#endif
}

static void newthread_cb(){
#if 0
	if (!alloc_threads){
		alloc_threads = 1;
		cpus_state = (thread_state_t*)malloc(sizeof(thread_state_t)*alloc_threads);
	}else if (alloc_threads < N_THREADS+1){
#else
	if (alloc_threads < N_THREADS+1){
#endif
		printf("RAVE tried to to generate more threads (%d) than allocated (%d)\n", N_THREADS+1, alloc_threads);
		printf("To allocate more threads, set the environment variable \"RAVE_MAX_THREADS\" to the desired value\n");
		printf("\t - RAVE will allocate the maximum between OMP_NUM_THREADS and RAVE_MAX_THREADS\n");
		exit(-1);
		//alloc_threads *= 2;
		//cpus_state = (thread_state_t*)realloc(cpus_state, sizeof(thread_state_t)*alloc_threads);
	}
	reset_thread(&cpus_state[N_THREADS]);
	++N_THREADS;
}

/**
 * On plugin exit, print last instruction in cache
 */
static void plugin_exit(qemu_plugin_id_t id, void *p)
{
		rave_counters global_counters;
		reset_counters(&global_counters);
		double * global_counters_ptr = (double *)&global_counters;
		for(int i=0; i<N_THREADS; ++i){
			double * thread_counters_ptr = (double *)&cpus_state[i].accum_counters; 
			for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
				global_counters_ptr[c] += thread_counters_ptr[c]; 
			}
		}

		rave_eventandcounters(-1, 0, -1, &global_counters); //End Global event
		if (DETECT_SYMBOLS) rave_eventandcounters(1001, 0, -1, &global_counters); //End Symbols event
		if(PRINT_REPORT){
			print_report(FD_REPORT);
		}
		if (PRINT_CSV){
			print_csv(FD_CSV);
		}
		
		free_regions();
#ifdef TIMEDEBUG
		printf("Cycles in Translation: %.4f %d times, %lu (%.2f)\n", (double)time_trans/num_trans, num_trans, time_trans, (double)time_trans/(time_trans+time_vcpu_exe+time_vcpu_control));
		printf("Cycles in VCPU_exe: %.4f %d times, %lu (%.2f)\n", (double)time_vcpu_exe/num_vcpu_exe, num_vcpu_exe, time_vcpu_exe, (double)time_vcpu_exe/(time_trans+time_vcpu_exe+time_vcpu_control));
		printf("Cycles in VCPU_event: %.4f %d times, %lu (%.2f)\n", (double)time_vcpu_control/num_vcpu_control, num_vcpu_control, time_vcpu_control, (double)time_vcpu_control/(time_trans+time_vcpu_exe+time_vcpu_control));
#endif

		if (PRINT_PRV){
			//Align end of trace
			if (TRACE_ENABLED){
				for(int i=0; i<N_THREADS; ++i){
					if (cpus_state[i].timestamp > 0){
						trace_row(mpi_rank, i, cpus_state[i].last_row, cpus_state[i].timestamp+1);
						clean_event(FD_PRV); 
					}
				}
			}

			int fd = fileno(FD_PRV);
			fclose(FD_PRV);
			file_lock(fd, LOCK_UN); //Unlock PRV

			if (mpi_rank > 0){
				//Write NTHREADS to file
				fprintf(FD_COMM,"%d\n",N_THREADS);
				fd=fileno(FD_COMM);
				fclose(FD_COMM);
				file_lock(fd, LOCK_UN); //Unlock COMM
			}else if (mpi_rank == 0){
				//Read COMM from others
				int * N_THREADS_all = (int *)malloc(sizeof(int)*mpi_size);
				N_THREADS_all[0] = N_THREADS;
				for(int i=1; i<mpi_size; ++i){
						char * rank_comm_file = malloc(snprintf(NULL, 0, "%s-%d.com", filename,i));
						sprintf(rank_comm_file, "%s-%d.com", filename,i);

						FD_COMM = fopen(rank_comm_file, "r");
						file_lock(fileno(FD_COMM), LOCK_EX); //Wait for lock on COM
						//Read N_THREADS
						int n_threads_rank=0;
						fscanf(FD_COMM, "%d\n", &n_threads_rank);
						N_THREADS_all[i] = n_threads_rank;

						file_lock(fileno(FD_COMM), LOCK_UN); //Unlock COMM
						fclose(FD_COMM);
						remove(rank_comm_file);
						free(rank_comm_file);
				}

				//Write ROW
				int len = strlen(filename)+1+4;
				char * ext_filename = malloc(len);
				sprintf(ext_filename, "%s.row", filename);
				open_file(&FD_ROW, ext_filename);
				write_row(FD_ROW, mpi_size, N_THREADS_all, 2);
				fclose(FD_ROW);	

				//Write PCF
				sprintf(ext_filename, "%s.pcf", filename);
				open_file(&FD_PCF, ext_filename);
				events_and_values_to_pcf(FD_PCF);
				write_pcf(FD_PCF);
				fclose(FD_PCF);

				free(ext_filename);

				//Rewrite header when more than 1 cpu was used (either MPI, OMP, or both)
				if (mpi_size > 1 || N_THREADS > expected_threads){ 
					FILE * FD_NEWPRV;
					char namebuff[32];
					sprintf(namebuff, "tmpfile-%d", getpid());
					open_file(&FD_NEWPRV, namebuff);
					write_prv(FD_NEWPRV, mpi_size, N_THREADS_all, 2); // write header
					trace_row(0, 0, SCALAR_ROW, 0);
					trace_event_value(event_VLEN,RAVE_VLMAX);
					trace_event_value(event_ELEN,RAVE_ELEN);


					#define PRV_BUFFSIZE 2048
					char buff[PRV_BUFFSIZE];

					//Merging
					for(int i=0; i<mpi_size; ++i){
						fprintf(FD_NEWPRV, "\n");

						char * rank_file;
						if (mpi_size==1){
							rank_file = malloc(snprintf(NULL, 0, "%s.prv", filename));
							sprintf(rank_file, "%s.prv", filename);
						}else{
							rank_file = malloc(snprintf(NULL, 0, "%s-%d.prv", filename, i));
							sprintf(rank_file, "%s-%d.prv", filename, i);
						}

						FD_PRV = fopen(rank_file, "r");
						file_lock(fileno(FD_PRV), LOCK_EX); //Wait for lock on PRV

						int found_newline=0;
						int r;
						//Skip header
						while (!found_newline && (r=fread(buff, 1, PRV_BUFFSIZE, FD_PRV))){
							for(int i=0; i<r; ++i){
								if (buff[i]=='\n'){
									found_newline=1;
									fwrite(&buff[i+1], 1, r-i-1, FD_NEWPRV);
									break;
								}
							}
						}
						//Copy PRV
						while ((r=fread(buff, 1, PRV_BUFFSIZE, FD_PRV)))	fwrite(buff, 1, r, FD_NEWPRV);

						fclose(FD_PRV);
						file_lock(fileno(FD_PRV), LOCK_UN); //Unlock PRV
						remove(rank_file);
						free(rank_file);
					}
					fclose(FD_NEWPRV);

					char * main_file = malloc(snprintf(NULL, 0, "%s.prv", filename));
					sprintf(main_file, "%s.prv", filename);
	    		rename(namebuff, main_file);
				}
			}
			free(filename);
		}
}

/**
 * Install the plugin
 */
QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
                                           const qemu_info_t *info, int argc,
                                           char **argv)
{
	char * RAVE_VLEN = getenv("RAVE_VLEN");
	RAVE_VLMAX = RAVE_VLEN==NULL? 16384 : atoi(RAVE_VLEN);

	parallel_region.master_thread = -1;
	//long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	//printf("nprocs: %d\n", nprocs);
	char * OMP_NUM_THREADS = getenv("OMP_NUM_THREADS");
	int omp_threads = OMP_NUM_THREADS==NULL? 1 : atoi(OMP_NUM_THREADS);
	char * RAVE_MAX_THREADS = getenv("RAVE_MAX_THREADS");
	int rave_threads = RAVE_MAX_THREADS==NULL ? 1 : atoi(RAVE_MAX_THREADS);
	alloc_threads = omp_threads > rave_threads ? omp_threads : rave_threads;

	char * world_rank = getenv("OMPI_COMM_WORLD_SIZE");
	if (world_rank!=NULL && alloc_threads < 3) alloc_threads += 2; //Mpi process adds two threads
	mpi_size = world_rank==NULL? 1 : atoi(world_rank);

	expected_threads = alloc_threads;
	cpus_state = (thread_state_t*)malloc(sizeof(thread_state_t)*alloc_threads);

	for(int i=0; i<argc; ++i){
			if (contains_string(argv[i], "PRINT_SCALAR")) PRINT_SCALAR = 1;
			else if (contains_string(argv[i], "PRINT_ADDR")) PRINT_ADDR = 1;
			else if (contains_string(argv[i], "PRINT_PRV")) PRINT_PRV = 1;
			else if (contains_string(argv[i], "PRINT_LOGFILE")) PRINT_LOGFILE = 1;
			else if (contains_string(argv[i], "PRINT_REPORT")) PRINT_REPORT = 1;
			else if (contains_string(argv[i], "PRINT_CSV")) PRINT_CSV = 1;
			else if (contains_string(argv[i], "DETECT_SYMBOLS")){
			 	DETECT_SYMBOLS = 1;
				add_event(1001, "symbols");
			}else if (contains_string(argv[i], "PRV_NAME")){
#if 1
							int j; for(j=0; j<strlen(argv[i]); ++j)	if (argv[i][j] == '=') break;

							int l_filename = strlen(&argv[i][j+1]);
							filename = malloc(l_filename+1);
							strcpy(filename, &argv[i][j+1]);

							int l_ext = 4; //.prv, .pcf, .row

							//Check for MPI
							char * prv_filename;
							if (mpi_size > 1){
							//if (world_rank != NULL && strcmp(world_rank,"1")){
								char * rank = getenv("OMPI_COMM_WORLD_RANK");
								mpi_rank=atoi(rank);
								int l_rank = strlen(rank);
								prv_filename = malloc(l_filename+l_rank+l_ext+1);
								strcpy(prv_filename, &argv[i][j+1]);
								sprintf(&prv_filename[l_filename],"-%s.prv",rank);
								open_file(&FD_PRV, prv_filename);
								file_lock(fileno(FD_PRV), LOCK_EX); //Lock PRV for this process

								//Communications file
								if (mpi_rank > 0){
									sprintf(&prv_filename[l_filename],"-%s.com",rank);
									open_file(&FD_COMM,prv_filename);
									file_lock(fileno(FD_COMM), LOCK_EX); //Lock COM for this process
								}
							}else{
								prv_filename = malloc(l_filename+l_ext+1);
								strcpy(prv_filename, &argv[i][j+1]);
								strcpy(&prv_filename[l_filename],".prv");
								open_file(&FD_PRV, prv_filename);
								file_lock(fileno(FD_PRV), LOCK_EX); //Lock PRV for this process
							}
							free(prv_filename);

							//setup_paraver_trace(filename);
							write_prv(FD_PRV, 1, &expected_threads, 2);
							trace_row(0, 0, SCALAR_ROW, 0);
							trace_event_value(event_VLEN,RAVE_VLMAX);
							trace_event_value(event_ELEN,RAVE_ELEN);
#endif
			}
			else if (contains_string(argv[i], "CSV_NAME")){
				//++i;
				int j; for(j=0; j<strlen(argv[i]); ++j) if (argv[i][j] == '=') break;
				char * world_rank = getenv("OMPI_COMM_WORLD_SIZE");
				mpi_size = world_rank==NULL? 1 : atoi(world_rank);
				if (mpi_size > 1){
					char * rank = getenv("OMPI_COMM_WORLD_RANK");
					mpi_rank=atoi(rank);
					int l_rank = strlen(rank);
					int l_filename = strlen(&argv[i][j+1]);
					char * csv_filename = malloc(l_filename+l_rank+1);
					strcpy(csv_filename, &argv[i][j+1]);
					sprintf(&csv_filename[l_filename],"-%s",rank);
					FD_CSV = fopen(csv_filename, "w+");
				}else{
					FD_CSV = fopen(&argv[i][j+1], "w+");
				}
			}
			else if (contains_string(argv[i], "REPORT_NAME")){
				int j; for(j=0; j<strlen(argv[i]); ++j) if (argv[i][j] == '=') break;
				FD_REPORT = fopen(&argv[i][j+1], "w");
			}
	}
	if (PRINT_REPORT && FD_REPORT==NULL) FD_REPORT = stdout; 

	add_event(-1,"Global");
	rave_counters global_counters;
	reset_counters(&global_counters);
	rave_eventandcounters(-1, 1, -1, &global_counters); //Start global event

    /* Register translation block and exit callbacks */
    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);

		qemu_plugin_register_vcpu_init_cb(id, newthread_cb);

    return 0;
}

