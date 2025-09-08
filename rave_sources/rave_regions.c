struct region_t{
	char closed;
	int nesting;
	char * name;
	int executions;
	rave_counters delta_counters;
	rave_counters acc_counters;
	int opened_by;
};
typedef struct region_t region_t;


struct region_node_t{
	region_t region;
	struct region_node_t * parent;
	struct region_node_t * first_child;
	struct region_node_t * last_child;
	struct region_node_t * next_sibling;
};
typedef struct region_node_t region_node_t;

struct track_regions_t{
	region_node_t * first_region;
	region_node_t * last_region; //TODO: This should be THREAD-independent?
	int nesting; //TODO: This should be THREAD-independent?
	int total_regions;
	int max_nested;
	int max_name;
};
typedef struct track_regions_t track_regions_t;
track_regions_t track_regions;

struct region_unique_list_t{
	int region_id;
	region_t * region;
	struct region_unique_list_t * next;
};
typedef struct region_unique_list_t region_unique_list_t;
region_unique_list_t *first_unique_region;
region_unique_list_t *last_unique_region; 

static int name_to_id(const char * name){
	region_unique_list_t * curr = first_unique_region;
	while (curr != NULL){
		if (curr->region != NULL && strcmp(curr->region->name,name)==0){
			return curr->region_id;
		}
		curr = curr->next;
	}
	//Did not find it:
	return -1;
}

static void rave_ini_regions(void){
	track_regions.first_region = NULL;
	track_regions.last_region = NULL;
	track_regions.nesting = -1;
	track_regions.total_regions = 0;

	first_unique_region = NULL;
	last_unique_region = NULL;
}

//TODO: This is slow. Add an associative cache!
static region_node_t* dfs_find_recursive(region_node_t* curr, const char * name){
	if (curr==NULL) return NULL;
	if (strcmp(curr -> region.name,name)==0) return curr;
//	printf(" ...no..\n");
	region_node_t* child = curr->first_child;
	while(child!=NULL){
	//	printf("   search its childs...\n");
		region_node_t* found = dfs_find_recursive(child,name);
		if (found!=NULL) return found;
		//printf("   It was not any of its childs\n");
		child = child->next_sibling; 
	}
	return NULL;
}

//TODO: End child regions too?
static void rave_end_region(int cpu_index, const char * name, rave_counters * current_counters, int accumulate){

	//Find the open region it's closing (if there's no open region, do nothing)
	//Backtrack parents to find region that it's being close (it cannot be a sibling, since all siblings have its childs completed already)
	//We can assert that given a name, only up to one region can be open with that name
	//We can also assert that if accumulate is set to one, only one region with that name will exist
	region_node_t * curr = track_regions.last_region; 
	while (curr != NULL){
		if (strcmp(curr -> region.name,name)==0 && curr->region.closed==0){// && cpu_index == curr->region.opened_by){ 
			update_counters(&curr->region.delta_counters, current_counters);
			curr->region.closed = 1;
			curr->region.executions++; 
			--track_regions.nesting;
			++track_regions.total_regions;
			if (accumulate){
				//avg_counters(&curr->region.acc_counters, &curr->region.delta_counters, curr->region.executions);
				add_counters(&curr->region.acc_counters, &curr->region.delta_counters);
			}
			track_regions.last_region = curr->parent;
			return; //Stop searching after first match.
		}
		//curr = curr->prev;
		curr = curr->parent;
	}
}


static void rave_begin_region(int cpu_index, const char * name, rave_counters * current_counters, int accumulate){

	//If region with this name was already open, close it
	rave_end_region(cpu_index, name, current_counters, accumulate);

	++track_regions.nesting;
	if (track_regions.max_nested < track_regions.nesting) track_regions.max_nested = track_regions.nesting;

#if 1
	//If I accumulate, check if the region already exists:
	if (accumulate){
		region_node_t * curr = dfs_find_recursive(track_regions.first_region, name);
		if (curr != NULL){
				update_counters(&curr->region.delta_counters, current_counters);
				curr -> region.delta_counters = *current_counters;
				curr -> region.closed = 0;
				track_regions.last_region = curr;
				return;
			}
	}
#endif

	//New region:
	region_node_t * new_node = (region_node_t*) malloc(sizeof(region_node_t));
	new_node -> parent = track_regions.last_region;

	//Become the last sibling of the parent's last child:
	if (new_node -> parent != NULL){
		if (new_node -> parent -> last_child != NULL){
			new_node -> parent -> last_child -> next_sibling = new_node;
		}else{
			new_node -> parent -> first_child = new_node;
		}
		new_node -> parent -> last_child  = new_node;
	}else{
		track_regions.first_region = new_node;
	}
	track_regions.last_region = new_node;

	//New region family:
	new_node -> first_child = NULL;
	new_node -> last_child = NULL;
	new_node -> next_sibling = NULL;


	//Initialize region:
	new_node -> region.closed = 0;
	new_node -> region.executions = 0;
	new_node -> region.nesting = track_regions.nesting;
	my_strcpy(new_node -> region.name, name);
	int len = strlen(name);
	if (len > track_regions.max_name) track_regions.max_name = len;

	reset_counters(&new_node->region.acc_counters);
	new_node -> region.delta_counters = *current_counters;
	new_node -> region.opened_by = cpu_index;

	//Add it to unique list of names:
	if (name_to_id(name)==-1){
		region_unique_list_t * new_unique = malloc(sizeof(region_unique_list_t));
		new_unique -> region = &(new_node -> region);
		new_unique -> next = NULL;
		if (last_unique_region != NULL){
			new_unique -> region_id = last_unique_region -> region_id + 1; 
			last_unique_region -> next = new_unique;
		}else{
			new_unique -> region_id = 0;
			first_unique_region = new_unique;
		}
		last_unique_region = new_unique;
	}
}

static void dfs_free_recursive(region_node_t* curr){
	if (curr==NULL) return;
	region_node_t* child = curr->first_child;
	while(child!=NULL){
		dfs_free_recursive(child);
		region_node_t* next = child->next_sibling;
		free(child);
		child = next; 
	}
}


static void free_regions(void){
	//Traverse tree
	dfs_free_recursive(track_regions.first_region);

	region_unique_list_t * uniq = first_unique_region;
	while (uniq != NULL){
		region_unique_list_t * tmp = uniq->next;
		free(uniq);
		uniq = tmp;
	}
}


extern int mpi_rank;
static void dfs_report_recursive(region_node_t* curr, FILE * fd, int accumulate, int * nregion){
	if (curr==NULL) return;


		if (curr->region.closed){
			//Control nesting of output:
			ic.spaces = 4;
			//last = 1 if it has no more siblings
			int last=0;
			if (curr->next_sibling == NULL) last = 1;

			indent(fd,curr->region.nesting, last);

			//Print header
			//rave_counters * c = accumulate ? &curr->region.acc_counters : &curr->region.delta_counters;
			//double weight = 100.0*get_tot_instr(c)/get_tot_instr(&track_regions.first_region->region.delta_counters);

			if (!PLAIN_TEXT) fprintf(fd, BOLD_WHITE);	

			fprintf(fd,"Region #%d: ", *nregion);
			P_NAME(fd,"%s",curr->region.name);
			fprintf(fd," [Nesting: %d] (Rank: %d, Thread: %d)", curr->region.nesting, mpi_rank,curr->region.opened_by);
			if (accumulate){
			 	fprintf(fd,". Executed %d times%s", curr->region.executions,curr->region.executions>1?" (counters are averaged)":"");
				mul_counters(&curr->region.delta_counters, &curr->region.acc_counters, 1.0/curr->region.executions);
			}
			fprintf(fd,"\n");

			if (!PLAIN_TEXT) fprintf(fd, CLEAR_FORMAT);	

			//Print counters:
			last=0;
			if (curr->first_child == NULL) last = 1;
			indent(fd,curr->region.nesting+1, last); P_COUNTERS(fd, "%s\n", "Counters:");
			ic.spaces = 4;

			print_counters_human(fd, &curr->region.delta_counters);
			*nregion = *nregion+1;
		}


	region_node_t* child = curr->first_child;
	if (child==NULL) return;
	while(child!=NULL){
		dfs_report_recursive(child, fd, accumulate, nregion);
		child = child->next_sibling;
	}
}


//extern int mpi_rank;
static void print_region_report(FILE * fd, int accumulate){
	fprintf(fd,"-------------------" " REPORT " "-------------------" "\n"); 
	int nregion=0;
	reset_indent();
	dfs_report_recursive(track_regions.first_region, fd, accumulate, &nregion);
	fprintf(fd,"------------------------------------------------\n");
	fflush(fd);
}

static void dfs_csv_recursive(region_node_t* curr, FILE * fd, int accumulate, int * nregion){
	if (curr==NULL) return;
		if (curr->region.closed){
			fprintf(fd,"%d,%d,%d,%s,%d,%d",mpi_rank, curr->region.opened_by, *nregion, curr->region.name, curr->region.nesting, curr->region.executions);
			print_counters_csv(fd, accumulate ? &curr->region.acc_counters : &curr->region.delta_counters);
			*nregion = *nregion+1;
		}

	region_node_t* child = curr->first_child;
	if (child==NULL) return;
	while(child!=NULL){
		dfs_csv_recursive(child, fd, accumulate, nregion);
		child = child->next_sibling;
	}
}
static void print_region_csv(FILE * fd, int accumulate){
	fprintf(fd,"process_id,thread_id,region,name,nesting,executions,");
	print_csv_header(fd);
	int nregion=0;
	dfs_csv_recursive(track_regions.first_region, fd, accumulate, &nregion);
	fflush(fd);
}

