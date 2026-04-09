/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "tb_hook.h"
#include "callbacks.h"
#include "regions.h"
#include "events.h"
#include "threading.h"
#include "instr_data.h"
#include "utils.h"
#include "profiling.h"
#ifdef RVV_07
#include "07_decode.h"
#endif
#include "scalar_blocks.h"
#include "init_exit.h"

void vcpu_scalar_block_exec(unsigned int cpu_index, void *udata){
	scalar_block_data_t * data = (scalar_block_data_t *)udata;
	scalar_block_exec(&cpus_state[cpu_index], data);
}

void vcpu_insn_exec(unsigned int cpu_index, void *udata){
	instr_data * instr = (instr_data*)udata;
	insn_exec(&cpus_state[cpu_index], instr);
}
void vcpu_rave_event_string(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_event_string(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_rave_value_string(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_value_string(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_rave_event_and_value(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_event_and_value(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_rave_name_event_value(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_name_event_value(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_rave_begin_region(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_begin_region(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_rave_end_region(unsigned int cpu_index, void * insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	rave_end_region(insn_opcode, &cpus_state[cpu_index]);
}
void vcpu_parallel_end(unsigned int cpu_index, void * udata){
	parallel_end(cpu_index);
}
void vcpu_parallel_begin(unsigned int cpu_index, void* insn_opcode_void){
	uint32_t insn_opcode = (uint32_t)(uint64_t)insn_opcode_void;
	parallel_begin(cpu_index, insn_opcode);
}
void vcpu_parallel_barrier(unsigned int cpu_index, void * udata){
	parallel_barrier(cpu_index);
}
void vcpu_restart_trace(unsigned int cpu_index, void *udata){
	restart_trace();
}
void vcpu_enable_regions(unsigned int cpu_index, void *udata){
	enable_regions();
}
void vcpu_disable_regions(unsigned int cpu_index, void *udata){
	disable_regions();
}
void vcpu_enable_trace(unsigned int cpu_index, void *udata){
	enable_trace();
}
void vcpu_disable_trace(unsigned int cpu_index, void *udata){
	disable_trace(cpu_index);
}


QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;
void plugin_exit(qemu_plugin_id_t id, void *p)
{
	rave_exit();
}
QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
		const qemu_info_t *info, int argc,
		char **argv)
{

		rave_init(argc,argv);
		qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
		qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);

		qemu_plugin_register_vcpu_init_cb(id, (void (*))newthread_cb);

		return 0;
}

char is_rave_api(uint32_t insn_opcode, struct qemu_plugin_insn * insn){
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

void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif

	struct qemu_plugin_insn *insn;
	uint32_t insn_opcode;
	char *insn_disas;

	size_t n = qemu_plugin_tb_n_insns(tb);

	if (!PRINT_PRV && !PRINT_LOGFILE && !PRINT_REPORT && !PRINT_CSV && !PRINT_PROFILE){
		return;
	}

	if (PRINT_PROFILE && BINARY_NAME != NULL && base==-1){
		init_dwfl(BINARY_NAME);
	}

	scalar_block_data_t * scalar_block_data = NULL;
	int scalar_block_start = 0;
	for (int i = 0; i < n; i++) {
		insn = qemu_plugin_tb_get_insn(tb, i);
		insn_disas = qemu_plugin_insn_disas(insn);
		uint64_t insn_vaddr = qemu_plugin_insn_vaddr(insn);
		int is_vector=0;
#ifdef RVV_07
		char my_disas[64];
		insn_opcode = *((uint32_t *)qemu_plugin_insn_data(insn));
		if (contains_string(insn_disas,"ill")){ //illegal instruction (vector, if we are on 0.7) 
			int extra = sprintf(my_disas, "%08x ", insn_opcode);
			MyDissasembler(&my_disas[extra], insn_opcode);
			free(insn_disas);
			insn_disas = my_disas;
			is_vector=1;
		}
#else
		qemu_plugin_insn_data(insn, &insn_opcode, sizeof(insn_opcode));
		if (insn_disas[0] == 'v') is_vector=1;
#endif		
		if (is_vector){
			//Register scalar block
			if (scalar_block_start != i){
				qemu_plugin_register_vcpu_insn_exec_cb(qemu_plugin_tb_get_insn(tb,scalar_block_start), vcpu_scalar_block_exec, QEMU_PLUGIN_CB_NO_REGS, scalar_block_data);
			}
			//Register vector instruction
			instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode, PRINT_PRV);
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
			//Prepare next scalar block
			scalar_block_start = i+1;
		}
		else if (is_rave_api(insn_opcode, insn)){
			//Register scalar block
			if (scalar_block_start != i){
				qemu_plugin_register_vcpu_insn_exec_cb(qemu_plugin_tb_get_insn(tb,scalar_block_start), vcpu_scalar_block_exec, QEMU_PLUGIN_CB_NO_REGS, scalar_block_data);
			}
			//Prepare next scalar block
			scalar_block_start = i+1;
		}else{ //Scalar
			if (scalar_block_start == i){
				int save_pcs = (PRINT_PROFILE) ? (n-i) : 1 ;
				scalar_block_data = alloc_scalar_block(save_pcs); 
				if (TRACE_SCALAR){
					scalar_block_data->strings = (char**)malloc(sizeof(char*)*(n-i));
				}
			}
			if (scalar_block_start == i || PRINT_PROFILE)scalar_block_data->PCs[i-scalar_block_start] = insn_vaddr; 
			if (TRACE_SCALAR){
				my_strcpy(scalar_block_data->strings[i-scalar_block_start], insn_disas); 
			}
			rolling_scalar_block(insn_opcode, insn_vaddr, scalar_block_data);
		}
	}
	if (scalar_block_start != n){ //Unfinished scalar block
		qemu_plugin_register_vcpu_insn_exec_cb(qemu_plugin_tb_get_insn(tb,scalar_block_start), vcpu_scalar_block_exec, QEMU_PLUGIN_CB_NO_REGS, scalar_block_data);
	}
#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_trans += time2-time1;
	num_trans++;
#endif
}

