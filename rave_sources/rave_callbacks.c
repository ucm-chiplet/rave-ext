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
				if (TRACE_ENABLED && PRINT_PRV && !MUSA){
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

	if (TRACE_ENABLED && PRINT_PROFILE){
		//Loop profiling
		if (cpus_state[cpu_index].next_PC!=-1){
			if (instr->PC != cpus_state[cpu_index].next_PC){ //Loop not taken
				update_PC(cpus_state[cpu_index].loop_PC - base, cpus_state[cpu_index].loop_weight);
				cpus_state[cpu_index].loop_weight = 0;
			}
			cpus_state[cpu_index].next_PC=-1;
		}
		//Detect loop
		int insn_opcode = instr->instr32;
		if ((insn_opcode&0x7F) == 0x063){
			int highest = ((insn_opcode>>31)&0x1);
			if (highest){ //Is it backwards?
										//printf("Loop on %lx (base is %lx)\n", cpus_state[cpu_index].loop_PC - base, base);
				int64_t offset = (((insn_opcode>>31)&0x1)<<12) + (((insn_opcode>>7)&0x1)<<11) + (((insn_opcode>>25)&0x3F)<<5) + (((insn_opcode>>8)&0xF)<<1);
				//Sign extend the 13 bit number
				offset <<= (64-13);
				offset >>= (64-13);
				cpus_state[cpu_index].loop_PC = instr->PC;
				cpus_state[cpu_index].next_PC = instr->PC + offset;
			}
		}
	}

	int row = SCALAR_ROW;
	uint64_t vl=0, vtype, sew=3, lmul=1, rvl=0;
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

			if (TRACE_ADDR){
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
	}else if (is_type(instr->type, T_VSETVL)){ 
		uint8_t *cpu = qemu_get_cpu(cpu_index);
		int src1 = (instr->instr32>>15)&0x1F;
		rvl = qemu_get_xreg(cpu,src1);
	}



	//  Logfile  //
	if (TRACE_ENABLED){
		if (PRINT_LOGFILE){
			if (!TRACE_SCALAR && !is_type(instr->type, T_SCALAR) && cpus_state[cpu_index].scalar_instr_since_vector>0){
				char * string = g_strdup_printf("%d scalar instructions\n", cpus_state[cpu_index].scalar_instr_since_vector); 
				set_lock(write_lock);
				qemu_plugin_outs(string);
				if (!is_type(instr->type, T_SCALAR) || TRACE_SCALAR){ 
					qemu_plugin_outs(instr->asm_string);
					qemu_plugin_outs("\n");
				}
				release_lock(write_lock);
				free(string);
			}else{
				if (!is_type(instr->type, T_SCALAR) || TRACE_SCALAR){ 
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
			if (row_change && !MUSA){ 
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, cpus_state[cpu_index].last_row, thread_timestamp);
				/*
					 if (row == SCALAR_ROW){
					 clean_event_vector(FD_PRV);
					 }
					 else{
					 clean_event_scalar(FD_PRV);
					 }
					 */
				clean_event(FD_PRV);
				release_lock(write_lock);
			}
			//Scalar instructions should always be printed when: row changed(1), type changed (2), is first scalar in the trace (3)
			if (is_type(instr->type, T_SCALAR) && !TRACE_SCALAR){
				if (row_change || cpus_state[cpu_index].last_was_vsetvl || cpus_state[cpu_index].print_first_scalar){	
					set_lock(write_lock);
					trace_row(mpi_rank, cpu_index, row, thread_timestamp);
					trace_event_value(event_instruction,PRV_SCALAR*!MUSA);
					trace_event_value(event_class,instr->type);
					trace_event_value(event_pc, instr->PC);
					if (MUSA){ //for MUSA
						int prev_dst = cpus_state[cpu_index].prev_dst;
						cpus_state[cpu_index].prev_dst = 0;
						trace_event_value(event_scalb, 0);
						if (TRACE_ADDR) trace_event_value(event_addr, addr);
						trace_event_value(event_dst, prev_dst);
						trace_event_value(event_src1, 0);
						trace_event_value(event_src2, 0);
						trace_event_value(event_vl, 0);
						trace_event_value(event_sew, 0);
						trace_event_value(event_lmul, 0);
					}
					release_lock(write_lock);
				}
			}else if (is_type(instr->type, T_VSETVL)){
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(event_class,instr->type);
				trace_event_value(event_pc, instr->PC);
				trace_event_value(event_instruction, instr->paraver_code);
				if (MUSA){
					int prev_dst = cpus_state[cpu_index].prev_dst;
					cpus_state[cpu_index].prev_dst = instr->dst;
					trace_event_value(event_dst, prev_dst);
				}else{
					trace_event_value(event_dst, instr->dst);
				}
				trace_event_value(event_src1, instr->src1);
				trace_event_value(event_rvl, rvl); 
				release_lock(write_lock);
			}else{ //TRACE_SCALAR || (instr!=SCALAR && instr!=VSETVL)
				set_lock(write_lock);
				trace_row(mpi_rank, cpu_index, row, thread_timestamp);
				trace_event_value(event_class,instr->type);
				trace_event_value(event_pc, instr->PC);
				trace_event_value(event_scalb, cpus_state[cpu_index].scalar_instr_since_vector);
				if (TRACE_ADDR) trace_event_value(event_addr, addr);
				if (MUSA){
					int prev_dst = cpus_state[cpu_index].prev_dst;
					cpus_state[cpu_index].prev_dst = instr->dst;
					trace_event_value(event_dst, prev_dst);
				}else{
					trace_event_value(event_dst, instr->dst);
				}
				trace_event_value(event_src1, instr->src1);
				trace_event_value(event_src2, instr->src2);
				trace_event_value(event_instruction, instr->paraver_code);
				trace_event_value(event_vl, vl);
				if (MUSA){
					trace_event_value(event_sew, 1<<(3+sew));
					int musa_lmul = lmul<4 ? 1<<lmul : lmul==5?18 : lmul==6?14 : lmul==7?12 : 0;
					trace_event_value(event_lmul, musa_lmul);
				}else{
					trace_event_value(event_sew, sew);
					trace_event_value(event_lmul, lmul);
				}
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
		if (is_subsubsubtype(instr->type, T_FUSED)) cpus_state[cpu_index].accum_counters.scalarflops += 2;
		else if (is_subsubsubtype(instr->type, T_SINGLE)) cpus_state[cpu_index].accum_counters.scalarflops += 1;

		int opcode = (instr->instr32 & 0x3F);
		if (opcode == 0b0000011 || opcode == 0b0100011 || opcode == 0b0000111 || opcode == 0b0100111){ 
			int width = (instr->instr32 >> 12)&0x3; //3 instead of 7 to %4
			cpus_state[cpu_index].accum_counters.moved_bytes_s += (1<<(width));
			//B 0, 4
			//H 1, 5
			//W 2, 6
			//D 3
		} 
		cpus_state[cpu_index].loop_weight += 1;
	}else if (is_type(instr->type, T_VECTOR)) {
		cpus_state[cpu_index].loop_weight += vl;

		cpus_state[cpu_index].scalar_instr_since_vector=0;
		++cpus_state[cpu_index].accum_counters.vector_instr[sew];
		cpus_state[cpu_index].accum_counters.velem[sew] += vl;
		if (is_subtype(instr->type, T_ARITH)){
			if (is_subsubtype(instr->type, T_FP)){
				++cpus_state[cpu_index].accum_counters.vfp_instr[sew];
				cpus_state[cpu_index].accum_counters.velem_arith[sew] += vl; 
				if (is_subsubsubtype(instr->type, T_FUSED)) cpus_state[cpu_index].accum_counters.vectorflops += 2*vl; 
				else cpus_state[cpu_index].accum_counters.vectorflops += vl;
			}else if (is_subsubtype(instr->type, T_INT)){
				++cpus_state[cpu_index].accum_counters.vint_instr[sew];
				cpus_state[cpu_index].accum_counters.velem_arith[sew] += vl; 
			}
		}else if (is_subtype(instr->type, T_REDUCTION)){
			cpus_state[cpu_index].accum_counters.velem_reductions[sew] += vl;
			if (is_subsubtype(instr->type, T_FP)){
				++cpus_state[cpu_index].accum_counters.vfp_reductions[sew];
			}else if (is_subsubtype(instr->type, T_INT)){
				++cpus_state[cpu_index].accum_counters.vint_reductions[sew];
			}
		}else if (is_subtype(instr->type, T_MASK)){
			++cpus_state[cpu_index].accum_counters.vmask_instr[sew];
			cpus_state[cpu_index].accum_counters.velem_mask[sew] += vl; 
		}else if (is_subtype(instr->type, T_MEMORY)){
			cpus_state[cpu_index].accum_counters.moved_bytes_v += vl*(1<<(sew));
			cpus_state[cpu_index].accum_counters.velem_mem[sew] += vl; 
			if (is_subsubtype(instr->type, T_UNIT)) ++cpus_state[cpu_index].accum_counters.vunit_instr[sew];
			else if (is_subsubtype(instr->type, T_STRIDE)){
				++cpus_state[cpu_index].accum_counters.vstride_instr[sew];
				cpus_state[cpu_index].accum_counters.agg_strides[sew] += stride;
			}
			else if (is_subsubtype(instr->type, T_INDEX)) ++cpus_state[cpu_index].accum_counters.vidx_instr[sew];
			else if (is_subsubtype(instr->type, T_SPILL)) ++cpus_state[cpu_index].accum_counters.vspill_instr[sew];
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

static void vcpu_rave_event_string(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	char data[128];
	rave_read_string(cpu_index, insn_opcode, data, 128);
	add_event(cpus_state[cpu_index].rave_event_number,data); 
}
static void vcpu_rave_value_string(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	char data[128];
	rave_read_string(cpu_index, insn_opcode, data, 128);
	add_value_to_event(cpus_state[cpu_index].rave_event_number,cpus_state[cpu_index].rave_value_number,data); 
}
static void vcpu_rave_begin_region(unsigned int cpu_index, void * insn_opcode_void){
	if (!REGIONS_ENABLED) return;
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	char data[128];
	rave_read_string(cpu_index, insn_opcode, data, 128);
	rave_begin_region(cpu_index, data, &cpus_state[cpu_index].accum_counters, ACCUM_REGIONS);
	region_trace(cpu_index, REGION_EVENT+track_regions.nesting-1, name_to_id(data));
	if (PRINT_LOGFILE){
		char * string = g_strdup_printf("Begin region %s\n", data); 
		set_lock(write_lock);
		qemu_plugin_outs(string);
		release_lock(write_lock);
		free(string);
	}
}
static void vcpu_rave_end_region(unsigned int cpu_index, void * insn_opcode_void){
	if (!REGIONS_ENABLED) return;
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	char data[128];
	rave_read_string(cpu_index, insn_opcode, data, 128);
	rave_end_region(cpu_index, data, &cpus_state[cpu_index].accum_counters, ACCUM_REGIONS, STREAM_REPORT?FD_REPORT:NULL);
	region_trace(cpu_index, REGION_EVENT+track_regions.nesting, 0);
	if (PRINT_LOGFILE){
		char * string = g_strdup_printf("End region %s\n", data);
		set_lock(write_lock);
		qemu_plugin_outs(string);
		release_lock(write_lock);
		free(string);
	}
}


static void vcpu_rave_event_and_value(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
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
	//rave_eventandcounters(qemu_trace_event, qemu_trace_value, cpu_index, &cpus_state[cpu_index].accum_counters);
	if (PRINT_PRV){
		/*
			 if (parallel_region.master_thread == -1){ //Not in a parallel region -> Propagate event to all threads
			 for(int cpu_id = 0; cpu_id < alloc_threads; ++cpu_id){
			 trace_row(mpi_rank, cpu_id, SCALAR_ROW, timestamp);
			 trace_event_value(qemu_trace_event,qemu_trace_value);
			 if (!TRACE_SCALAR) trace_event_value(event_instruction, 1000);
			 trace_row(mpi_rank, cpu_id, VECTOR_ROW, timestamp);
			 trace_event_value(qemu_trace_event,qemu_trace_value);
			 }
			 }else{ //In a parallel region -> Event is local to this thread
			 */
		uint64_t thread_timestamp = cpus_state[cpu_index].timestamp;
		trace_row(mpi_rank, cpu_index, SCALAR_ROW, thread_timestamp);
		trace_event_value(qemu_trace_event,qemu_trace_value);
		if (!TRACE_SCALAR) trace_event_value(event_instruction, PRV_SCALAR*!MUSA);
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

static void vcpu_rave_name_event_value(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	cpus_state[cpu_index].rave_event_number = qemu_get_xreg(cpu,src1);
	cpus_state[cpu_index].rave_value_number = qemu_get_xreg(cpu,src2);
}

///////////////////////////////////////////

static void vcpu_parallel_end(unsigned int cpu_index, void * udata){

	//Wait for everyone to cross the last barrier
	while (__sync_val_compare_and_swap(&parallel_region.crossed_barrier, 0, 0) != 0) {;} 

	//Substract counters and add to master's
	update_counters(&parallel_region.parallel_region_counter, &parallel_region.last_barrier_counters); //region_c = last_b - region_c
	add_counters(&cpus_state[cpu_index].accum_counters, &parallel_region.parallel_region_counter);// master_thread += region_c
	parallel_region.master_thread = -1;
}

static void vcpu_parallel_begin(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;

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
		write_prv(FD_PRV, 1, &expected_threads, N_PIPELINES); 
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

static void vcpu_enable_regions(unsigned int cpu_index, void *udata){
	REGIONS_ENABLED=1;
}
static void vcpu_disable_regions(unsigned int cpu_index, void *udata){
	REGIONS_ENABLED=0;
}

static void vcpu_enable_trace(unsigned int cpu_index, void *udata){
	for(int i=0; i<N_THREADS; ++i){
		cpus_state[i].print_first_scalar = 1;
	}
	TRACE_ENABLED=1;
}

static void vcpu_disable_trace(unsigned int cpu_index, void *udata){
	TRACE_ENABLED=0;
	disabled_once=1;
	if (PRINT_PRV){
		trace_row(mpi_rank, cpu_index, SCALAR_ROW, timestamp);
		clean_event(FD_PRV); 
		if (!MUSA){
			trace_row(mpi_rank, cpu_index, VECTOR_ROW, timestamp);
			clean_event(FD_PRV); 
		}
	}
}
