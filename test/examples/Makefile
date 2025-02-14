all: example-c example-cpp example-f

CC=${LLVM_DIR}/bin/clang
CPP=${LLVM_DIR}/bin/clang++
FC=${LLVM_DIR}/bin/flang

VFLAGS=-O3 -ffast-math -mepi -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy -mllvm -disable-loop-idiom-memset

example-c: example.c
	$(CC) $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x

example-cpp: example.cpp
	$(CPP) $(VFLAGS) -fno-vectorize  -I${RAVE_INCLUDE} $^ -o $@.x

example-f: example.f90
	$(FC) $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} ${RAVE_INCLUDE}/rave_user_events_f.o -fopenmp $^ -o $@.x

clean:
	rm *.x 
