all: ompt.so 

lib:
	mkdir -p lib

ompt.so: ompt/ompt.c lib
	clang -O3 $< -shared -fPIC -o lib/$@

#libmpi.so

#libomp.so:
