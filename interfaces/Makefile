all: f_module #py_module

LLVM_1_0=../build/llvm-cross/llvm-EPI-development-toolchain-cross

#py_module:
#	CC=$(LLVM_1_0)/bin/clang python py_module.py build --build-lib .

f_module:
	$(LLVM_1_0)/bin/clang rave_fortran_c_func.c -c -o rave_fortran_c_func.o
	$(LLVM_1_0)/bin/flang rave_user_events.f90  -c -o rave_user_events_tmp.o
	$(LLVM_1_0)/riscv64-unknown-linux-gnu/bin/ld -r rave_fortran_c_func.o rave_user_events_tmp.o -o rave_user_events_f.o

clean:
	rm -f *.so *.o *.mod
