all: clean f_module py_module

f_module:
	clang rave_fortran_c_func.c -c -o rave_fortran_c_func.o
	flang rave_user_events.f90  -c -o rave_user_events_tmp.o
	clang -r rave_fortran_c_func.o rave_user_events_tmp.o -o rave_user_events_f.o

py_module: py_module.py
	srun -p arriesgado-jammy -t 00:01:00 bash -c 'python $^ build --build-temp=../python_tmp --build-lib .'


clean:
	rm -f *.so *.o *.mod
