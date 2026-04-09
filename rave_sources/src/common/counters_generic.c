/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "counters_generic.h"

void reset_counters(rave_counters * c){
	double * ptr = (double *)c;
	for(int i=0; i<sizeof(rave_counters)/sizeof(double); ++i){
		ptr[i]=0.0;
	}
}

//c1 = c2
void copy_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c]; 
	}
}

//c1 += c2;
void add_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] += c2_ptr[c]; 
	}
}

#if 0
//c1 = moving_avg(c1,c2)
void avg_counters(rave_counters * c1, rave_counters * c2, int n){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] += (c2_ptr[c]-c1_ptr[c])/n;
	}
}
#endif
// c1 = c2*mult
void mul_counters(rave_counters * c1, rave_counters * c2, double mult){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c] * mult;
	}
}

//c1 = c2-c1
void update_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c] - c1_ptr[c]; 
	}
}
