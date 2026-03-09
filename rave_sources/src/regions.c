/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "regions.h"
#include "formatting.h"
#include "utils.h"

#include <string.h>
#include <stdlib.h>

track_regions_t track_regions;
region_unique_list_t *first_unique_region;
region_unique_list_t *last_unique_region; 

#include "threading.h" //for mpi_rank

int name_to_id(const char * name){
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

void rave_ini_regions(void){
	track_regions.first_region = NULL;
	track_regions.nesting = -1;
	track_regions.total_regions = 0;

	first_unique_region = NULL;
	last_unique_region = NULL;

	track_regions.stack_top = NULL;
}

//TODO: This is slow. Add an associative cache!
region_node_t* dfs_find_recursive(region_node_t* curr, const char * name){
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

void rave_end_region(int cpu_index, const char * name, rave_counters * current_counters, int accumulate, FILE * fd){


	//Find the open region it's closing (if there's no open region, do nothing)
	//Backtrack parents to find region that it's being close (it cannot be a sibling, since all siblings have its childs completed already)
	//We can assert that given a name, only up to one region can be open with that name
	//We can also assert that if accumulate is set to one, only one region with that name will exist
	
	if (track_regions.stack_top != NULL && 
			track_regions.stack_top->region_node != NULL /*&& 
			(name[0]=='\0' || strcmp(track_regions.stack_top -> region_node->region.name,name)==0)*/){

		update_counters(&track_regions.stack_top->region_node->region.delta_counters, current_counters);
		track_regions.stack_top->region_node->region.closed = 1;
		//track_regions.stack_top->region_node->region.enabled |= enabled;
		track_regions.stack_top->region_node->region.executions++; 
		--track_regions.nesting;
		++track_regions.total_regions;
		if (accumulate){
			//avg_counters(&curr->region.acc_counters, &curr->region.delta_counters, curr->region.executions);
			add_counters(&(track_regions.stack_top->region_node->region.acc_counters), &(track_regions.stack_top->region_node->region.delta_counters));
		}

		if (fd!=NULL){ //Streaming mode
			print_region_human(fd, track_regions.total_regions-1, &track_regions.stack_top->region_node->region, 0, 0, 1, 1);
		}

		region_stack_t * prev = track_regions.stack_top->prev;
		free(track_regions.stack_top);
		track_regions.stack_top = prev;
	}else{
		printf("Trying to close region %s, but last region is %s\n",name, track_regions.stack_top != NULL? track_regions.stack_top -> region_node -> region.name:"NULL");
		exit(-1);
	}
}


void rave_begin_region(int cpu_index, const char * name, rave_counters * current_counters, int accumulate/*, int enabled*/){

	++track_regions.nesting;
	if (track_regions.max_nested < track_regions.nesting) track_regions.max_nested = track_regions.nesting;

	region_stack_t * new_top = (region_stack_t *)malloc(sizeof(region_stack_t));
	new_top -> prev = track_regions.stack_top;
	track_regions.stack_top = new_top;

#if 1
	//If I accumulate, check if the region already exists:
	if (accumulate){
		region_node_t * curr = dfs_find_recursive(track_regions.first_region, name);
		if (curr != NULL){
				update_counters(&curr->region.delta_counters, current_counters);
				curr -> region.delta_counters = *current_counters;
				curr -> region.closed = 0;
				//curr -> region.enabled |= enabled;
				track_regions.stack_top -> region_node = curr; 
				track_regions.stack_top = new_top;
				return;
			}
	}
#endif

	//New region:
	region_node_t * new_node = (region_node_t*) malloc(sizeof(region_node_t));
	new_top -> region_node = new_node;


	//Become the last sibling of the parent's last child:
	if (new_top -> prev!= NULL){
		if (new_top -> prev -> region_node -> last_child != NULL){
			new_top -> prev -> region_node -> last_child -> next_sibling = new_node;
		}else{
			new_top -> prev -> region_node -> first_child = new_node;
		}
		new_top -> prev -> region_node -> last_child  = new_node;
	}else{
		track_regions.first_region = new_node;
	}


	if(track_regions.first_region==NULL){
		track_regions.first_region = new_node;
	}


	//New region family:
	new_node -> first_child = NULL;
	new_node -> last_child = NULL;
	new_node -> next_sibling = NULL;


	//Initialize region:
	//new_node -> region.enabled |= enabled;
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

void dfs_free_recursive(region_node_t* curr){
	if (curr==NULL) return;
	region_node_t* child = curr->first_child;
	while(child!=NULL){
		dfs_free_recursive(child);
		region_node_t* next = child->next_sibling;
		free(child);
		child = next; 
	}
}


void free_regions(void){
	//Traverse tree
	dfs_free_recursive(track_regions.first_region);

	region_unique_list_t * uniq = first_unique_region;
	while (uniq != NULL){
		region_unique_list_t * tmp = uniq->next;
		free(uniq);
		uniq = tmp;
	}
}

void print_region_human(FILE * fd, int n, region_t * region, int accumulate, int indent_region, int last_child, int no_childs){
			//Control nesting of output:
			ic.spaces = 4;

			//If we accumulate, we don't indent
			if (indent_region){
				indent(fd,region->nesting, last_child);
			}

			//Print header
			if (!PLAIN_TEXT) fprintf(fd, BOLD_WHITE);	

			fprintf(fd,"Region #%d: ", n);
			P_NAME(fd,"%s",region->name);
			if (!accumulate) fprintf(fd," [Nesting: %d]",region->nesting);
			fprintf(fd," (Rank: %d, Thread: %d)", mpi_rank,region->opened_by);
			if (accumulate){
			 	fprintf(fd,". Executed %d times%s", region->executions,region->executions>1?" (counters are averaged)":"");
				mul_counters(&region->delta_counters, &region->acc_counters, 1.0/region->executions);
			}
			fprintf(fd,"\n");

			if (!PLAIN_TEXT) fprintf(fd, CLEAR_FORMAT);	

			//Print counters:
			indent(fd, accumulate ? 1 : region->nesting+1, accumulate || no_childs); P_COUNTERS(fd, "%s\n", "Counters:");
			ic.spaces = 4;

			print_counters_human(fd, &region->delta_counters);
}

void dfs_report_recursive(region_node_t* curr, FILE * fd, int accumulate, int * nregion){
	if (curr==NULL) return;

		if (curr->region.closed/* && curr->region.enabled*/){
			//last = 1 if it has no more siblings
			//If we accumulate, we don't indent, so all childs are single-childs
//			int last = (accumulate || curr->next_sibling == NULL || curr->first_child == NULL);
			int last_child = curr->next_sibling == NULL;
			int no_childs = curr->first_child == NULL;
			print_region_human(fd, *nregion, &curr->region, accumulate, !accumulate, last_child, no_childs); 
			*nregion = *nregion+1;
		}

	region_node_t* child = curr->first_child;
	if (child==NULL) return;
	while(child!=NULL){
		dfs_report_recursive(child, fd, accumulate, nregion);
		child = child->next_sibling;
	}
}


void print_region_report(FILE * fd, int accumulate){
	fprintf(fd,"-------------------" " REPORT " "-------------------" "\n"); 
	int nregion=0;
	reset_indent();
	dfs_report_recursive(track_regions.first_region, fd, accumulate, &nregion);
	fprintf(fd,"------------------------------------------------\n");
	fflush(fd);
}

void dfs_csv_recursive(region_node_t* curr, FILE * fd, int accumulate, int * nregion){
	if (curr==NULL) return;
		if (curr->region.closed/* && curr->region.enabled*/){
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
void print_region_csv(FILE * fd, int accumulate){
	fprintf(fd,"process_id,thread_id,region,name,nesting,executions,");
	print_csv_header(fd);
	int nregion=0;
	dfs_csv_recursive(track_regions.first_region, fd, accumulate, &nregion);
	fflush(fd);
}

