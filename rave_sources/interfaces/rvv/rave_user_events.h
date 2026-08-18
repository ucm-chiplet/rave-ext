#define rave_name_event_len(x,name,len)\
{\
	asm volatile("and x0, %0, x0\n" :: "r"(x));\
	asm volatile("sll x0, %0, %1\n" :: "r"(&name[0]), "r"(len));\
}
#define rave_name_value_len(x,y,name,len)\
{\
	asm volatile("and x0, %0, %1\n" :: "r"(x), "r"(y));\
	asm volatile("srl x0, %0, %1\n" :: "r"(&name[0]), "r"(len));\
}

#define rave_name_event(x,name) rave_name_event_len(x,name,-1)
#define rave_name_value(x,y,name) rave_name_value_len(x,y,name,-1)

#define rave_restart_trace() asm volatile("li x0, -2\n");

//Maintaining old start/stop functions instead of enable/disable
#define rave_start_trace() asm volatile("li x0, -3\n");
#define rave_stop_trace() asm volatile("li x0, -4\n");
//New naming convention:
#define rave_enable_trace() rave_start_trace()
#define rave_disable_trace() rave_stop_trace() 
#define rave_enable_regions() asm volatile("li x0, -7\n");
#define rave_disable_regions() asm volatile("li x0, -8\n");

#define rave_enable() rave_enable_regions() rave_enable_trace()
#define rave_disable() rave_disable_regions() rave_disable_trace()

#define rave_event_and_value(x,y) asm volatile("or x0, %0, %1\n"::"r"(x),"r"(y));

#define rave_begin_region_len(name,len) asm volatile("add x0, %0, %1\n" :: "r"(&name[0]), "r"(len))
#define rave_end_region_len(name,len) asm volatile("sub x0, %0, %1\n" :: "r"(&name[0]), "r"(len))

#define rave_begin_region(name) rave_begin_region_len(name,-1)
#define rave_end_region(name) rave_end_region_len(name,-1) 

