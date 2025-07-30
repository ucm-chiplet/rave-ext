all: axpy axpy-omp axpy-mpi axpy-hybrid 

LLVM_DIR?=../../build/llvm-cross/llvm-EPI-development-toolchain-cross
RAVE_INCLUDE?=../../interfaces

CC=${LLVM_DIR}/bin/clang

VFLAGS=-O3 -ffast-math -mepi -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy -mllvm -disable-loop-idiom-memset

axpy: axpy.c
	$(CC) $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x

axpy-omp: axpy.c
	$(CC) $(VFLAGS) -fno-vectorize -fopenmp -I${RAVE_INCLUDE} $^ -o $@.x


axpy-mpi: axpy-mpi.c
	OMPI_CC=$(CC) mpicc $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x 

axpy-hybrid: axpy-mpi.c
	OMPI_CC=$(CC) mpicc $(VFLAGS) -fno-vectorize -fopenmp -I${RAVE_INCLUDE} $^ -o $@.x 

clean:
	rm -f *.x
