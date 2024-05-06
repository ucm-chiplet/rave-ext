
#define qemu_write_hex(x)\
			switch (x){\
							case 0: asm volatile("lui x0, 0\n"); break;\
							case 1: asm volatile("lui x0, 1\n"); break;\
							case 2: asm volatile("lui x0, 2\n"); break;\
							case 3: asm volatile("lui x0, 3\n"); break;\
							case 4: asm volatile("lui x0, 4\n"); break;\
							case 5: asm volatile("lui x0, 5\n"); break;\
							case 6: asm volatile("lui x0, 6\n"); break;\
							case 7: asm volatile("lui x0, 7\n"); break;\
							case 8: asm volatile("lui x0, 8\n"); break;\
							case 9: asm volatile("lui x0, 9\n"); break;\
							case 10: asm volatile("lui x0, 10\n"); break;\
							case 11: asm volatile("lui x0, 11\n"); break;\
							case 12: asm volatile("lui x0, 12\n"); break;\
							case 13: asm volatile("lui x0, 13\n"); break;\
							case 14: asm volatile("lui x0, 14\n"); break;\
							case 15: asm volatile("lui x0, 15\n"); break;\
			}

#include <stdio.h>

#define  qemu_name(name)\
{\
		asm volatile("li x0, -1\n");\
		for(int i=0; name[i]!='\0'; ++i){\
			int y=(int)name[i];\
			for(int tmp=y; tmp>0; tmp>>=4){\
				qemu_write_hex(tmp&0x0f);\
			}\
		}\
		asm volatile("li x0, -1\n");\
}


#define qemu_name_event(x,name)\
{\
	asm volatile("lui x0, %0\n"::"i"(x));\
	qemu_name(name);\
}
#define qemu_name_value(x,y,name)\
{\
	asm volatile("lui x0, %0\n"::"i"(x));\
	asm volatile("lui x0, %0\n"::"i"(y));\
	qemu_name(name);\
}

#define qemu_restart_trace() asm volatile("li x0, -2\n");
#define qemu_start_trace() asm volatile("li x0, -3\n");
#define qemu_stop_trace() asm volatile("li x0, -4\n");

#define qemu_event(x,y) asm volatile("or x0, %0, %1\n"::"r"(x),"r"(y));
