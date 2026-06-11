TARGETS = example-c example-cpp 


ifndef RVV
  $(error Need to define RVV variable to 0_7_1 or 1_0)
endif

ifeq ($(RVV),0_7_1)
  LLVM_DIR?=../../build/llvm-cross/llvm-EPI-0.7-development-toolchain-cross
else
  LLVM_DIR?=../../build/llvm-cross/llvm-EPI-development-toolchain-cross
  TARGETS+=example-f
endif
RAVE_INCLUDE?=../../build/interfaces

CC=${LLVM_DIR}/bin/clang
CPP=${LLVM_DIR}/bin/clang++
FC=${LLVM_DIR}/bin/flang

DEBUG=-g 
VFLAGS=-O3 -ffast-math -mepi -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy -mllvm -disable-loop-idiom-memset

all: $(TARGETS)

example-c: example.c
	$(CC) $(VFLAGS) $(DEBUG) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@-$(RVV).x

example-cpp: example.cpp
	$(CPP) $(VFLAGS) -fno-vectorize  -I${RAVE_INCLUDE} $^ -o $@-$(RVV).x

example-f: example.f90
	$(FC) $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} ${RAVE_INCLUDE}/rave_user_events_f.o -fopenmp-simd $^ -o $@-$(RVV).x

clean:
	rm *.x 
