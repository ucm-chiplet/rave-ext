/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "threading.h"
#include "state.h"
#include "counters_generic.h"
#include "utils.h"
#include <stdlib.h>

int mpi_rank;
volatile int N_THREADS; //This increases when a new CPU is online
int alloc_threads;
int expected_threads;
parallel_region_t parallel_region;
volatile int write_lock = 0;
int mpi_size;

void newthread_cb(long unsigned int id, unsigned int vcpu_index){
	(void)id;         /* Silence unused parameter error */
  (void)vcpu_index; /* Silence unused parameter error */
	if (alloc_threads < N_THREADS+1){
		#if 1
		printf("RAVE tried to to generate more threads (%d) than allocated (%d)\n", N_THREADS+1, alloc_threads);
		printf("To allocate more threads, set the environment variable \"RAVE_MAX_THREADS\" to the desired value\n");
		printf("\t - RAVE will allocate the maximum between OMP_NUM_THREADS and RAVE_MAX_THREADS\n");
		exit(-1);
		#else
		//printf("Need more threads\n");
		alloc_threads *= 2;
		cpus_state = (thread_state_t*)realloc(cpus_state, sizeof(thread_state_t)*alloc_threads);
		//printf("Added more threads\n");
		#endif
	}
	reset_thread(N_THREADS); 
	++N_THREADS;
}

void parallel_end(int cpu_index){

	//Wait for everyone to cross the last barrier
	while (__sync_val_compare_and_swap(&parallel_region.crossed_barrier, 0, 0) != 0) {;} 

	//Substract counters and add to master's
	update_counters(&parallel_region.parallel_region_counter, &parallel_region.last_barrier_counters); //region_c = last_b - region_c
	add_counters(&cpus_state[cpu_index].accum_counters, &parallel_region.parallel_region_counter);// master_thread += region_c
	parallel_region.master_thread = -1;
}

void parallel_begin(int cpu_index, int parallelism){
	//Atomicity assumed (only on thread active when this happens -> No nested parallel regions
	//TODO: Check this assumption, act accordingly
	parallel_region.master_thread = cpu_index;

	//Build barrier
	parallel_region.n_threads = parallelism;

	//Allocate more threads if needed (It shouldn't cause a race condition here)
	if (parallelism > alloc_threads){
		alloc_threads = parallelism;
		cpus_state = (thread_state_t*)realloc(cpus_state, sizeof(thread_state_t)*alloc_threads);
	}

	parallel_region.in_barrier = 0;
	parallel_region.crossed_barrier = 0; 
	parallel_region.barrier_time = 0;
	parallel_region.first_barrier = 1;
}

#include <sched.h>
void parallel_barrier(int cpu_index){

	//Wait if the previous barrier has not been crossed by other threads
	while (__sync_val_compare_and_swap(&parallel_region.crossed_barrier, 0, 0) != 0) {sched_yield();} 

	//Counters
	if (cpu_index == parallel_region.master_thread){
		//Master sets to 0 the last_barrier_counters (the other threads will accumulate when they exit the barrier)
		reset_counters(&parallel_region.last_barrier_counters);
	}

	//Set max barrier time
	while (1) {
		uint64_t old_tmax = parallel_region.barrier_time; // Read the current tmax
		if (cpus_state[cpu_index].timestamp<= old_tmax) break; // No need to update if the thread's t is not greater than tmax

		// Atomically update tmax if it has not changed
		if (__sync_val_compare_and_swap(&parallel_region.barrier_time, old_tmax, cpus_state[cpu_index].timestamp) == old_tmax) break; // Successful update, exit loop
	}

	//Add counters to parallel region
	if (cpu_index != parallel_region.master_thread){
		//Critical region! Locking
		set_lock(parallel_region.lock);
		add_counters(&parallel_region.last_barrier_counters, &cpus_state[cpu_index].accum_counters);
		release_lock(parallel_region.lock);
	}

	//Increase the barrier (+1)
	__sync_fetch_and_add(&parallel_region.in_barrier, 1);

	//Wait until the barrier is equal to n_threads
	while (__sync_val_compare_and_swap(&parallel_region.in_barrier, parallel_region.n_threads, parallel_region.n_threads) != parallel_region.n_threads) {sched_yield();} 


	//Cross the barrier (+1). If everyone crossed it, enter if: 
	if (__sync_add_and_fetch(&parallel_region.crossed_barrier, 1) == parallel_region.n_threads){
		//If first barrier, set its counters
		if (parallel_region.first_barrier){
			parallel_region.first_barrier = 0;
			copy_counters(&parallel_region.parallel_region_counter, &parallel_region.last_barrier_counters);
		}

		//**Afterwards** Unlock barrier, so next can start
		parallel_region.crossed_barrier = 0;
		parallel_region.in_barrier = 0;
	}
	cpus_state[cpu_index].need_align = 2;
}
