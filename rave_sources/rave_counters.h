#define SEWS 4


struct rave_counters{
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
};
typedef struct rave_counters rave_counters;

void reset_counters(rave_counters * c){
	double * ptr = (double *)c;
	for(int i=0; i<sizeof(rave_counters)/sizeof(double); ++i){
		ptr[i]=0.0;
	}
}

//c1 = c2
void copy_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c]; 
	}
}

//c1 += c2;
void add_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] += c2_ptr[c]; 
	}
}

//c1 = c2-c1
void update_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c] - c1_ptr[c]; 
	}
}

//static rave_counters total_counters;

//TODO: Names shouldn't be fixed size...
struct value_info{
	struct value_info * next;
	char * name;
	int64_t ID;
};
typedef struct value_info value_info;

struct event_info{
	struct event_info * next;
	char * name; 
	int64_t ID;
	value_info * values;
	value_info * last_value;
};
typedef struct event_info event_info;
event_info * first_event_info = NULL;
event_info * last_event_info = NULL;

#define my_strcpy(dst, src)\
{\
	int len = strlen(src);\
	dst = malloc(len+1);\
	strcpy(dst,src);\
}


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
	my_strcpy(new_values->name, name);

	//Add it to the value stack
	if (event->last_value == NULL){
		event->values = new_values;
		event->last_value = new_values;
		return new_values;
	}
	event->last_value->next = new_values;
	event->last_value = new_values;
	return new_values;
}

void add_value_to_event(int id, int val, char * name){
	event_info * event = find_event(id);
	if (event == NULL){
		//printf("Event %d not found\n",id);
	 	return;
	}

	//If value already exists, just update its name
	value_info * value = find_value(event, val);
	if (value != NULL){
		free(value->name);
		//printf("%d %d found, update name from %s to %s\n", id,val,value->name, name);
		my_strcpy(value->name, name);
		return;
	}

	//value not found: create it
	//printf("%d %d not found, named it %s\n",id,val,name);
	add_new_value(event,val,name);
}

//For symbols
int add_value_name_to_event(int id, char * name){
	event_info * event = find_event(id);
	if (event == NULL) return -1;

	value_info * curr = event->values;
	int val = 1;
	while (curr!=NULL){
		if (!strcmp(curr->name, name)){
		 	return curr->ID;
		}
		curr = curr->next;
		++val;
	}
	//value not found: create it
	add_new_value(event,val,name);
	return val;
}

event_info * add_event(int id, char *name){

	event_info * event = find_event(id);
	if (event != NULL){
		free(event->name);
		my_strcpy(event->name, name);
		return event;
	}
	//event not found: create it
	event_info * new_event = (event_info*)malloc(sizeof(event_info));
	new_event -> next = NULL;
	new_event -> ID = id;
	new_event -> values = NULL;
	new_event -> last_value = NULL;
	my_strcpy(new_event->name, name);

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
				rave_counters counters;
				int opened_by;
				char closed;
};
typedef struct region_stats region_stats; 

region_stats * global_region;
region_stats * last_region;

#if 0
void print_regions(int cpu_index){
	region_stats * curr = last_region;
	while (curr != NULL){
		printf("%d: (%d, %d, [cpu%d], {%d})\n",cpu_index,curr->event->ID,curr->value1,curr->opened_by,curr->closed);
		curr = curr->prev;
	}
	printf("\n");
}
#endif

void rave_eventandcounters(int event, int value, int cpu_index, rave_counters * current_counters){
	//Find open region to close it:
	region_stats * region = last_region; //global_region;
	event_info * eventinfo = find_event(event);
	while (region!=NULL){
#if 0
					printf(" %d: Checking (%d,%d,[%d],{%d})",cpu_index,region->event->ID,region->value1,region->opened_by,region->closed);
					if (region->event->ID==event){
						if (region->opened_by != cpu_index) printf(" -> not mine\n");
						else if (region->closed) printf(" -> mine but already closed\n");
						else printf(" -> mine and open!\n");
					}else{
						printf(" -> Not the same event (%d vs %d)\n",region->event->ID,event);
					}
#endif
					if (region->event->ID==event && !region->closed && region->opened_by == cpu_index){
									//printf("\t%d: Closes (%d,%d,[%d])\n",cpu_index,event,region->value1,region->opened_by);
									//printf(" -> I close it\n");
									//Close region
									region->closed = 1;
									region->value2 = value;

									//TODO: Write csv here instead of saving?
									//print_region_csv(region);

									update_counters(&region->counters, current_counters);
									break; 
					}
					region = region->prev;
	}
	//Open a new one (if value is not 0)
	if (value==0){
	 	return;
	}
	region_stats * new_region = (region_stats*)malloc(sizeof(region_stats));

	new_region -> prev = last_region;
	new_region -> next = NULL;
	new_region -> event = eventinfo;
	new_region -> opened_by = cpu_index;
	if (new_region -> event == NULL){
					new_region -> event = add_event(event,"-");
	}
	new_region -> value1 = value;
	new_region -> counters = *current_counters;
	new_region -> closed = 0;

	if(last_region!=NULL){
		last_region -> next = new_region;
	}else{
		global_region = new_region;
	}
	last_region = new_region;
	//printf("\t%d: Opens (%d,%d,[%d])\n",cpu_index,event,value,cpu_index);

	return;
}


#define PERCENTAGE(x,y) ((y)==0?0:(100.0*(x))/(y))


extern int mpi_rank;
void print_region_human(FILE * fd, int nregion, region_stats* curr){
	//Print Region header
	value_info * v1 = find_value(curr->event, curr->value1);
	fprintf(fd,"Region #%d: Event %ld (%s), Value %ld (%s), Rank %d, Thread %d\n", nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name, mpi_rank, curr->opened_by);

	//Compute total instructions (sum of SEW!)
	rave_counters * counters = &curr->counters;
	double scalinstr = counters->scalar_instr + counters->vsetvl_instr;
	double vecinstr = 0;
	for(int s=0; s<SEWS; ++s) vecinstr += counters->vector_instr[s];
	double totinstr = scalinstr + vecinstr; 

	//Others...
	double totbytes = counters->moved_bytes_s + counters->moved_bytes_v;
	fprintf(fd,"\t" "Moved bytes (Total): %.0f\n", totbytes);
	fprintf(fd,"\t\t" "Moved bytes (scalar): %.0f (%.2f %%)\n", counters->moved_bytes_s, PERCENTAGE(counters->moved_bytes_s,totbytes));
	fprintf(fd,"\t\t" "Moved bytes (vector): %.0f (%.2f %%)\n", counters->moved_bytes_v, PERCENTAGE(counters->moved_bytes_v,totbytes));

	//Print general counters
	fprintf(fd,"\t" "tot_instr: %.0f\n", totinstr);
	fprintf(fd,"\t\t"   "scalar_instr: %.0f (%.2f %%)\n", counters->scalar_instr, PERCENTAGE(counters->scalar_instr, totinstr)); 
	fprintf(fd,"\t\t"   "vsetvl_instr: %.0f (%.2f %%)\n", counters->vsetvl_instr, PERCENTAGE(counters->vsetvl_instr, totinstr));
	fprintf(fd,"\t\t"   "vector_instr: %.0f (%.2f %%)\n", vecinstr, PERCENTAGE(vecinstr, totinstr)); 

	//Print SEW-specific counters (vec)
	for(int s=0; s<SEWS; ++s){
		fprintf(fd,"\t\t\t" "SEW %d vector_instr: %.0f (%.2f %%)\n", 1<<(s+3),counters->vector_instr[s], PERCENTAGE(counters->vector_instr[s], vecinstr));
		if (counters->vector_instr[s]>0){
			double  totvmem		= counters->vunit_instr[s] + counters->vstride_instr[s] + counters->vidx_instr[s];
			double  totvarith	= counters->vfp_instr[s] + counters->vint_instr[s];
			double  totvother	= counters->vector_instr[s] - totvmem - totvarith - counters->vmask_instr[s];
			fprintf(fd,"\t\t\t\t"  "avg_VL: %.2f elements\n",counters->velem[s] / counters->vector_instr[s]);
			fprintf(fd,"\t\t\t\t"  "Arith: %.0f (%.2f %%)\n",totvarith, PERCENTAGE(totvarith, counters->vector_instr[s]));
			fprintf(fd,"\t\t\t\t\t"   "FP: %.0f (%.2f %%)\n",counters->vfp_instr[s], PERCENTAGE(counters->vfp_instr[s], totvarith));
			fprintf(fd,"\t\t\t\t\t"   "INT: %.0f (%.2f %%)\n", counters->vint_instr[s], PERCENTAGE(counters->vint_instr[s], totvarith));
			fprintf(fd,"\t\t\t\t"  "Mem: %.0f (%.2f %%)\n", totvmem, PERCENTAGE(totvmem, counters->vector_instr[s]));
			fprintf(fd,"\t\t\t\t\t"   "unit: %.0f (%.2f %%)\n", counters->vunit_instr[s], PERCENTAGE(counters->vunit_instr[s], totvmem));
			fprintf(fd,"\t\t\t\t\t"   "strided: %.0f (%.2f %%)\n", counters->vstride_instr[s], PERCENTAGE(counters->vstride_instr[s], totvmem));
			if (counters->vstride_instr[s] > 0) fprintf(fd,"\t\t\t\t\t\t"		"Avg. Stride (B): %.2f\n", counters->agg_strides[s] / counters->vstride_instr[s]);
			fprintf(fd,"\t\t\t\t\t"   "indexed: %.0f (%.2f %%)\n", counters->vidx_instr[s], PERCENTAGE(counters->vidx_instr[s], totvmem));
			fprintf(fd,"\t\t\t\t"  "Mask: %.0f (%.2f %%)\n", counters->vmask_instr[s], PERCENTAGE(counters->vmask_instr[s], counters->vector_instr[s]));
			fprintf(fd,"\t\t\t\t"  "Other: %.0f (%.2f %%)\n", totvother, PERCENTAGE(totvother, counters->vector_instr[s]));
		}
	}
}

char first_csv_row=1;

void print_region_csv(FILE * fd, int nregion, region_stats* curr){
	if (first_csv_row){
		fprintf(fd,"process_id,thread_id,region,event_id,event_name,value_id,value_name,tot_instr,scalar_instr,vsetvl_instr,vec_instr");
		for(int s=0; s<SEWS; ++s){
			fprintf(fd,",vector_sew%d_instr,vector_sew%d_elems,vector_sew%d_arith,vector_sew%d_fp,vector_sew%d_int,vector_sew%d_mem,vector_sew%d_memunit,vector_sew%d_memstride,vector_sew%d_memidx,vector_sew%d_mask,vector_sew%d_other,vector_sew%d_avg_stride",1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3),1<<(s+3), 1<<(s+3));
		}
		fprintf(fd,",moved_bytes_s,moved_bytes_v\n");
		first_csv_row = 0;
	}
	//Print Region header
	value_info * v1 = find_value(curr->event, curr->value1);
	fprintf(fd,"%d,%d,%d,%ld,%s,%ld,%s",mpi_rank, curr->opened_by,nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name);

	//Compute total instructions (sum of SEW!)
	rave_counters * counters = &curr->counters;
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
		fprintf(fd,",%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.0f,%.2f", counters->vector_instr[s], counters->velem[s], totvarith, counters->vfp_instr[s], counters->vint_instr[s], totvmem, counters->vunit_instr[s], counters->vstride_instr[s], counters->vidx_instr[s], counters->vmask_instr[s], totvother,strides);
	}
	fprintf(fd,",%.0f,%.0f\n", counters->moved_bytes_s, counters->moved_bytes_v);
}

void print_report(FILE * fd){
	region_stats * curr = global_region;
	fprintf(fd,"-------------------" " REPORT " "-------------------" "\n"); 
	int nregion=0;
	while (curr!=NULL){
					//if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						print_region_human(fd, nregion++, curr);
					}
					curr = curr->next;
	}
	fprintf(fd, "------------------------------------------------\n");
	fflush(fd);
	fclose(fd);
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
