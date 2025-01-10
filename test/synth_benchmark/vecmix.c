#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define nop_scalar "nop \n"
#define nop_vector "vor.vv v0,v0,v0 \n"

#if __riscv_vector_version==800
#define vsetvl(vl) asm volatile("vsetvli %0, x0, e64, m1, ta, ma\n":"=r"(vl));
#else
#define vsetvl(vl) asm volatile("vsetvli %0, x0, e64, m1\n":"=r"(vl));
#endif

#define nops_10(instr) \
		instr\
		instr\
		instr\
		instr\
		instr\
		instr\
		instr\
		instr\
		instr\
		instr

#define nops_100(instr) asm volatile(\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		nops_10(instr)\
		);



int main(int argc, char * argv[]){

	double f = 1e4; //In %Million
	int N = 1e6;

	if (argc>=2) f = atoi(argv[1]); 

	if (argc>=3) N = atoi(argv[2]);

	f/=1.0e6;

	uint64_t N_vector = (uint64_t)((f*N));
	uint64_t N_scalar = N - N_vector;
	printf("%lu vector, %lu scalar, (%.2f%%m)\n",N_vector,N_scalar,((double)N_vector/(double)(N_vector+N_scalar))*1e6);

	long vl;
	vsetvl(vl);
	for(uint64_t i=0; i<N_vector; i+=100){
		nops_100(nop_vector);
	}
	for(uint64_t i=0; i<N_scalar; i+=100){
		nops_100(nop_scalar);
	}
}
