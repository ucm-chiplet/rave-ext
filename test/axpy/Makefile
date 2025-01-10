all: axpy axpy-omp axpy-mpi axpy-hybrid 

VFLAGS=-O3 -ffast-math -mepi -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy -mllvm -disable-loop-idiom-memset

axpy: axpy.c
	clang $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x

axpy-omp: axpy.c
	clang $(VFLAGS) -fno-vectorize -fopenmp -I${RAVE_INCLUDE} $^ -o $@.x


axpy-mpi: axpy-mpi.c
	mpicc $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x 

axpy-hybrid: axpy-mpi.c
	mpicc $(VFLAGS) -fno-vectorize -fopenmp -I${RAVE_INCLUDE} $^ -o $@.x 

clean:
	rm -f *.x
