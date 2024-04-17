
static uint64_t tot_scalar_instr=0;
static uint64_t tot_vector_instr=0;

static uint64_t tot_vsetvl_instr=0;
static uint64_t tot_vfp_instr=0;
static uint64_t tot_vint_instr=0;
static uint64_t tot_vmask_instr=0;
static uint64_t tot_vunit_instr=0; 
static uint64_t tot_vstride_instr=0; 
static uint64_t tot_vidx_instr=0; 
static uint64_t tot_velem=0;
//My definition of region: code enclosed between two values of one event
struct region_stats{
				struct region_stats * prev;
				struct region_stats * next;
				/*
				char name[64];
				int64_t event;
				*/
				event_info * event;
				int64_t value1; //Last event
				int64_t value2; //Next event
				uint64_t scalar_instr;
				uint64_t vector_instr;
				uint64_t vsetvl_instr;
				uint64_t vunit_instr;
				uint64_t vstride_instr;
				uint64_t vidx_instr;
				uint64_t vfp_instr;
				uint64_t vint_instr;
				uint64_t vmask_instr;
				uint64_t velem;
				char closed;
};


typedef struct region_stats region_stats; 

region_stats * global_region;
//Returns closed or opened region 
region_stats * qemu_eventandcounters(int event, int value){
	//Find open region:
	region_stats * curr = global_region;
	region_stats * prev = curr;
	while (curr!=NULL){
					if (curr->event->ID==event && !curr->closed){
									//Close region
									curr->closed = 1;
									curr->scalar_instr = tot_scalar_instr - curr->scalar_instr;
									curr->vector_instr = tot_vector_instr - curr->vector_instr;
									curr->vsetvl_instr = tot_vsetvl_instr - curr->vsetvl_instr;
									curr->vunit_instr = tot_vunit_instr - curr->vunit_instr;
									curr->vstride_instr = tot_vstride_instr - curr->vstride_instr;
									curr->vidx_instr = tot_vidx_instr - curr->vidx_instr;
									curr->vfp_instr = tot_vfp_instr - curr->vfp_instr;
									curr->vint_instr = tot_vint_instr - curr->vint_instr;
									curr->vmask_instr = tot_vmask_instr - curr->vmask_instr;
									curr->velem = tot_velem - curr->velem;
									curr->value2 = value;
									//TODO: Write here instead of saving?
									return curr;
					}
					prev=curr;
					curr = curr->next;
	}
	//Not found: open a new one (if value is not 0)
	if (value==0) return NULL;
	region_stats * new_region = (region_stats*)malloc(sizeof(region_stats));

	new_region -> prev = prev;
	new_region -> next = NULL;
	new_region -> event = find_event(event);
	if (new_region -> event == NULL){
					new_region -> event = add_event(event,"-");
	}
	new_region -> value1 = value;
	restart_region(new_region);

	if(prev!=NULL) prev -> next = new_region;
	
	return new_region;
}

void restart_region(region_stats * region){
	region -> scalar_instr = tot_scalar_instr;
	region -> vector_instr = tot_vector_instr;
	region -> vsetvl_instr = tot_vsetvl_instr;
	region -> vunit_instr = tot_vunit_instr;
	region -> vstride_instr = tot_vstride_instr;
	region -> vidx_instr = tot_vidx_instr;
	region -> vfp_instr = tot_vfp_instr;
	region -> vint_instr = tot_vint_instr;
	region -> vmask_instr = tot_vmask_instr;
	region -> velem = tot_velem;
	region -> closed = 0;
}

void print_regions(){
	region_stats * curr = global_region;
	while (curr!=NULL){
					if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						uint64_t totinstr = (curr->scalar_instr+curr->vector_instr+curr->vsetvl_instr);
						uint64_t totvmem = (curr->vunit_instr + curr->vstride_instr + curr->vidx_instr);
						uint64_t totvarith = (curr->vfp_instr + curr->vint_instr);
						uint64_t totvother = curr->vector_instr - totvmem - totvarith - curr->vmask_instr;

						value_info * v1 = find_value(curr->event, curr->value1);
						value_info * v2 = find_value(curr->event, curr->value2);

						printf("Event %ld (%s), Value %ld (%s)\n"
									"\t" "tot_instr: %lu\n"
									"\t\t" "scalar_instr: %lu (%.2f %%)\n"
									"\t\t" "vsetvl_instr: %lu (%.2f %%)\n"
									"\t\t" "vector_instr: %lu (%.2f %%)\n"
									"\t\t\t" "avg_VL: %.2f\n"
									"\t\t\t" "Arith: %lu (%.2f %%)\n"
									"\t\t\t\t" "FP: %lu (%.2f %%)\n"
									"\t\t\t\t" "INT: %lu (%.2f %%)\n"
									"\t\t\t" "Mem: %lu (%.2f %%)\n"
									"\t\t\t\t" "unit: %lu (%.2f %%)\n"
									"\t\t\t\t" "strided: %lu (%.2f %%)\n"
									"\t\t\t\t" "indexed: %lu (%.2f %%)\n"
									"\t\t\t" "Mask: %lu (%.2f %%)\n"
									"\t\t\t" "Other: %lu (%.2f %%)\n"
													,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name

													,totinstr

													,curr->scalar_instr, 100.0*(double)curr->scalar_instr/(double)totinstr
													,curr->vsetvl_instr, 100.0*(double)curr->vsetvl_instr/(double)totinstr
													,curr->vector_instr, 100.0*(double)curr->vector_instr/(double)totinstr

													,curr->vector_instr==0?0 : (double)curr->velem / (double)curr->vector_instr
													,totvarith,  curr->vector_instr==0?0:100.0*totvarith/curr->vector_instr 
													,curr->vfp_instr,  totvarith==0?0:100.0*curr->vfp_instr / totvarith
													,curr->vint_instr,  totvarith==0?0:100.0*curr->vint_instr / totvarith
													,totvmem, curr->vector_instr==0?0:100.0*totvmem/curr->vector_instr 
													,curr->vunit_instr,  totvmem==0?0:100.0*curr->vunit_instr / totvmem
													,curr->vstride_instr,  totvmem==0?0:100.0*curr->vstride_instr / totvmem
													,curr->vidx_instr,  totvmem==0?0:100.0*curr->vidx_instr / totvmem
													,curr->vmask_instr,  curr->vector_instr==0?0:100.0*curr->vmask_instr/curr->vector_instr
													,totvother, curr->vector_instr==0?0:100.0*totvother/curr->vector_instr 
													);
					}
					curr = curr->next;
	}
}
