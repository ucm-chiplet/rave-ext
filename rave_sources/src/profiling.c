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

PC_node * first_PC_node;
PC_node * last_PC_node;

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

void update_PC(uint64_t PC, uint64_t weight){
	PC_node * node = first_PC_node; 
	PC_node * prev = NULL;

	//TODO: Save "most likely" place!

	//Look for node
	while(node != NULL){
		if (node -> PC == PC) break;
		if (node->next == NULL){
			prev = node;
		}
		node = node->next;
	}
	//Initialize node if not found
	if (node == NULL){
		node = (PC_node *)malloc(sizeof(PC_node));
		node -> PC = PC;
		node -> freq = 1;
		node -> weight = (double)weight;
		node -> next = NULL;
		node -> prev = prev;
		if (prev==NULL){
			first_PC_node = node;
		}else{
			prev -> next = node;
		}
	//Update node otherwise
	}else{
		node -> freq += 1;
		//node -> weight = node->weight * (double)(node->freq-1)/(node->freq) + (double)weight/node->freq;
		node -> weight += (double)weight; 
	}

	//Relocation forwards
	while(node->prev != NULL && node->prev->weight < node->weight){
		PC_node * prev = node->prev;
		PC_node * prevprev = node->prev->prev;
		PC_node * next = node->next;
		//Move prev forwards
		prev->next = next;
		if(next != NULL) next->prev = prev;
		//Link with prev
		node->next = prev;
		prev->prev = node;
		//Link prevprev with myself
		if (prevprev != NULL) prevprev->next = node;
		node->prev = prevprev;
		//Fix head of hash
		if (prev == first_PC_node) first_PC_node = node;
	}
	//Relocation basckwards
	while(node->next != NULL && node->next->weight > node->weight){
		PC_node * prev = node->prev;
		PC_node * next = node->next;
		PC_node * nextnext = node->next->next;

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
		if (node == first_PC_node) first_PC_node = next;
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

void print_loop_profile(FILE * fd){
	fprintf(fd,"-------------------" " PROFILED LOOPS " "--------------------" "\n");

	fprintf(fd,"Elems" "\t" "Instances" "\t" "PC" "\t" "Funct" "\t" "file:line" "\n");
	uint64_t totweight=0.0;
	const double cutoff=0.01;
		PC_node * node = first_PC_node; 
		if (node != NULL){
			//Print all occurences:
			while(node != NULL){
				uint64_t PC = node->PC;
				const char * pc_file=NULL;
				const char * pc_symbol=NULL;
				int pc_line=-1;
				int pc_column=-1;

				/*int ret =*/ resolve_pc_to_source(PC, &pc_symbol, &pc_file, &pc_line, &pc_column);
				if (/*!ret*/ pc_symbol != NULL){
					totweight += node->weight;
					//printf("%.2f\n", (double)node->weight / totweight);
					if ((double)node->weight / totweight < cutoff) break;
					fprintf(fd,"%.0f" "\t" "%ld" "\t" "\t" "0x%lx" "\t",node->weight,node->freq,node->PC);
				 	fprintf(fd, "%s" "\t" "%s:%d\n", pc_symbol!=NULL?pc_symbol:"Unknown", pc_file!=NULL?pc_file:"Unknown", pc_line); 
				}
				node = node->next;
			}
		}

	fprintf(fd, "-------------------------------------------------------\n");
	fflush(fd);
}
