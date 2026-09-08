/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>
#include "counters.h"
#include <sched.h>

#define set_lock(lock) if(N_THREADS>1){ while (! __sync_bool_compare_and_swap(&lock, 0, 1)){sched_yield();}}//Wait until lock is 0, then put it to 1
#define release_lock(lock) if (N_THREADS>1) { __sync_val_compare_and_swap(&lock, 1, 0);} //Unlock 
#define file_lock(fd, action) flock(fd,action); 

struct parallel_region_t{
	volatile int n_threads; //Counts threads in region
	volatile int in_barrier; //Counts threads waiting in barrier
	volatile int crossed_barrier; //Counts threads that passed the last barrier
	volatile uint64_t barrier_time; //Max. timestamp from all threads at the barrier (to sync)

	volatile int lock;
	int master_thread;
	int first_barrier;
	rave_counters parallel_region_counter;
	rave_counters last_barrier_counters;
};
typedef struct parallel_region_t parallel_region_t;

//Exported variables
extern int mpi_rank;
extern int mpi_size;
extern volatile int N_THREADS;
extern int expected_threads;
extern int alloc_threads;
extern parallel_region_t parallel_region;
extern volatile int write_lock;

void newthread_cb(long unsigned int id, unsigned int vcpu_index);
void parallel_end(int cpu_index);
void parallel_begin(int cpu_index, int parallelism);
void parallel_barrier(int cpu_index);
