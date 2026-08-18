/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>


struct loop_node{
	uint64_t PC;
	long freq;
	double weight;
	double tot_instr;
	double tot_vinstr;
	double tot_its;
	double register_usage;
	struct loop_node * next;
	struct loop_node * prev;
};
typedef struct loop_node loop_node;

#define NHASHES 4096
struct PC_hash_map_node{
	int occupancy;
	uint64_t * PC;
	loop_node ** position;
};
typedef struct PC_hash_map_node PC_hash_map_node; 

#define NUM_VREGS 32
struct profile_t{
	loop_node * first_loop_node;
	loop_node * last_loop_node;
	uint64_t curr_loop_PC;
	uint64_t jump_PC;
	uint64_t loop_its;
	uint64_t loop_instr;
	uint64_t loop_vinstr;
	uint64_t loop_weight;
	char used_vreg[NUM_VREGS];
	PC_hash_map_node PC_hash_map[NHASHES];
};
typedef struct profile_t profile_t;

void reset_profile(profile_t * loop_profile);

#define N_PCS_NODE 4096
struct calltrace_node_t{
	int fill;
	uint64_t PCs[N_PCS_NODE];
	struct calltrace_node_t * next;
};
typedef struct calltrace_node_t calltrace_node_t;

struct calltrace_t{
	int next_is_func;
	int n_nodes;
	calltrace_node_t * first_node;
	calltrace_node_t * last_node;
};
typedef struct calltrace_t calltrace_t;

void reset_calltrace(calltrace_t * ct);

void add_to_calltrace(calltrace_t * ct, uint64_t PC);


struct callstack_t{
	uint64_t PC;
	struct callstack_t * prev;
};
typedef struct callstack_t callstack_t;

/*
#define max_sample_freq 512
#define log_sample 9
#define sample_variability 67
int sample_countdown = max_sample_freq;
*/

uint64_t find_binary_base(void) ;
void update_PC(profile_t * loop_profile);

/*
void sample(uint64_t PC);
*/

#include "elfutils/libdwfl.h"

int get_first_module_base(Dwfl_Module *mod, void **userdata, const char *name, Dwarf_Addr _base, void *arg);
void init_dwfl(const char *binary_path) ;
int resolve_pc_to_source(Dwarf_Addr pc, const char ** symbol, const char **filename, int *line, int *column) ;
void print_loop_profile(FILE * fd, profile_t * loop_profile);
void print_call_trace(FILE * fd, calltrace_t * ct);
