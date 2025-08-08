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

void rave_eventandcounters(int event, int value, int cpu_index, rave_counters * current_counters){
	//Find open region to close it:
	region_stats * region = last_region; //global_region;
	event_info * eventinfo = find_event(event);
	while (region!=NULL){
					if (region->event->ID==event && !region->closed && region->opened_by == cpu_index){
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




extern int mpi_rank;

void print_events_report(FILE * fd){
	region_stats * curr = global_region;
	fprintf(fd,"-------------------" " REPORT " "-------------------" "\n"); 
	int nregion=0;
	while (curr!=NULL){
					//if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						value_info * v1 = find_value(curr->event, curr->value1);
						fprintf(fd,"Region #%d: Event %ld (%s), Value %ld (%s), Rank %d, Thread %d\n", nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name, mpi_rank, curr->opened_by);
						print_counters_human(fd, &curr->counters);
						nregion++;
					}
					curr = curr->next;
	}
	fprintf(fd, "------------------------------------------------\n");
	fflush(fd);
	fclose(fd);
}
void print_events_csv(FILE * fd){
	fprintf(fd,"process_id,thread_id,region,event_id,event_name,value_id,value_name,");
	print_csv_header(fd);
	region_stats * curr = global_region;
	int nregion=0;
	while (curr!=NULL){
					//if (curr->prev!=NULL) free(curr->prev);
					if (curr->closed){
						value_info * v1 = find_value(curr->event, curr->value1);
						fprintf(fd,"%d,%d,%d,%ld,%s,%ld,%s",mpi_rank, curr->opened_by,nregion,curr->event->ID, curr->event->name, curr->value1, v1==NULL?"-":v1->name);
						print_counters_csv(fd, &curr->counters);
						nregion++;
					}
					curr = curr->next;
	}
	fflush(fd);
	fclose(fd);
}
void free_event_regions(){
	region_stats * curr = global_region;
	while (curr!=NULL){
		if (curr->prev!=NULL) free(curr->prev);
		curr = curr->next;
	}
}
