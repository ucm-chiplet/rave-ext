/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>

struct PC_node{
	uint64_t PC;
	long freq;
	double weight;
	struct PC_node * next;
	struct PC_node * prev;
};
typedef struct PC_node PC_node;


/*
#define max_sample_freq 512
#define log_sample 9
#define sample_variability 67
int sample_countdown = max_sample_freq;
*/

uint64_t find_binary_base(void) ;
void update_PC(uint64_t PC, uint64_t weight);

/*
void sample(uint64_t PC);
*/

#include "elfutils/libdwfl.h"

int get_first_module_base(Dwfl_Module *mod, void **userdata, const char *name, Dwarf_Addr _base, void *arg);
void init_dwfl(const char *binary_path) ;
int resolve_pc_to_source(Dwarf_Addr pc, const char ** symbol, const char **filename, int *line, int *column) ;
void print_loop_profile(FILE * fd);
