int main(){
	asm volatile(
			".rept 10 \n"
			"nop \n"
			".endr \n"
	);

	long N;
	#pragma clang loop unroll(disable)
	for(int i=0; i<4; ++i){
		asm volatile(
				".rept 10 \n"
				"nop \n"
				".endr \n"
		);
		asm volatile( 
				"li %[count], 100\n"
				"1: \n"
				"addi %[count], %[count], -1 \n"
				"bne %[count], x0, 1b \n"
				:[count]"+r"(N)
		);
	}

#if 1
	#pragma clang loop unroll(disable)
	for(int i=0; i<3; ++i){
		asm volatile(
				".rept 50 \n"
				"nop \n"
				".endr \n"
		);

		asm volatile( //Second loop
				"li %[count], 300\n"
				"1: \n"
				"nop\n"
				"nop\n"
				"nop\n"
				"nop\n"
				"addi %[count], %[count], -1 \n"
				"bne %[count], x0, 1b \n"
				:[count]"+r"(N)
		);
	}

	asm volatile(
			".rept 80 \n"
			"nop \n"
			".endr \n"
	);

	long gvl;
	#pragma clang loop unroll(disable)
	for(int i=0; i<2; ++i){
		asm volatile( //First loop
				"li %[count], 50\n"
				"1: \n"
				"vsetvli %[_gvl], %[avl], e64, m1\n"
				"vor.vv v3, v3, v3\n"
				"vor.vv v4, v4, v4\n"
				"vor.vv v5, v5, v5\n"
				"addi %[count], %[count], -1 \n"
				"bne %[count], x0, 1b \n"
				:[count]"+r"(N), [_gvl]"+r"(gvl) : [avl]"r"(42)
		);
	}
#endif
}
