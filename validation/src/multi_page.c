int main(){
	long data[256*8];
	long gvl;
#ifdef RVV0_7_1
	asm volatile("vsetvli %0, x0, e64, m8\n":"+r"(gvl));
	asm volatile("vle.v v0, (%0)\n"::"r"(data));
#else
	asm volatile("vsetvli %0, x0, e64, m8, ta, ma\n":"+r"(gvl));
	asm volatile("vle64.v v0, (%0)\n"::"r"(data));
#endif
	return 0;
}
