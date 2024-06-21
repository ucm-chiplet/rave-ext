#define SEWS 4

struct qemu_counters{
				double scalar_instr;
				double vsetvl_instr;
				double vector_instr[SEWS];
				double vunit_instr[SEWS];
				double vstride_instr[SEWS];
				double agg_strides[SEWS];
				double vidx_instr[SEWS];
				double vfp_instr[SEWS];
				double vint_instr[SEWS];
				double vmask_instr[SEWS];
				double velem[SEWS];
				double moved_bytes_s;
				double moved_bytes_v;
				double flops;
//				double vl[SEWS];
};
typedef struct qemu_counters qemu_counters;

static qemu_counters total_counters;

//TODO: Names shouldn't be fixed size...
struct value_info{
	struct value_info * next;
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

value_info *  add_new_value(event_info * event, int val, char * name){
	value_info * new_values = (value_info*)malloc(sizeof(value_info));
	new_values -> next = NULL;
	new_values -> ID = val;
	strcpy(new_values->name, name);

	//Add it to the value queue
	//TODO: Doing a Stack instead of Queue would accelerate this to O(1)
	value_info * last = event->values;
	if (last == NULL){
		event->values = new_values;
		return new_values;
	}
	while (last != NULL){
		if (last->next == NULL){
			last->next = new_values;
			return new_values;
		}
		last = last->next;
	}
	return new_values;
}

void add_value_to_event(int id, int val, char * name){
	event_info * event = find_event(id);
	if (event == NULL) return;

	//If value already exists, just update its name
	value_info * value = find_value(event, val);
	if (value != NULL){
		strcpy(value->name, name);
		return;
	}

	//value not found: create it
	add_new_value(event,val,name);

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

									//TODO: Write csv here instead of saving?
									//print_region_csv(curr);

									double * event_counter_ptr = (double *)&curr->counters; //Traeating consecutive arrays as single array 
									double * total_counter_ptr = (double *)&total_counters;
									for(int c=0; c<sizeof(qemu_counters)/sizeof(double); ++c){
										event_counter_ptr[c] = total_counter_ptr[c] - event_counter_ptr[c];
									}
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
	region -> counters = total_counters;
	region -> closed = 0;
}



#define PERCENTAGE(x,y) ((y)==0?0:(100.0*(x))/(y))


void print_region_human(int nregion, region_stats* curr){
	//Print Region header
	value_info * v1 = find_value(curr->event, curr->value1);
	printf("Region #%d: Event %ld (%s), Value %ld (%s)\n", nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name);

	//Compute total instructions (sum of SEW!)
	qemu_counters * counters = &curr->counters;
	double scalinstr = counters->scalar_instr + counters->vsetvl_instr;
	double vecinstr = 0;
	for(int s=0; s<SEWS; ++s) vecinstr += counters->vector_instr[s];
	double totinstr = scalinstr + vecinstr; 

	//Others...
	double totbytes = counters->moved_bytes_s + counters->moved_bytes_v;
	printf("\t" "Moved bytes (Total): %.0f\n", totbytes);
	printf("\t\t" "Moved bytes (scalar): %.0f (%.2f %%)\n", counters->moved_bytes_s, PERCENTAGE(counters->moved_bytes_s,totbytes));
	printf("\t\t" "Moved bytes (vector): %.0f (%.2f %%)\n", counters->moved_bytes_v, PERCENTAGE(counters->moved_bytes_v,totbytes));

	//Print general counters
	printf("\t" "tot_instr: %.0f\n", totinstr);
	printf("\t\t"   "scalar_instr: %.0f (%.2f %%)\n", counters->scalar_instr, PERCENTAGE(counters->scalar_instr, totinstr)); 
	printf("\t\t"   "vsetvl_instr: %.0f (%.2f %%)\n", counters->vsetvl_instr, PERCENTAGE(counters->vsetvl_instr, totinstr));
	printf("\t\t"   "vector_instr: %.0f (%.2f %%)\n", counters->scalar_instr, PERCENTAGE(vecinstr, totinstr)); 

	//Print SEW-specific counters (vec)
	for(int s=0; s<SEWS; ++s){
		printf("\t\t\t" "SEW %d vector_instr: %.0f (%.2f %%)\n", 1<<(s+3),counters->vector_instr[s], PERCENTAGE(counters->vector_instr[s], vecinstr));
		if (counters->vector_instr[s]>0){
			double  totvmem		= counters->vunit_instr[s] + counters->vstride_instr[s] + counters->vidx_instr[s];
			double  totvarith	= counters->vfp_instr[s] + counters->vint_instr[s];
			double  totvother	= counters->vector_instr[s] - totvmem - totvarith - counters->vmask_instr[s];
			printf("\t\t\t\t"  "avg_VL: %.2f elements\n",counters->velem[s] / counters->vector_instr[s]);
			printf("\t\t\t\t"  "Arith: %.0f (%.2f %%)\n",totvarith, PERCENTAGE(totvarith, counters->vector_instr[s]));
			printf("\t\t\t\t\t"   "FP: %.0f (%.2f %%)\n",counters->vfp_instr[s], PERCENTAGE(counters->vfp_instr[s], totvarith));
			printf("\t\t\t\t\t"   "INT: %.0f (%.2f %%)\n", counters->vint_instr[s], PERCENTAGE(counters->vint_instr[s], totvarith));
			printf("\t\t\t\t"  "Mem: %.0f (%.2f %%)\n", totvmem, PERCENTAGE(totvmem, counters->vector_instr[s]));
			printf("\t\t\t\t\t"   "unit: %.0f (%.2f %%)\n", counters->vunit_instr[s], PERCENTAGE(counters->vunit_instr[s], totvmem));
			printf("\t\t\t\t\t"   "strided: %.0f (%.2f %%)\n", counters->vstride_instr[s], PERCENTAGE(counters->vstride_instr[s], totvmem));
			if (counters->vstride_instr[s] > 0) printf("\t\t\t\t\t\t"		"Avg. Stride (B): %.2f\n", counters->agg_strides[s] / counters->vstride_instr[s]);
			printf("\t\t\t\t\t"   "indexed: %.0f (%.2f %%)\n", counters->vidx_instr[s], PERCENTAGE(counters->vidx_instr[s], totvmem));
			printf("\t\t\t\t"  "Mask: %.0f (%.2f %%)\n", counters->vmask_instr[s], PERCENTAGE(counters->vmask_instr[s], counters->vector_instr[s]));
			printf("\t\t\t\t"  "Other: %.0f (%.2f %%)\n", totvother, PERCENTAGE(totvother, counters->vector_instr[s]));
		}
	}
}

static char first_csv_row=1;

void print_region_csv(FILE * fd, int nregion, region_stats* curr){
	if (first_csv_row){
		fprintf(fd,"region,event_id,event_name,value_id,value_name,tot_instr,scalar_instr,vsetvl_instr,vec_instr");
		for(int s=0; s<SEWS; ++s){
			fprintf(fd,",vector_sew%d_instr,vector_sew%d_elems,vector_sew%d_arith,vector_sew%d_fp,vector_sew%d_int,vector_sew%d_mem,vector_sew%d_memunit,vector_sew%d_memstride,vector_sew%d_memidx,vector_sew%d_mask,vector_sew%d_other,vector_sew%d_avg_stride",1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3), 1<<(s+3));
		}
		fprintf(fd,"moved_bytes_s,moved_bytes_v\n");
		first_csv_row = 0;
	}
	//Print Region header
	value_info * v1 = find_value(curr->event, curr->value1);
	fprintf(fd,"%d,%d,%s,%d,%s", nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name);

	//Compute total instructions (sum of SEW!)
	qemu_counters * counters = &curr->counters;
	double totinstr = counters->scalar_instr + counters->vsetvl_instr;
	double totvec = 0;
	for(int s=0; s<SEWS; ++s) totvec += counters->vector_instr[s];
	totinstr += totvec;

	//Print general counters
	fprintf(fd,",%.0f,%.0f,%.0f,%.0f", totinstr, counters->scalar_instr, counters->vsetvl_instr, totvec);

	//Print SEW-specific counters (vec)
	for(int s=0; s<SEWS; ++s){
		double  totvmem		= counters->vunit_instr[s] + counters->vstride_instr[s] + counters->vidx_instr[s];
		double  totvarith	= counters->vfp_instr[s] + counters->vint_instr[s];
		double  totvother	= counters->vector_instr[s] - totvmem - totvarith - counters->vmask_instr[s];
		double strides = (counters->vstride_instr[s] > 0)? counters->agg_strides[s] / counters->vstride_instr[s] : 0;
		fprintf(fd,",%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f", counters->vector_instr[s], counters->velem[s], totvarith, counters->vfp_instr[s], counters->vint_instr[s], totvmem, counters->vunit_instr[s], counters->vstride_instr[s], counters->vidx_instr[s], counters->vmask_instr[s], totvother,strides);
	}
	fprintf(fd,",%.0f,%.0f\n", counters->moved_bytes_s, counters->moved_bytes_v);
}

void print_report(){
	region_stats * curr = global_region;
	printf("-------------------"); printf(" REPORT "); printf("-------------------"); printf("\n");
	int nregion=0;
	while (curr!=NULL){
					//if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						print_region_human(nregion++, curr);
						//print_region_csv(nregion++, curr);
					}
					curr = curr->next;
	}
	printf("------------------------------------------------\n");
	fflush(stdout);
}
void print_csv(FILE * fd){
	region_stats * curr = global_region;
	int nregion=0;
	while (curr!=NULL){
					//if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						print_region_csv(fd,nregion++, curr);
					}
					curr = curr->next;
	}
	fflush(fd);
	fclose(fd);
}
void free_regions(){
	region_stats * curr = global_region;
	while (curr!=NULL){
		if (curr->prev!=NULL) free(curr->prev);
		curr = curr->next;
	}
}
