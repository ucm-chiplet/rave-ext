all: example-c example-cpp example-f

VFLAGS=-O3 -ffast-math -mepi -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy -mllvm -disable-loop-idiom-memset

example-c: example.c
	clang $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} $^ -o $@.x

example-cpp: example.cpp
	clang++ $(VFLAGS) -fno-vectorize  -I${RAVE_INCLUDE} $^ -o $@.x

example-f: example.f90
	flang $(VFLAGS) -fno-vectorize -I${RAVE_INCLUDE} ${RAVE_INCLUDE}/rave_user_events_f.o -fopenmp $^ -o $@.x

clean:
	rm *.x 
