int mpi_rank = 0;
int mpi_size = 1;
volatile int N_THREADS = 0; //This increases when a new CPU is online
int expected_threads = 0;
int alloc_threads = 0;

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
parallel_region_t parallel_region;

static void newthread_cb(void){
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
