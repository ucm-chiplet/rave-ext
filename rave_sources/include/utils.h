/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once


#ifdef RVV_07
#undef RVV_10
#else
#define RVV_10
#endif

#include <string.h>
#include <stdint.h>

#define my_strcpy(dst, src)\
{\
	int len = strlen(src);\
	dst = malloc(len+1);\
	strcpy(dst,src);\
}

//include/hw/core/cpu.h (0.7 :307 (def) ||||| 1.0 : 323 (def CPUState) //Util for knowing OFFSET REGS
#define sizeof_ulong sizeof(uint64_t)

//qemu_get_cpu returns an ArchCPU, which has a CPURISCVState (typdef of CPUArchState), 
//ArchCPU is defined in target/riscv/cpu.h (l:277 for 0.7, l:444 for 1.0)
//CPUArchState is defined in target/riscv/cpu.h (l:114 for 0.7, l:161 for 1.0)
//In accel/tcg/plugin-gen.c (l:175 for 0.7, l:165 for 1.0) is a good place to put : printf("Offset is %ld\n",offsetof(ArchCPU, env));
#ifdef RVV_07
#define OFFSET_CPUState (33552) //For 0.7
#define OFFSET_REGS (sizeof_ulong*32 + sizeof(uint64_t)*32 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#else
//#define OFFSET_CPUState (832) //For 1.0
#define OFFSET_CPUState (10176) //For 1.0
#define OFFSET_REGS (sizeof_ulong*32*2 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#endif

#define RV_VLEN_MAX (256*64)

#ifdef RVV_07
//Defined in QEMU
extern int cpu_memory_rw_debug(uint8_t *cpu, uint64_t addr, uint8_t *buf, int len, int is_write);
void *qemu_get_cpu(int index);
#endif

//Defined by us
int64_t qemu_get_vl(unsigned int cpu_index);
int64_t qemu_get_vtype(unsigned int cpu_index);
int64_t qemu_get_xreg(unsigned int cpu_index, int reg);

char contains_string(char * str, const char * find);
void rave_read_string(unsigned int cpu_index, uint32_t insn_opcode, char * string, uint64_t maxlen);
