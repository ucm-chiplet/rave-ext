#include <mpi.h>
#ifdef _OPENMP
#include "omp.h"
#endif
#include "rave_user_events.h"
#include "stdlib.h"

__attribute__((noinline))
void validate(double * Y, double * X, double alpha, int N){
	double err=0;
	#pragma clang loop vectorize(enable)
	for(int i=0; i<N; ++i){
		double diff = (Y[i] - X[i]*alpha);
		err += diff*diff;
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
	rave_name_event(1000,"code_region");
	rave_name_value(1000,0,"End");
	rave_name_value(1000,1,"ini-simd");
	rave_name_value(1000,2,"ini-omp");
	rave_name_value(1000,3,"axpy");
	rave_name_value(1000,4,"validate");
	rave_name_value(1000,5,"Scatter");
	rave_name_value(1000,6,"Gather");
	rave_restart_trace();

	int N = 1<<20;
	double * Y = (double *)malloc(sizeof(double)*N);
	double * X = (double *)malloc(sizeof(double)*N);
	double alpha = 42;

	if (world_rank == 0){
		rave_event_and_value(1000,1)
		#pragma clang loop vectorize(enable)
		for(int i=0; i<N; ++i){
			Y[i]=0;
		}
		rave_event_and_value(1000,0)
	}
	if (world_size == 1 || world_rank == 1){
		rave_event_and_value(1000,2)
		#pragma omp parallel for
		for(int i=0; i<N; ++i){
			X[i]=(double)(i%32);
		}
		rave_event_and_value(1000,0)
	}

	rave_event_and_value(1000,5)
	int slice = N/world_size;
	if (world_rank == world_size-1) slice+=N%world_size;

	if (world_rank == 0){
		for(int i=1; i<world_size; ++i){
			int start= i*slice;
			int i_slice = (i==world_size-1)?slice+N%world_size:slice;
			MPI_Send(&X[start],i_slice,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
			MPI_Send(&Y[start],i_slice,MPI_DOUBLE,i,0,MPI_COMM_WORLD);
		}
	}else{
		MPI_Recv(X,slice,MPI_DOUBLE,0,0,MPI_COMM_WORLD, &status);
		MPI_Recv(Y,slice,MPI_DOUBLE,0,0,MPI_COMM_WORLD, &status);
	}
	rave_event_and_value(1000,0)

	#ifdef _OPENMP
	int n_threads = omp_get_num_threads();
	if ((world_rank % 2) == 0) omp_set_num_threads(n_threads/2);
	#endif


	rave_event_and_value(1000,3)
	#pragma omp parallel for
	#pragma clang loop vectorize(enable)
	for(int i=0; i<slice; ++i){
		Y[i] += X[i] * alpha;
	}
	rave_event_and_value(1000,0)

	rave_event_and_value(1000,6)
	if (world_rank == 0){
		for(int i=1; i<world_size; ++i){
			int start=i*slice;
			int i_slice = (i==world_size-1)?slice+N%world_size:slice;
			MPI_Recv(&Y[start],i_slice,MPI_DOUBLE,i,0,MPI_COMM_WORLD, &status);
		}
	}else{
		MPI_Send(Y,slice,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
	}
	rave_event_and_value(1000,0)


	#pragma omp parallel
	{
	#pragma omp single
	{
	if (world_rank == 0){
		int batch = N/8;
		for(int i=0; i<N; i+=batch){
			#pragma omp task
			{
			rave_event_and_value(1000,4)
			validate(&Y[i], &X[i], alpha, batch);
			rave_event_and_value(1000,0)
			}
		}
	}
	}
	}
	
	volatile double y = Y[0];
	free(X);
	free(Y);

  MPI_Finalize();
	printf("Completed!\n");
	return 0;

}
