/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "profiling.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void reset_profile(profile_t * loop_profile){
	loop_profile -> first_loop_node = NULL;
	loop_profile -> last_loop_node = NULL;
	loop_profile -> curr_loop_PC = -1;
	loop_profile -> jump_PC = -1;
	loop_profile -> loop_its = 0;
	loop_profile -> loop_instr = 0;
	loop_profile -> loop_weight = 0;
	for(int i=0; i<NUM_VREGS; ++i) loop_profile -> used_vreg[i]=0;
	
	for(int i=0; i<NHASHES; ++i){
		loop_profile -> PC_hash_map[i].occupancy=0; 
		loop_profile -> PC_hash_map[i].PC = NULL;
		loop_profile -> PC_hash_map[i].position = NULL;
	}
	
}

Dwfl *dwfl;

const Dwfl_Callbacks dwfl_callbacks = {
    .find_elf = dwfl_build_id_find_elf,
    .find_debuginfo = dwfl_standard_find_debuginfo,
};


uint64_t find_binary_base(void) {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) {
        perror("fopen /proc/self/maps");
        return 0;
    }
    char line[512];
    uint64_t base_addr = 0;
    char * ret = fgets(line, sizeof(line), fp);
    sscanf(ret, "%" SCNx64 "-", &base_addr);
    fclose(fp);
    return base_addr;
}

/*
#define max_sample_freq 512
#define log_sample 9
#define sample_variability 67
int sample_countdown = max_sample_freq;
*/
/////// HASH MAP for PCs ///////////
uint32_t index_hash(int64_t key, uint32_t table_size) {
    uint64_t z = (uint64_t)key;
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9U;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebU;
    z = z ^ (z >> 31);
    return (uint32_t)(z % table_size);
}
void update_position_hash(profile_t * loop_profile, uint64_t PC, loop_node * position){
	uint32_t hash = index_hash(PC,NHASHES);
	PC_hash_map_node * node = &(loop_profile -> PC_hash_map[hash]);
	//printf("Updating PC %lx on %u (%d)\n",PC,hash,node->occupancy);
 	for(int i=0; i<node->occupancy; ++i){
		if (node->PC[i] == PC){
		 	node->position[i] = position;
			return;
		}	
	}
	printf("Error in PROFILER hashmap: Updating Position of non-existant PC\n");
	exit(-1);
}

loop_node * find_PC_hash(profile_t * loop_profile, uint64_t PC){
	uint32_t hash = index_hash(PC,NHASHES);
	PC_hash_map_node * node = &(loop_profile -> PC_hash_map[hash]);
 	for(int i=0; i<node->occupancy; ++i){
		if (node->PC[i] == PC){
			if (node->position[i] == NULL){
				printf("Error in PROFILER hashmap: NULL pre-existing PC\n");
				exit(-1);
			}
			//printf("Found node with PC %lx\n",PC);
			return node->position[i];
		}
	}
	//Not found, increase occupancy 
	node->occupancy++;
	uint64_t * new_PCs = (uint64_t*)malloc(sizeof(uint64_t)*node->occupancy);
	new_PCs[node->occupancy-1] = PC;
	//printf("Introduced node with PC %lx on %u (%d)\n",PC,hash,node->occupancy);
	loop_node ** new_positions = (loop_node **)malloc(sizeof(loop_node *)*node->occupancy);
	//Copy-back
	if (node->PC != NULL && node->position != NULL){
		for(int i=0; i<node->occupancy-1; ++i){
			new_PCs[i] = node->PC[i];
			new_positions[i] = node->position[i];
		}
		free(node->PC);
		free(node->position);
	}
	//Put new
	node->PC = new_PCs;
	node->position = new_positions;
	return NULL;
}
///////////////////////////


void update_PC(profile_t * loop_profile){
 
	uint64_t PC = loop_profile -> curr_loop_PC;
 	uint64_t weight = loop_profile -> loop_weight;
	double avg_instr = loop_profile -> loop_its==0 ? 0 : (double)(loop_profile -> loop_instr) / (loop_profile -> loop_its); 
	int reg_count=0;
	for(int i=0; i<NUM_VREGS; ++i) reg_count += (int)loop_profile->used_vreg[i];
	//loop_node * prev = loop_profile->last_loop_node; 

	//TODO: Save "most likely" place!

	//Look for node
#if 1
	loop_node * node = find_PC_hash(loop_profile, PC);
#else
	loop_node * node = loop_profile->first_loop_node; 
	while(node != NULL){
		if (node -> PC == PC) break;
		node = node->next;
	}
#endif
	//Initialize node if not found
	if (node == NULL){
		node = (loop_node *)malloc(sizeof(loop_node));
		update_position_hash(loop_profile, PC, node);	
		node -> PC = PC;
		node -> freq = 1;
		node -> weight = (double)weight;
		node -> avg_instr = avg_instr;
		node -> register_usage = (double)reg_count / NUM_VREGS;
		node -> next = NULL;
		node -> prev = loop_profile->last_loop_node;
		if (loop_profile->last_loop_node == NULL){ //no prev: I'm first
			loop_profile->first_loop_node = node;
		}else{
			loop_profile->last_loop_node->next = node;
		}
		loop_profile->last_loop_node = node;
	//Update node otherwise
	}else{
		node -> freq += 1;
		//node -> weight = node->weight * (double)(node->freq-1)/(node->freq) + (double)weight/node->freq;
		node -> weight += (double)weight; 
		node -> avg_instr += avg_instr;
		node -> register_usage += (double)reg_count / NUM_VREGS;
	}

	//Speed this for first-time nodes...
	//Relocation backwards
	while(node->prev != NULL && node->prev->weight < node->weight){
		loop_node * prev = node->prev;
		loop_node * prevprev = node->prev->prev;
		loop_node * next = node->next;
		//Move prev forwards
		prev->next = next;
		if(next != NULL) next->prev = prev;
		//Link with prev
		node->next = prev;
		prev->prev = node;
		//Link prevprev with myself
		if (prevprev != NULL) prevprev->next = node;
		node->prev = prevprev;
		//Fix head of linked list
		if (prev == loop_profile->first_loop_node) loop_profile->first_loop_node = node;
		//Fix tail of linked list
		if (node == loop_profile->last_loop_node) loop_profile->last_loop_node = prev;
	}
	//Relocation forwards
	while(node->next != NULL && node->next->weight > node->weight){
		loop_node * prev = node->prev;
		loop_node * next = node->next;
		loop_node * nextnext = node->next->next;

		//Move next backwards
		if (prev!=NULL) prev->next = next;
		next->prev = prev;

		//Link next with myself
		next->next = node;
		node->prev = next;

		//Link nexnex with myself
		node->next = nextnext;
		if(nextnext != NULL) nextnext->prev = node;

		//Fix head of hash
		if (node == loop_profile->first_loop_node) loop_profile->first_loop_node = next;
		//Fix tail of linked list
		if (next == loop_profile->last_loop_node) loop_profile->last_loop_node = node;
	}

}


/*
void sample(uint64_t PC){
	if (sample_countdown-- == 0){
		PC -= base;
		uint64_t quantize_PC = PC >> log_sample; 
		update_PC(quantize_PC);
		sample_countdown = max_sample_freq - rand()%(sample_variability); 
	}
}
*/

extern uint64_t base /*= -1*/;
int get_first_module_base(Dwfl_Module *mod, void **userdata,
                          const char *name, Dwarf_Addr _base, void *arg) {
				base = _base;
        return 1; 
}

void init_dwfl(const char *binary_path) {
    dwfl = dwfl_begin(&dwfl_callbacks);
    if (!dwfl) {
        fprintf(stderr, "Profile: dwfl_begin failed\n");
        return;
    }

    int fd = open(binary_path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Profile: Failed to open %s\n", binary_path);
        dwfl_end(dwfl);
        return;
    }

    Dwfl_Module *mod = dwfl_report_elf(dwfl, binary_path, binary_path, fd, 0, true);
    if (!mod) {
        fprintf(stderr, "Profile: dwfl_report_elf failed for %s\n", binary_path);
        close(fd);
        dwfl_end(dwfl);
        return;
    }

    close(fd);

    if (dwfl_report_end(dwfl,NULL,NULL) != 0) {
        fprintf(stderr, "Profile: dwfl_report_end failed\n");
        dwfl_end(dwfl);
        return;
    }

    //print_valid_ranges(dwfl);
		dwfl_getmodules(dwfl, get_first_module_base, NULL, 0);
		//Only need to substract when base is eq to 0
		if (base == 0) base = find_binary_base();
		else base = 0;
}


int resolve_pc_to_source(Dwarf_Addr pc, const char ** symbol, const char **filename, int *line, int *column) {

    Dwfl_Module *mod = dwfl_addrmodule(dwfl, pc);
		Dwfl_Line * dwfl_line;
    if (!mod) {
				dwfl_line = dwfl_getsrc(dwfl, pc);
		}else{
		 	dwfl_line = dwfl_module_getsrc(mod, pc);
			*symbol = dwfl_module_addrname(mod, pc);
		}
    if (!dwfl_line) {
        //fprintf(stderr, "dwfl_[module]_getsrc failed @%08lx: %s\n", pc, dwfl_errmsg(-1));
        return -1;
		}
		//const char *funcname = dwfl_module_addrname(mod, pc);
    Dwarf_Addr addr;
    const char *file_str = dwfl_lineinfo(dwfl_line, &addr, line, column, NULL, NULL);
    if (!*file_str) {
        //fprintf(stderr, "dwfl_lineinfo failed: %s\n", dwfl_errmsg(-1));
        return -1;
    }
		int i=strlen(file_str)-1;
		while(file_str[--i] != '/');
		*filename = &file_str[i+1];
		return 0;
}

void print_loop_profile(FILE * fd, profile_t * loop_profile){
	fprintf(fd,"Elems" "\t" "avg_instr" "\t" "avg_vreg_use" "\t" "Instances" "\t" "PC" "\t" "Funct" "\t" "file:line" "\n");
	uint64_t totweight=0.0;
	const double cutoff=0.01;
		loop_node * node = loop_profile->first_loop_node; 
		while(node != NULL){
			uint64_t PC = node->PC-base;
			const char * pc_file=NULL;
			const char * pc_symbol=NULL;
			int pc_line=-1;
			int pc_column=-1;

			/*int ret =*/ resolve_pc_to_source(PC, &pc_symbol, &pc_file, &pc_line, &pc_column);
			if (/*!ret*/ pc_symbol != NULL){
				totweight += node->weight;
				if ((double)node->weight / totweight < cutoff) break;
				double avg_instr = node->avg_instr / node->freq;
				double avg_usage = node->register_usage / node->freq;
				fprintf(fd,"%.0f" "\t" "%.1f" "\t" "%.3f" "\t" "%ld" "\t" "\t" "0x%lx" "\t",node->weight, avg_instr, avg_usage, node->freq, PC);
				fprintf(fd, "%s" "\t" "%s:%d\n", pc_symbol!=NULL?pc_symbol:"Unknown", pc_file!=NULL?pc_file:"Unknown", pc_line); 
			}
			node = node->next;
		}
	fflush(fd);
}
