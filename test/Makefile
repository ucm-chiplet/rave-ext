all: example

VFLAGS=-O3 -ffast-math -mepi -fno-vectorize -mllvm -combiner-store-merging=0 -Rpass=loop-vectorize -Rpass-analysis=loop-vectorize -mcpu=avispado -mllvm -vectorizer-use-vp-strided-load-store -mllvm -enable-mem-access-versioning=0 -mllvm -disable-loop-idiom-memcpy

example:
	clang $(VFLAGS) -I../include/ example.c -o example.x

example-f:
	clang rave_fortran.c -c -o rave_fortran.o
	flang rave_module.f90 -c -o rave_module.o 
	flang $(VFLAGS) -fopenmp example.f90 -c -o example-f.o 
	flang example-f.o rave_fortran.o rave_module.o -o example-f.x 
