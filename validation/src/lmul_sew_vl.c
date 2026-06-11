#ifdef RVV0_7_1

#define vsetvl(lmul,sew,vl)({\
		long gvl;\
		asm volatile("vsetvli %0, %1, " #sew ", " #lmul "\n" : "+r"(gvl) : "r"(vl));\
		gvl;\
})

#define vsetvlmax(lmul,sew)({\
		long gvl;\
		asm volatile("vsetvli %0, x0, " #sew ", " #lmul "\n" : "+r"(gvl));\
		gvl;\
})

#else

#define vsetvl(lmul,sew,vl)({\
		long gvl;\
		asm volatile("vsetvli %0, %1, " #sew ", " #lmul ", ta, ma\n" : "+r"(gvl) : "r"(vl));\
		gvl;\
})

#define vsetvlmax(lmul,sew)({\
		long gvl;\
		asm volatile("vsetvli %0, x0, " #sew ", " #lmul ", ta, ma\n" : "+r"(gvl));\
		gvl;\
})

#endif 

#include <stdio.h>

#define TEST(lmul, sew){\
	vlmax = vsetvlmax(lmul, sew);\
	float avgvl = (3.0*vlmax + 2.0*(vlmax/2))/5.0;\
	sprintf(region_name, #lmul "_" #sew "_%.2f", avgvl );\
	rave_begin_region(region_name);\
	gvl = vsetvl(lmul, sew, vlmax);\
	asm volatile("vor.vv v0, v0, v0\n");\
	asm volatile("vor.vv v0, v0, v0\n");\
	asm volatile("vor.vv v0, v0, v0\n");\
	gvl = vsetvl(lmul, sew, vlmax/2);\
	asm volatile("vor.vv v0, v0, v0\n");\
	asm volatile("vor.vv v0, v0, v0\n");\
	rave_end_region(region_name);\
}


#include "rave_user_events.h"
int main(){
	long gvl;
	long vlmax;
	char region_name[64];

	int seed = 0xBEEFCAFE;

	TEST(m1, e64);
	TEST(m1, e32);
	TEST(m1, e16);
	TEST(m1, e8);
	TEST(m2, e64);
	TEST(m2, e32);
	TEST(m2, e16);
	TEST(m2, e8);
	TEST(m4, e64);
	TEST(m4, e32);
	TEST(m4, e16);
	TEST(m4, e8);
	TEST(m8, e64);
	TEST(m8, e32);
	TEST(m8, e16);
	TEST(m8, e8);

	return 0;
}
