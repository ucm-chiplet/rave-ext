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
static uint64_t getmicros(){
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
char TRACE_SCALAR = 0;
char TRACE_ADDR = 0;
char PRINT_PRV = 0;
char PRINT_REPORT = 0;
char STREAM_REPORT = 0;
char PRINT_PROFILE = 0;
char PRINT_CSV = 0;
char TRACE_ENABLED = 1; //Enabled by default 
char REGIONS_ENABLED = 1; //Enabled by default 
char ACCUM_REGIONS = 0;
int REGION_EVENT = 1000;
char * filename = NULL; 
char * BINARY_NAME = NULL;
FILE * FD_PRV;
FILE * FD_PCF;
FILE * FD_ROW;
FILE * FD_CSV;
FILE * FD_COMM;
FILE * FD_REPORT;
FILE * FD_PROFILE;

int disabled_once = 0;

//#define EPI_07

#ifdef EPI_07
#undef EPI_10
#else
#define EPI_10
#endif

#define my_strcpy(dst, src)\
{\
	int len = strlen(src);\
	dst = malloc(len+1);\
	strcpy(dst,src);\
}

//The order of the includes is relevant, as they depend on each other
#include "formatting.c"
#include "rave_counters.c"
#include "rave_events.c"
//#include "rave_regions_legacy.c"
#include "rave_regions.c"
#include "profiling.c"
#include "rave2prv.c"
#include "instr_data.c"
#include "rave_utils.c"
///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

/* Store last executed instruction on each vCPU as a GString */

/**
 * Log instruction execution
 */


uint64_t timestamp=0;

//Per-thread info
struct thread_state_t{
	int last_row;
	int reset_stride;
	int last_was_vsetvl;
	int scalar_instr_since_vector;
	int print_first_scalar;
	char need_align;
	uint64_t timestamp;
	rave_counters accum_counters;

	//For loop detection:
	uint64_t loop_PC;
	uint64_t next_PC;
	uint64_t loop_weight;

	//For events
	int rave_event_number;
	int rave_value_number;

	//For MUSA:
	int prev_dst;
};
typedef struct thread_state_t thread_state_t;
thread_state_t * cpus_state;

static void reset_thread(thread_state_t * state){
	state -> last_row = 0;
	state -> reset_stride = 0;
	state -> last_was_vsetvl = 0;
	state -> scalar_instr_since_vector = 0;
	state -> print_first_scalar = 1;
	state -> need_align = 1;
	state -> timestamp = 0;

	//Loop control:
	state -> loop_PC = -1;
	state -> next_PC = -1;
	state -> loop_weight = 0;

	state -> rave_event_number=-1;
	state -> rave_value_number=-1;
	reset_counters(&(state->accum_counters));
	
	//Musa:
	state -> prev_dst = 0;
}

#include "rave_threading.c"


volatile int write_lock = 0;
#define set_lock(lock) if(N_THREADS>1){ while (! __sync_bool_compare_and_swap(&lock, 0, 1)){sched_yield();}}//Wait until lock is 0, then put it to 1
#define release_lock(lock) if (N_THREADS>1) { __sync_val_compare_and_swap(&lock, 1, 0);} //Unlock 
#define file_lock(fd, action) flock(fd,action); 
static void region_trace(unsigned int cpu_index, int event, int value){
	set_lock(write_lock);
	if (PRINT_PRV){
		uint64_t thread_timestamp = cpus_state[cpu_index].timestamp;
		trace_row(mpi_rank, cpu_index, SCALAR_ROW, thread_timestamp);
		trace_event_value(event,value);
		if (!TRACE_SCALAR) trace_event_value(event_instruction, PRV_SCALAR*!MUSA);
		trace_row(mpi_rank, cpu_index, VECTOR_ROW, thread_timestamp);
		trace_event_value(event, value);
	}
	release_lock(write_lock);
}


#include "rave_callbacks.c"

//API
static char is_rave_api(uint32_t insn_opcode, struct qemu_plugin_insn * insn){

	unsigned int dst = (insn_opcode>>7)&0x1F;
	if (dst!=0) return 0;
	unsigned int major = (insn_opcode)&0x7F;
	unsigned int funct3=(insn_opcode>>12)&0x7;
	unsigned int funct6=(insn_opcode>>26)&0x3F;
	int32_t imm = ((int32_t)insn_opcode>>20); 
	//printf("ins 0x%08x → major %02x , f3 %01x, f6 %02x, imm=%d\n",insn_opcode,major,funct3,funct6,imm);
	//----------------------------------------
	// TRACE control
	//----------------------------------------
	if (major == 0x13 && funct3 == 0){
		//li x0, -2 (restart trace)
		if (imm==-2){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_restart_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		//li x0, -3 (enable trace)
		}else if (imm==-3){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_enable_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		//li x0, -4 (disable trace)		
		}else if (imm==-4){ 
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_disable_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		//li x0, -5 (enable regions)
		}else if (imm==-7){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_enable_regions, QEMU_PLUGIN_CB_R_REGS, NULL);
		//li x0, -6 (disable regions)		
		}else if (imm==-8){ 
		qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_disable_regions, QEMU_PLUGIN_CB_R_REGS, NULL);
		}
	}else if (major==0x33){
		// or x0, ..., ... (rave_event_and_value)		
		if (funct3 == 0x6){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_event_and_value, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		//and x0, ..., ... (name event value)		
		}else if (funct3 == 0x7){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_name_event_value, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		// sll x0, ..., ... (action: event string)
		}else if (funct3 == 1 && funct6==0) {
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_event_string, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		// srl x0, ..., ... (action: value string)
		}else if (funct3 == 5 && funct6==0) {
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_value_string, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		// add x0, ..., ... (action: begin region string)
		}else if (funct3 == 0 && funct6==0) {
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_begin_region, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		// sub x0, ..., ... (action: end region string)
		}else if (funct3 == 0 && funct6==0x10) {
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_rave_end_region, QEMU_PLUGIN_CB_NO_REGS, (void *)(uint64_t)insn_opcode);
		}	
	//----------------------------------------
	// OMP control
	//----------------------------------------
	//li x0, -5 (parallel_barrier)		
	}else if (major == 0x13 && funct3 == 0 &&  imm==-5){ 
		qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_barrier, QEMU_PLUGIN_CB_NO_REGS, NULL);
	//xor x0, ..., ... (parallel begin)		
	}else if (major == 0x33 && funct3 == 4 && funct6 == 0){
		qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_begin, QEMU_PLUGIN_CB_R_REGS, (void *)(uint64_t)insn_opcode);
	//li x0, -6 (parallel_end)		
	}else if (major == 0x13 && funct3 == 0 &&  imm==-6){ 
		qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_parallel_end, QEMU_PLUGIN_CB_NO_REGS, NULL);
	}else{
		return 0;
	}
	return 1;
}


/**
 * On translation block new translation
 *
 * QEMU convert code by translation block (TB). By hooking here we can then hook
 * a callback on each instruction and memory access.
 */

#ifdef EPI_07
#include "07_decode.c"
#endif

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

	if (!PRINT_PRV && !PRINT_LOGFILE && !PRINT_REPORT && !PRINT_CSV && !PRINT_PROFILE){
		return;
	}

	if (PRINT_PROFILE && BINARY_NAME != NULL && base==-1){
		init_dwfl(BINARY_NAME);
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
		char is_vector=0;
#ifdef EPI_07
		char my_disas[64];
		if (is_illegal){ //illegal instruction (vector, if we are on 0.7) 
			int extra = sprintf(my_disas, "%08x ", insn_opcode);
			MyDissasembler(&my_disas[extra], insn_opcode);
			free(insn_disas);
			insn_disas = my_disas;
			is_vector=1;
		}else{
			is_vector = contains_string(insn_disas," v");
		}
#else
		if (!is_illegal) is_vector = insn_disas[0] == 'v';
#endif

		if (is_vector){ //This includes vsetvl
			instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
		}else if (!is_rave_api(insn_opcode, insn)){
			if (TRACE_SCALAR){
				instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
				qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
			}else{ 
				instr_basic_data * insn_struct = (instr_basic_data *)malloc(sizeof(instr_basic_data));
				insn_struct->type = T_SCALAR; insn_struct->instr32  = insn_opcode; insn_struct->PC = insn_vaddr; 
				if ((insn_opcode&0x7F) == 0b1010011) insn_struct->type |= T_SINGLE;
				else if ((insn_opcode&0x7F) == 0b1000011) insn_struct->type |= T_FUSED;
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


#include "rave_init_exit.c"
