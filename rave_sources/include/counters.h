/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdio.h>

#define PERCENTAGE(fd,x,y,fin)\
	if (x>0) {\
		P_PERCENTAGE(fd, " (%.2f %%)", ((y)==0?0:(100.0*(x))/(y)));\
	}\
	fprintf(fd,"%c",fin);

#define SEWS 4
struct rave_counters{
				double scalar_instr;
				double vsetvl_instr;
				double vector_instr[SEWS];
				double velem[SEWS];

				//Memory
				double velem_mem[SEWS];
				double vunit_instr[SEWS];
				double vstride_instr[SEWS];
				double agg_strides[SEWS];
				double vidx_instr[SEWS];
				double vspill_instr[SEWS];

				//Arith
				double velem_arith[SEWS];
				double vfp_instr[SEWS];
				double vint_instr[SEWS];

				//Reductions
				double velem_reductions[SEWS];
				double vfp_reductions[SEWS];
				double vint_reductions[SEWS];

				//Masks
				double velem_mask[SEWS];
				double vmask_instr[SEWS];

				//General
				double moved_bytes_s;
				double moved_bytes_v;
				double scalarflops;
				double vectorflops;
};
typedef struct rave_counters rave_counters;

void reset_counters(rave_counters * c);
//c1 = c2
void copy_counters(rave_counters * c1, rave_counters * c2);
//c1 += c2;
void add_counters(rave_counters * c1, rave_counters * c2);
#if 0
//c1 = moving_avg(c1,c2)
void avg_counters(rave_counters * c1, rave_counters * c2, int n);
#endif
// c1 = c2*mult
void mul_counters(rave_counters * c1, rave_counters * c2, double mult);
//c1 = c2-c1
void update_counters(rave_counters * c1, rave_counters * c2);

void print_counters_human(FILE * fd, rave_counters * counters);
void print_csv_header(FILE * fd);
void print_counters_csv(FILE * fd, rave_counters * counters);
