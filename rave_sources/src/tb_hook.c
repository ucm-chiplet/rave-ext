/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "tb_hook.h"
#include "callbacks.h"
#include "instr_data.h"
#include "utils.h"
#include "profiling.h"
#ifdef RVV_07
#include "07_decode.h"
#endif

#include "state.h"

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
		#ifdef RVV_07
		insn_opcode = *((uint32_t *)qemu_plugin_insn_data(insn));
		#else
		qemu_plugin_insn_data(insn, &insn_opcode, sizeof(insn_opcode));
		#endif
		insn_disas = qemu_plugin_insn_disas(insn);


		char is_illegal = contains_string(insn_disas,"ill");

		//Dissassembly
		char is_vector=0;
#ifdef RVV_07
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
			instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode, PRINT_PRV);
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
		}else if (!is_rave_api(insn_opcode, insn)){
			if (TRACE_SCALAR){
				instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode, PRINT_PRV);
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
