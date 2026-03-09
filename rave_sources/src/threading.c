/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "threading.h"
#include "state.h"
#include <stdlib.h>

int mpi_rank;
volatile int N_THREADS; //This increases when a new CPU is online
int alloc_threads;
int expected_threads;
parallel_region_t parallel_region;
volatile int write_lock = 0;
int mpi_size;

void newthread_cb(void){
	if (alloc_threads < N_THREADS+1){
		printf("RAVE tried to to generate more threads (%d) than allocated (%d)\n", N_THREADS+1, alloc_threads);
		printf("To allocate more threads, set the environment variable \"RAVE_MAX_THREADS\" to the desired value\n");
		printf("\t - RAVE will allocate the maximum between OMP_NUM_THREADS and RAVE_MAX_THREADS\n");
		exit(-1);
		//alloc_threads *= 2;
		//cpus_state = (thread_state_t*)realloc(cpus_state, sizeof(thread_state_t)*alloc_threads);
	}
	reset_thread(&cpus_state[N_THREADS]);
	++N_THREADS;
}
