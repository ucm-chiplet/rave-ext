static double tot_scalar_instr=0;
static double tot_vector_instr=0;
static double tot_vsetvl_instr=0;
static double tot_vfp_instr=0;
static double tot_vint_instr=0;
static double tot_vmask_instr=0;
static double tot_vunit_instr=0; 
static double tot_vstride_instr=0; 
static double tot_vidx_instr=0; 
static double tot_velem=0;

struct qemu_counters{
				double scalar_instr;
				double vector_instr;
				double vsetvl_instr;
				double vunit_instr;
				double vstride_instr;
				double vidx_instr;
				double vfp_instr;
				double vint_instr;
				double vmask_instr;
				double velem;
};
typedef struct qemu_counters qemu_counters;
//TODO: Names shouldn't be fixed size...
struct value_info{
	struct value_info * next;
	qemu_counters average_counters;
	int n;
	char name[64];
	int64_t ID;
};
typedef struct value_info value_info;

struct event_info{
	struct event_info * next;
	char name[64];
	int64_t ID;
	value_info * values;
};
typedef struct event_info event_info;
event_info * first_event_info = NULL;
event_info * last_event_info = NULL;


event_info * find_event(int id){
	event_info * curr = first_event_info;
	while (curr!=NULL){
		if (curr->ID == id) return curr;
		curr = curr->next;
	}
	return NULL;
}

value_info * find_value(event_info * event, int val){
	value_info * curr = event->values;
	while (curr!=NULL){
		if (curr->ID == val) return curr;
		curr = curr->next;
	}
	return NULL;
}

#define ROLLING_AVG(old,add,new_n) old = ((old)*(new_n - 1) + (add))/(new_n)
void add_counters_to_value(qemu_counters * from, value_info * value){
	if (from==NULL || value==NULL){
		return;
	}
	value -> n = value -> n  + 1;
	ROLLING_AVG (value -> average_counters.scalar_instr, from -> scalar_instr, value -> n); 
	ROLLING_AVG (value -> average_counters.vector_instr, from -> vector_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vsetvl_instr, from -> vsetvl_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vunit_instr, from -> vunit_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vstride_instr, from -> vstride_instr, value -> n); 
	ROLLING_AVG (value -> average_counters.vidx_instr, from -> vidx_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vfp_instr, from -> vfp_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vint_instr, from -> vint_instr, value -> n);
	ROLLING_AVG (value -> average_counters.vmask_instr, from -> vmask_instr, value -> n);
	ROLLING_AVG (value -> average_counters.velem, from -> velem, value -> n); 
}


void add_value_to_event(int id, int val, char * name){
	event_info * event = find_event(id);
	if (event == NULL) return;
	value_info * value = find_value(event, val);
	if (value != NULL){
		strcpy(value->name, name);
		return;
	}
	//value not found: create it
	value_info * new_values = (value_info*)malloc(sizeof(value_info));
	new_values -> next = NULL;
	new_values -> ID = val;
	strcpy(new_values->name, name);

	new_values -> n = 0;
	new_values -> average_counters.scalar_instr = 0; 
	new_values -> average_counters.vector_instr = 0;
	new_values -> average_counters.vsetvl_instr = 0;
	new_values -> average_counters.vunit_instr = 0;
	new_values -> average_counters.vstride_instr = 0;
	new_values -> average_counters.vidx_instr = 0;
	new_values -> average_counters.vfp_instr = 0;
	new_values -> average_counters.vint_instr = 0;
	new_values -> average_counters.vmask_instr = 0;
	new_values -> average_counters.velem = 0;

	value_info * last = event->values;
	if (last == NULL){
		event->values = new_values;
		return;
	}
	while (last != NULL){
		if (last->next == NULL){
			last->next = new_values;
			return;
		}
		last = last->next;
	}
}

event_info * add_event(int id, char *name){

	event_info * event = find_event(id);
	if (event != NULL){
		strcpy(event->name, name);
		return event;
	}
	//event not found: create it
	event_info * new_event = (event_info*)malloc(sizeof(event_info));
	new_event -> next = NULL;
	new_event -> ID = id;
	new_event -> values = NULL;
	strcpy(new_event->name, name);

	if (first_event_info==NULL) first_event_info = new_event;
	if (last_event_info!=NULL) last_event_info->next = new_event;
	last_event_info = new_event;

	return new_event;
}

char * get_event_value_name(event_info * event, int val){
	if (event==NULL) return "Value name not found";
	value_info * value = event->values; 
	while(value!=NULL){
		if (value->ID == val){
			return value->name;
		}
		value = value->next;
	}
	return "Value name not found";
}

/////////////

//My definition of region: code enclosed between two values of one event
struct region_stats{
				struct region_stats * prev;
				struct region_stats * next;
				event_info * event;
				int64_t value1; //Last event
				int64_t value2; //Next event
				qemu_counters counters;
				char closed;
};

typedef struct region_stats region_stats; 

region_stats * global_region;

void qemu_eventandcounters(int event, int value){
	//Find open region to close it:
	region_stats * curr = global_region;
	region_stats * prev = curr;
	event_info * eventinfo = find_event(event);
	while (curr!=NULL){
					prev=curr;
					if (curr->event->ID==event && !curr->closed){
									//Close region
									curr->closed = 1;
									curr->value2 = value;

									curr->counters.scalar_instr = tot_scalar_instr - curr->counters.scalar_instr;
									curr->counters.vector_instr = tot_vector_instr - curr->counters.vector_instr;
									curr->counters.vsetvl_instr = tot_vsetvl_instr - curr->counters.vsetvl_instr;
									curr->counters.vunit_instr = tot_vunit_instr - curr->counters.vunit_instr;
									curr->counters.vstride_instr = tot_vstride_instr - curr->counters.vstride_instr;
									curr->counters.vidx_instr = tot_vidx_instr - curr->counters.vidx_instr;
									curr->counters.vfp_instr = tot_vfp_instr - curr->counters.vfp_instr;
									curr->counters.vint_instr = tot_vint_instr - curr->counters.vint_instr;
									curr->counters.vmask_instr = tot_vmask_instr - curr->counters.vmask_instr;
									curr->counters.velem = tot_velem - curr->counters.velem;

									add_counters_to_value(&curr->counters, find_value(eventinfo, curr->value1));

									//TODO: Write summary here instead of saving?
									//return curr;
									break; 
					}
					curr = curr->next;
	}
	//Open a new one (if value is not 0)
	if (value==0){
	 	return;
	}
	region_stats * new_region = (region_stats*)malloc(sizeof(region_stats));

	new_region -> prev = prev;
	new_region -> next = NULL;
	new_region -> event = eventinfo;
	if (new_region -> event == NULL){
					new_region -> event = add_event(event,"-");
	}
	new_region -> value1 = value;
	restart_region(new_region);

	if(prev!=NULL){
		prev -> next = new_region;
	}else{
		global_region = new_region;
	}

	return;
}

void restart_region(region_stats * region){
	region -> counters.scalar_instr = tot_scalar_instr;
	region -> counters.vector_instr = tot_vector_instr;
	region -> counters.vsetvl_instr = tot_vsetvl_instr;
	region -> counters.vunit_instr = tot_vunit_instr;
	region -> counters.vstride_instr = tot_vstride_instr;
	region -> counters.vidx_instr = tot_vidx_instr;
	region -> counters.vfp_instr = tot_vfp_instr;
	region -> counters.vint_instr = tot_vint_instr;
	region -> counters.vmask_instr = tot_vmask_instr;
	region -> counters.velem = tot_velem;
	region -> closed = 0;
}

void print_regions(){
	region_stats * curr = global_region;
	while (curr!=NULL){
					if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						double  totinstr	= (curr->counters.scalar_instr + curr->counters.vector_instr + curr->counters.vsetvl_instr);
						double  totvmem		= (curr->counters.vunit_instr + curr->counters.vstride_instr + curr->counters.vidx_instr);
						double  totvarith	= (curr->counters.vfp_instr + curr->counters.vint_instr);
						double  totvother	= (curr->counters.vector_instr - totvmem - totvarith - curr->counters.vmask_instr);

						value_info * v1 = find_value(curr->event, curr->value1);
						value_info * v2 = find_value(curr->event, curr->value2);

						printf("Event %ld (%s), Value %ld (%s)\n"
									"\t" "tot_instr: %.0f\n"
									"\t\t" "scalar_instr: %.0f (%.2f %%)\n"
									"\t\t" "vsetvl_instr: %.0f (%.2f %%)\n"
									"\t\t" "vector_instr: %.0f (%.2f %%)\n"
									"\t\t\t" "avg_VL: %.2f\n"
									"\t\t\t" "Arith: %.0f (%.2f %%)\n"
									"\t\t\t\t" "FP: %.0f (%.2f %%)\n"
									"\t\t\t\t" "INT: %.0f (%.2f %%)\n"
									"\t\t\t" "Mem: %.0f (%.2f %%)\n"
									"\t\t\t\t" "unit: %.0f (%.2f %%)\n"
									"\t\t\t\t" "strided: %.0f (%.2f %%)\n"
									"\t\t\t\t" "indexed: %.0f (%.2f %%)\n"
									"\t\t\t" "Mask: %.0f (%.2f %%)\n"
									"\t\t\t" "Other: %.0f (%.2f %%)\n"
													,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name

													,totinstr

													,curr->counters.scalar_instr, totinstr==0?0:100.0*curr->counters.scalar_instr/totinstr
													,curr->counters.vsetvl_instr, totinstr==0?0:100.0*curr->counters.vsetvl_instr/totinstr
													,curr->counters.vector_instr, totinstr==0?0:100.0*curr->counters.vector_instr/totinstr

													,curr->counters.vector_instr==0?0 : curr->counters.velem / curr->counters.vector_instr
													,totvarith,  curr->counters.vector_instr==0?0:100.0*totvarith/curr->counters.vector_instr 
													,curr->counters.vfp_instr,  totvarith==0?0:100.0*curr->counters.vfp_instr / totvarith
													,curr->counters.vint_instr,  totvarith==0?0:100.0*curr->counters.vint_instr / totvarith
													,totvmem, curr->counters.vector_instr==0?0:100.0*totvmem/curr->counters.vector_instr 
													,curr->counters.vunit_instr,  totvmem==0?0:100.0*curr->counters.vunit_instr / totvmem
													,curr->counters.vstride_instr,  totvmem==0?0:100.0*curr->counters.vstride_instr / totvmem
													,curr->counters.vidx_instr,  totvmem==0?0:100.0*curr->counters.vidx_instr / totvmem
													,curr->counters.vmask_instr,  curr->counters.vector_instr==0?0:100.0*curr->counters.vmask_instr/curr->counters.vector_instr
													,totvother, curr->counters.vector_instr==0?0:100.0*totvother/curr->counters.vector_instr 
													);
					}
					curr = curr->next;
	}
}

void print_averages(){
	event_info * curr = first_event_info;
	while (curr!=NULL){
		value_info * value = curr->values;
		while (value != NULL){
			if (value->n != 0){
				printf("Avg Instr %d %d: %.2f\n", curr->ID, value->ID, value->average_counters.scalar_instr);
			}
			value = value->next;
		}
		curr = curr->next;
	}
	return NULL;
}
