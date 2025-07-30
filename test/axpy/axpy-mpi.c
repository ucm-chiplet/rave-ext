#include <mpi.h>
#ifdef _OPENMP
#include "omp.h"
#endif
#include "rave_user_events.h"
#include "stdlib.h"
#include <stdio.h>

#define reps 1

__attribute__((noinline))
void validate(double * Y, double * X, double alpha, int N){
	double err=0;

	for(int r=0; r<reps; ++r){
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		double diff = (Y[i] - X[i]*alpha);
		err += diff*diff;
	}
	}
	if (err > 1e-3) printf("Error!\n");
}

int main(){

  // Initialize the MPI environment
  MPI_Init(NULL, NULL);
  // Get the number of processes
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
  // Get the rank of the process
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	MPI_Status status;

	long gvl;
	rave_restart_trace();

	int N = 1<<20; 
	if ((N%world_size) != 0) N+=(world_size - (N%world_size)); //Make N a multiple of mpi processes
	int N_local = N/world_size;

	double * Y = (double *)malloc(sizeof(double)*N);
	double * X = (double *)malloc(sizeof(double)*N);
	double alpha = 42;

	//Rank 0 initializes Y
	if (world_rank == 0){
		rave_begin_region("ini-simd");
		for(int r=0; r<reps; ++r){
		#pragma clang loop vectorize(enable)
		for(int i=0; i<N; ++i){
			Y[i]=0;
		}
		}
		rave_end_region("ini-simd");
	}

	//Rank 1 initializes X
	if (world_size == 1 || world_rank == 1){
		rave_begin_region("ini-omp");
		for(int r=0; r<reps; ++r){
		#pragma omp parallel for
		for(int i=0; i<N; ++i){
			X[i]=(double)(i%32);
		}
		}
		rave_end_region("ini-omp");
	}

	rave_begin_region("Scatter");

#if 0
	//Scatter Y to everybody
	MPI_Scatter(Y, N_local, MPI_DOUBLE, Y, N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	//Send X slice to everybody, and full to A
	MPI_Scatter(X, N_local, MPI_DOUBLE, X, N_local, MPI_DOUBLE, world_size>1 ? 1 : 0, MPI_COMM_WORLD);
	if (world_size > 1){
#if 0
		MPI_Sendrecv(&X[N_local], N-N_local, MPI_DOUBLE, 0, 0, &X[N_local], N-N_local, MPI_DOUBLE, 1, 0,	MPI_COMM_WORLD, &status);
#else
	
		if (world_rank == 1){
			MPI_Send(&X[N_local],N-N_local,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
		}else if (world_rank==0){
			MPI_Recv(&X[N_local],N-N_local,MPI_DOUBLE,1,0,MPI_COMM_WORLD, &status);
		}
#endif
	}
#else
	int N_nop = N/80;
	for(int i=0; i<N_nop; ++i) asm volatile("nop\n");
	if (world_rank==0){
		for(int i=0; i<N_nop/2; ++i) asm volatile("nop\n");
		for(int i=0; i<3*N_nop/4; ++i) asm volatile("nop\n");
	}else if (world_rank==1){
		for(int i=0; i<3*N_nop/4; ++i) asm volatile("nop\n");
	}
#endif

	rave_end_region("Scatter");


	#ifdef _OPENMP
	int n_threads = omp_get_num_threads();
	if ((world_rank % 2) == 1) omp_set_num_threads(n_threads/2);
	#endif

	#pragma omp parallel
	{
	rave_begin_region("axpy");
	for(int r=0; r<reps; ++r){
	#pragma omp for
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N_local; ++i){
		Y[i] += X[i] * alpha;
	}
	}
	rave_end_region("axpy");
	}

	//Rank 0 receives the Y buffer
	rave_begin_region("gather");
#if 0
	MPI_Gather(Y, N_local, MPI_DOUBLE, Y, N_local, MPI_DOUBLE, 0, MPI_COMM_WORLD);
#else
	int N_nop2 = N/100;
	for(int i=0; i<N_nop2; ++i) asm volatile("nop\n");
	if (world_rank==0){
		for(int i=0; i<N_nop2/2; ++i) asm volatile("nop\n");
	}
#endif
	rave_end_region("gather");


	//Rank 0 validates the result
	if (world_rank == 0){
	#pragma omp parallel
	{
	#pragma omp single
	{
		int batch = N/8;
		for(int i=0; i<=N-batch; i+=batch){
			if (i+batch > N) batch = N-i;
			#pragma omp task
			{
			rave_begin_region("validate");
			validate(&Y[i], &X[i], alpha*reps, batch);
			rave_end_region("validate");
			}
		}
	}
	}
	}
	
	volatile double y = Y[0];
	free(X);
	free(Y);

  MPI_Finalize();
	//printf("Rank %d completed!\n", world_rank);
	return 0;

}
