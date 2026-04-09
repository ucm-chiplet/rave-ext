/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "utils.h"
#include "qemu-plugin.h"

#ifdef RVV_07
//include/hw/core/cpu.h (0.7 :307 (def)) //Util for knowing OFFSET REGS
//qemu_get_cpu returns an ArchCPU, which has a CPURISCVState (typdef of CPUArchState), 
//ArchCPU is defined in target/riscv/cpu.h (l:277)
//CPUArchState is defined in target/riscv/cpu.h (l:114)
//In accel/tcg/plugin-gen.c (l:175) is a good place to put : printf("Offset is %ld\n",offsetof(ArchCPU, env));
#define OFFSET_CPUState (33552) //For 0.7
#define RV_VLEN_MAX (256*64)
#define OFFSET_REGS (sizeof(uint64_t)*32 + sizeof(uint64_t)*32 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
void setup_regs(unsigned int cpu_index){
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	cpus_state[cpu_index].regs = (void*)(cpu + OFFSET_CPUState); 
}
int64_t qemu_get_vl(thread_state_t * state){
	uint8_t * regs = (uint8_t*)state->regs;
	return *(uint64_t*)(regs + OFFSET_REGS + sizeof(uint64_t)*2);
}
int64_t qemu_get_vtype(thread_state_t * state){
	uint8_t * regs = (uint8_t*)state->regs;
	return *(uint64_t*)(regs + OFFSET_REGS + sizeof(uint64_t)*4);
}
int64_t qemu_get_xreg(thread_state_t * state, int reg){
	uint8_t * regs = (uint8_t*)state->regs;
	return *(uint64_t*)(regs + sizeof(uint64_t)*reg); 
}
#else
int idx_xregs;
int idx_vl;
int idx_vtype;
void setup_regs(unsigned int cpu_index){
	//Registers:
	idx_xregs=idx_vl=idx_vtype=-1;

	GArray * regs = qemu_plugin_get_registers();
	for (int i = 0; i < regs->len; i++) {
		qemu_plugin_reg_descriptor *desc = &g_array_index(regs, qemu_plugin_reg_descriptor, i);
		//printf("%d %s\n",i,desc->name);
		if (idx_xregs < 0 && (g_strcmp0(desc->name, "zero")==0)) idx_xregs=i;
		if (idx_vtype < 0 && (g_strcmp0(desc->name, "vtype")==0)) idx_vtype=i;
		if (idx_vl < 0 && (g_strcmp0(desc->name, "vl")==0)) idx_vl=i;
	}
	cpus_state[cpu_index].regs = (void*)regs;
}

int64_t qemu_get_vl(thread_state_t * state){
  qemu_plugin_reg_descriptor *desc = &g_array_index((GArray*)(state->regs), qemu_plugin_reg_descriptor, idx_vl);
	GByteArray *buf = g_byte_array_new();
	qemu_plugin_read_register(desc->handle, buf);
	uint64_t reg_val = *((uint64_t*)buf->data);
  g_byte_array_unref(buf);
	return (int64_t)reg_val; 
}
int64_t qemu_get_vtype(thread_state_t * state){
  qemu_plugin_reg_descriptor *desc = &g_array_index((GArray*)(state->regs), qemu_plugin_reg_descriptor, idx_vtype);
	GByteArray *buf = g_byte_array_new();
	qemu_plugin_read_register(desc->handle, buf);
	uint64_t reg_val = *((uint64_t*)buf->data);
  g_byte_array_unref(buf);
	return (int64_t)reg_val; 
}
int64_t qemu_get_xreg(thread_state_t * state, int reg){
  qemu_plugin_reg_descriptor *desc = &g_array_index((GArray*)(state->regs), qemu_plugin_reg_descriptor, idx_xregs+reg);
	GByteArray *buf = g_byte_array_new();
	qemu_plugin_read_register(desc->handle, buf);
	uint64_t reg_val = *((uint64_t*)buf->data);
  g_byte_array_unref(buf);
	return (int64_t)reg_val; 
}
#endif

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

void rave_read_string(unsigned int cpu_index, uint32_t insn_opcode, char * string, uint64_t maxlen){
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	uint64_t string_addr = qemu_get_xreg(&cpus_state[cpu_index],src1);
	uint64_t len = qemu_get_xreg(&cpus_state[cpu_index],src2);

	//Read string from guest memory
	#ifndef RVV_07
	GByteArray *mem_buf = g_byte_array_new();
	#endif
	uint64_t i;
	for(i=0; i<maxlen && i<len; ++i){
		#ifdef RVV_07
		cpu_memory_rw_debug(qemu_get_cpu(cpu_index), string_addr + i, (uint8_t*)&string[i], 1, 0);
		#else
    qemu_plugin_read_memory_vaddr(string_addr + i, mem_buf, 1); 
		string[i] = (uint8_t)mem_buf->data[0];
		#endif
		if (string[i] == '\n') string[i] = '\0';
		if (string[i] == '\0') break;
	}
	string[i] = '\0';
	#ifndef RVV_07
	g_byte_array_unref(mem_buf);
	#endif
}

