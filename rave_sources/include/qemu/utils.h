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
#elif ! defined (RVV_10)
#define RVV_10
#endif

#include <string.h>
#include <stdint.h>
#include "state.h"

#define my_strcpy(dst, src)\
{\
	int len = strlen(src);\
	dst = malloc(len+1);\
	strcpy(dst,src);\
}


#ifdef RVV_07
//Defined in QEMU
extern int cpu_memory_rw_debug(uint8_t *cpu, uint64_t addr, uint8_t *buf, int len, int is_write);
void *qemu_get_cpu(int index);
#endif

//Defined by us
void setup_regs(unsigned int cpu_index);
int64_t get_vl(thread_state_t * state);
int64_t get_vtype(thread_state_t * state);
int64_t get_xreg(thread_state_t * state, int reg);
char * get_vreg(thread_state_t * state, int reg, int vlB);

void plugin_outs(char * str);

char contains_string(char * str, const char * find);
void rave_read_string(unsigned int cpu_index, uint64_t string_addr, uint64_t len, char * string, uint64_t maxlen);
