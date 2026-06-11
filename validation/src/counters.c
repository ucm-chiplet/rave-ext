#include <stdio.h>
#include "rave_user_events.h"
int main(){
	long ptr = 0xBEEFCAFE;
	char i8;
	short i16;
	int i32;
	long i64;
	float f32;
	double f64;

	int instr=19;
	int flops=6;
	int bytes=1*3 + 2*3 + 4*4 + 8*4;
	char region_name[64];
	sprintf(region_name, "%d_%d_%d", instr,flops,bytes);
	//instr_flops_bytes
	rave_begin_region(region_name);
	asm volatile("lb %0 , 0(%1)\n":"+r"(i8):"r"(&ptr));
	asm volatile("lbu %0 , 0(%1)\n":"+r"(i8):"r"(&ptr));
	asm volatile("lh %0 , 0(%1)\n":"+r"(i16):"r"(&ptr));
	asm volatile("lhu %0 , 0(%1)\n":"+r"(i16):"r"(&ptr));
	asm volatile("lw %0 , 0(%1)\n":"+r"(i32):"r"(&ptr));
	asm volatile("ld %0 , 0(%1)\n":"+r"(i64):"r"(&ptr));
	asm volatile("flw %0 , 0(%1)\n":"+f"(f32):"r"(&ptr));
	asm volatile("fld %0 , 0(%1)\n":"+f"(f64):"r"(&ptr));

	asm volatile("sb %0 , 0(%1)\n"::"r"(i8),"r"(&ptr));
	asm volatile("sw %0 , 0(%1)\n"::"r"(i16),"r"(&ptr));
	asm volatile("sh %0 , 0(%1)\n"::"r"(i32),"r"(&ptr));
	asm volatile("sd %0 , 0(%1)\n"::"r"(i64),"r"(&ptr));
	asm volatile("fsw %0 , 0(%1)\n"::"f"(f32),"r"(&ptr));
	asm volatile("fsd %0 , 0(%1)\n"::"f"(f64),"r"(&ptr));

	asm volatile("fmadd.d %0, %0, %0, %0\n":"+f"(f64));
	asm volatile("fadd.d %0, %0, %0\n":"+f"(f64));
	asm volatile("fmadd.s %0, %0, %0, %0\n":"+f"(f32));
	asm volatile("fadd.s %0, %0, %0\n":"+f"(f32));
	asm volatile("add %0, %0, %0\n":"+r"(i32));
	rave_end_region(region_name);
}
