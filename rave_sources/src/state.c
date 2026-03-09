/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "state.h"
#include <stdio.h>

uint64_t timestamp;
//QEMU_PLUGIN_EXPORT int qemu_plugin_version;

char TRACE_ENABLED = 1; //Enabled by default 
char ACCUM_REGIONS = 0;
FILE * FD_PRV;
FILE * FD_REPORT;
int MUSA;
int N_PIPELINES;
char PRINT_LOGFILE = 0;
char PRINT_PROFILE = 0;
char PRINT_PRV = 0;
int RAVE_ELEN = 64;
int RAVE_VLMAX = 0;
char REGIONS_ENABLED = 1; //Enabled by default 
int REGION_EVENT = 1000;
char STREAM_REPORT = 0;
char TRACE_ADDR = 0;
char TRACE_SCALAR = 0;
uint64_t base;
int disabled_once = 0;
uint64_t timestamp;
int PLAIN_TEXT=0;

char PRINT_REPORT = 0;
char PRINT_CSV = 0;
char * filename = NULL; 
char * BINARY_NAME = NULL;
FILE * FD_PCF;
FILE * FD_ROW;
FILE * FD_CSV;
FILE * FD_COMM;
FILE * FD_PROFILE;

thread_state_t * cpus_state;

#ifndef RVV_07
int idx_xregs;
int idx_vl;
int idx_vtype;
#endif

void reset_thread(thread_state_t * state){
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

	#ifndef RVV_07
	//Registers:
	idx_xregs=idx_vl=idx_vtype=-1;

	state -> regs = qemu_plugin_get_registers();
	for (int i = 0; i < state->regs->len; i++) {
		qemu_plugin_reg_descriptor *desc = &g_array_index(state->regs, qemu_plugin_reg_descriptor, i);
		//printf("%d %s\n",i,desc->name);
		if (idx_xregs < 0 && (g_strcmp0(desc->name, "zero")==0)) idx_xregs=i;
		if (idx_vtype < 0 && (g_strcmp0(desc->name, "vtype")==0)) idx_vtype=i;
		if (idx_vl < 0 && (g_strcmp0(desc->name, "vl")==0)) idx_vl=i;
	}
	#endif
}

