/*********************************************************/
// Copyright (C) 2025 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "state.h"
#include "profiling.h"
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <formatting.h>

void reset_profile(profile_t * loop_profile){
	loop_profile -> first_loop_node = NULL;
	loop_profile -> last_loop_node = NULL;
	loop_profile -> curr_loop_PC = -1;
	loop_profile -> jump_PC = -1;
	loop_profile -> loop_its = 0;
	loop_profile -> loop_instr = 0;
	loop_profile -> loop_vinstr = 0;
	loop_profile -> loop_weight = 0;
	for(int i=0; i<NUM_VREGS; ++i) loop_profile -> used_vreg[i]=0;
	
	for(int i=0; i<NHASHES; ++i){
		loop_profile -> PC_hash_map[i].occupancy=0; 
		loop_profile -> PC_hash_map[i].PC = NULL;
		loop_profile -> PC_hash_map[i].position = NULL;
	}
	
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
	double avg_vinstr = loop_profile -> loop_its==0 ? 0 : (double)(loop_profile -> loop_vinstr) / (loop_profile -> loop_its); 
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
		node -> tot_instr = avg_instr;
		node -> tot_vinstr = avg_vinstr;
		node -> tot_its = loop_profile->loop_its;
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
		node -> tot_instr += avg_instr;
		node -> tot_vinstr += avg_vinstr;
		node -> tot_its += loop_profile->loop_its;
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

static int read_len(char **p) {
    int n = 0;
    while (**p >= '0' && **p <= '9') { n = n * 10 + (**p - '0'); (*p)++; }
    return n;
}

/* s must be a NUL-terminated, writable buffer. Modified in place. */
void demangle_cpp(char *mangled, char *demangled, int maxsize) {
		if (mangled==NULL) return;
    if (strncmp(mangled, "_Z", 2) != 0) { //No mangled
    	memcpy(demangled, mangled, strlen(mangled)); 
			return;
		}
    char *p = mangled + 2;
    if (*p == 'L') p++;                       /* local-linkage marker */
    char *name = NULL;   /* points inside s */
    int   nlen = 0;
    if (*p == 'N') {
        p++;                                  /* enter nested-name */
        while (*p && *p != 'E') {
            if ((*p == 'C' || *p == 'D') && p[1] >= '0' && p[1] <= '9') {
                p += 2;                        /* ctor/dtor: keep prior name */
                continue;
            }
            char *lenp = p;
            int len = read_len(&lenp);
            if (len <= 0) break;               /* unparsed construct (template, etc.) */
            name = lenp;
            nlen = len;
            p = lenp + len;
        }
    } else {
        char *lenp = p;
        int len = read_len(&lenp);
        if (len > 0) {
            name = lenp;
            nlen = len;
        }
    }

    if (name) {
				if (nlen >= maxsize) nlen=maxsize-1;
        memcpy(demangled, name, nlen); 
        demangled[nlen] = '\0';
    }
}



static const Dwfl_Callbacks dwfl_callbacks = {
    .find_elf = dwfl_linux_proc_find_elf,
    .find_debuginfo = dwfl_standard_find_debuginfo,
    .section_address = dwfl_offline_section_address,
};

static Dwfl *dwfl = NULL;
extern uint64_t base /*= -1*/;

void init_dwfl(const char *binary_path) {
		if (dwfl) return; //Don't double-initialize...
		
    // Initialize dwfl with callbacks that support dynamic libraries
    dwfl = dwfl_begin(&dwfl_callbacks);
    if (!dwfl) {
        fprintf(stderr, "Profile: dwfl_begin failed: %s\n", dwfl_errmsg(-1));
        return;
    }

    // Report the main binary
    int fd = open(binary_path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Profile: Failed to open %s: %s\n", binary_path,"a");
        dwfl_end(dwfl);
        dwfl = NULL;
        return;
    }

    Dwfl_Module *mod = dwfl_report_elf(dwfl, binary_path, binary_path, fd, 0, true);
    if (!mod) {
        fprintf(stderr, "Profile: dwfl_report_elf failed for %s: %s\n", 
                binary_path, dwfl_errmsg(-1));
        close(fd);
        dwfl_end(dwfl);
        dwfl = NULL;
        return;
    }
    close(fd);

    // Now report all loaded dynamic libraries for the current process
    pid_t pid = getpid();
    if (dwfl_linux_proc_report(dwfl, pid) != 0) {
        fprintf(stderr, "Profile: dwfl_linux_proc_report failed: %s\n", 
                dwfl_errmsg(-1));
        dwfl_end(dwfl);
        dwfl = NULL;
        return;
    }

    // Finalize the reporting
    if (dwfl_report_end(dwfl, NULL, NULL) != 0) {
        fprintf(stderr, "Profile: dwfl_report_end failed: %s\n", 
                dwfl_errmsg(-1));
        dwfl_end(dwfl);
        dwfl = NULL;
        return;
    }

    // Calculate the base address for relocation
    base = find_binary_base();
    
}

// Helper to find the base address of the main binary
Dwarf_Addr find_binary_base(void) {
    // Read /proc/self/maps to find the base address of the main executable
    FILE *maps = fopen("/proc/self/maps", "r");
    if (!maps) {
        fprintf(stderr, "Failed to open /proc/self/maps\n");
        return 0;
    }

    char line[512];
    Dwarf_Addr base_addr = 0;
    
    while (fgets(line, sizeof(line), maps)) {
        Dwarf_Addr start, end;
        char perms[5], path[256] = {0};
        
        // Parse the maps line
        if (sscanf(line, "%lx-%lx %4s %*s %*s %*s %255s", 
                   &start, &end, perms, path) >= 3) {
            // Look for the main executable (usually has execute permission and is first)
            if (strstr(perms, "x") && path[0] == '/' && !strstr(path, ".so")) {
                base_addr = start;
                break;
            }
        }
    }
    
    fclose(maps);
    return base_addr;
}

// Debug function to verify all modules are properly loaded
/*
static void print_all_modules(Dwfl *dwfl) {
    if (!dwfl) return;
    
    fprintf(stderr, "Loaded modules:\n");
    
    // Iterate through all modules
    int mod_index = 0;
    Dwfl_Module *mod = NULL;
    while ((mod = dwfl_getmodules(dwfl, mod_index++)) != NULL) {
        const char *name = dwfl_module_info(mod, NULL, NULL, NULL, 
                                           NULL, NULL, NULL, NULL);
        Dwarf_Addr low_addr, high_addr;
        dwfl_module_info(mod, NULL, &low_addr, &high_addr, 
                        NULL, NULL, NULL, NULL);
        
        fprintf(stderr, "  [%d] %s (0x%lx - 0x%lx)\n", 
                mod_index - 1, name ? name : "<unknown>", 
                low_addr, high_addr);
    }
}
*/

// Improved resolve function that handles dynamic libraries properly
int resolve_pc_to_source(Dwarf_Addr pc, const char **symbol, 
                         const char **filename, int *line, int *column) {
    if (!dwfl) {
        fprintf(stderr, "dwfl not initialized\n");
        return -1;
    }

    // Adjust PC if we're dealing with offset-based addresses
    Dwarf_Addr adjusted_pc = pc;
    if (base > 0) {
        // If pc is an offset, add the base address
        adjusted_pc = pc + base;
    }

    // Get the module containing this address
    Dwfl_Module *mod = dwfl_addrmodule(dwfl, adjusted_pc);
    if (!mod) {
//        fprintf(stderr, "dwfl_addrmodule failed for pc=%#lx (adjusted=%#lx): %s\n", pc, adjusted_pc, dwfl_errmsg(-1));
        return -1;
    }

    // Try to get detailed symbol information first
    if (symbol) {
        // Use dwfl_module_addrinfo for better symbol resolution,
        // including for dynamic libraries
        GElf_Sym sym;
        GElf_Off off;
        const char *sym_name = dwfl_module_addrinfo(mod, adjusted_pc, &off, &sym,
                                                    NULL, NULL, NULL);
        if (sym_name) {
            // Check if we need to demangle C++ symbols
            // You might want to add demangling here if needed
            *symbol = sym_name;
        } else {
            // Fallback to dwfl_module_addrname
            *symbol = dwfl_module_addrname(mod, adjusted_pc);
						/*
            if (!*symbol) {
                *symbol = "??";
            }
						*/
        }
    }

    // Get source line information
    Dwfl_Line *dwfl_line = dwfl_module_getsrc(mod, adjusted_pc);
    if (!dwfl_line) {
 //       fprintf(stderr, "dwfl_module_getsrc failed for pc=%#lx: %s\n", pc, dwfl_errmsg(-1));
        return -1;
    }

    // Extract line and file information
    Dwarf_Addr addr;
    const char *file_str = dwfl_lineinfo(dwfl_line, &addr, line, column, 
                                        NULL, NULL);
    if (!file_str) {
//        fprintf(stderr, "dwfl_lineinfo failed: %s\n", dwfl_errmsg(-1));
        return -1;
    }

    // Extract just the filename from the full path
    if (filename) {
        const char *last_slash = strrchr(file_str, '/');
        if (last_slash) {
            *filename = last_slash + 1;
        } else {
            *filename = file_str;
        }
    }

    return 0;
}

// Cleanup function
void cleanup_dwfl(void) {
    if (dwfl) {
        dwfl_end(dwfl);
        dwfl = NULL;
    }
}

void print_loop_profile(FILE * fd, profile_t * loop_profile){
//	P_COUNTERS(fd,"%s","Elems" "\t" "avg_instr" "\t" "avg_its" "\t" "avg_vreg_use" "\t" "Instances" /*"\t" "PC"*/ "\t" "Funct" "\t" "file:line" "\n");

	if (PROFILE_WEIGHT == w_ELEMS){ P_NUMBER5(fd,"%s","Elems\t");}
	else{ P_NUMBER5(fd,"%s","Instr\t");}
	P_NUMBER(fd,"%s","Instances\t");
	P_NUMBER2(fd,"%s","Iters(avg)\t");
	P_NUMBER(fd,"%s","it_Instr(avg)\t");
	P_NUMBER3(fd,"%s","vmix\t");
	P_NUMBER(fd,"%s","vreg_use(avg)\t");
	P_NAME(fd,"%s","Function\t");
	fprintf(fd,"%s","file:line\n");
	uint64_t totweight=0.0;
	const double cutoff=0.0075;
		loop_node * node = loop_profile->first_loop_node; 
		while(node != NULL){
			uint64_t PC = node->PC-base;
			const char * pc_file=NULL;
			const char * pc_symbol=NULL;
			int pc_line=-1;
			int pc_column=-1;
			/*int ret =*/ resolve_pc_to_source(PC, &pc_symbol, &pc_file, &pc_line, &pc_column);
			char demangled[128]="Unknown";
			demangle_cpp((char*)pc_symbol, demangled, 128);
			double avg_its = node->tot_its / node->freq;
			double avg_instr =  node->tot_instr / node->freq;
			double vmix = node->tot_vinstr / node->tot_instr;
			double avg_usage = node->register_usage / node->freq;
			totweight += node->weight;
			if ((double)node->weight / totweight < cutoff) break;
			P_NUMBER5(fd, "%.0f\t", node->weight);
			P_NUMBER(fd, "%ld\t",  node->freq);
			P_NUMBER2(fd, "%.1f\t", avg_its);
			P_NUMBER(fd, "%.1f\t", avg_instr);
			P_NUMBER3(fd, "%.2f\t", vmix);
			P_NUMBER(fd, "%.2f\t", avg_usage);
			P_NAME(fd,"%s\t",demangled);
			fprintf(fd, "%s:%d\n", pc_file!=NULL?pc_file:"Unknown", pc_line);
			//fprintf(fd,"%.0f" "\t" "%.1f" "\t" "%.1f" "\t" "%.3f" "\t" "%ld" "\t" "\t" /*"0x%lx" "\t"*/,node->weight, avg_instr, avg_its, avg_usage, node->freq/*, PC*/);
			//fprintf(fd, "%s" "\t" "%s:%d\n", demangled, pc_file!=NULL?pc_file:"Unknown", pc_line); 
			node = node->next;
		}
	fflush(fd);
}

calltrace_node_t * reset_calltrace_node(){
	calltrace_node_t * ctn = (calltrace_node_t *)malloc(sizeof(calltrace_node_t));
	ctn->fill = 0;  
	ctn->next = NULL;
	return ctn;
}

void reset_calltrace(calltrace_t * ct){
	ct->next_is_func = 0;
	ct->n_nodes = 1;
	ct->first_node = reset_calltrace_node();
	ct->last_node = ct->first_node;
}
void add_to_calltrace(calltrace_t * ct, uint64_t PC){
	calltrace_node_t * curr = ct->last_node; 
	if (curr->fill == N_PCS_NODE){ //Full node, allocate a new one
		calltrace_node_t * ctn = reset_calltrace_node();
		curr->next = ctn;
		ct->n_nodes++;
		curr = ctn;
		ct->last_node = curr;
	}
	//Add new PC
	curr->PCs[curr->fill] = PC;
	curr->fill++;
}

void print_call_trace(FILE * fd, calltrace_t * ct){
	calltrace_node_t * curr = ct->first_node;
	int unknowns = 0;

	char * enabler = getenv("RAVE_FUNC_ENABLER");
	long PC_comp = 0;
	char * enabler_PC = getenv("RAVE_PC_ENABLER");
	if (enabler_PC != NULL){
		char * endptr;
		PC_comp = strtol(enabler_PC, &endptr, 16);
	}

	int enabled = (enabler_PC == NULL) && (enabler == NULL);

	printf("%ld traced calls (Approx)\n", (long)ct->n_nodes*(long)N_PCS_NODE);
	while (curr != NULL){
		for(int i=0; i<curr->fill; ++i){
			uint64_t PC = curr->PCs[i]-base;
			const char * pc_file=NULL;
			const char * pc_symbol=NULL;
			int pc_line=-1;
			int pc_column=-1;
			/*int ret =*/ resolve_pc_to_source(PC, &pc_symbol, &pc_file, &pc_line, &pc_column); //I should cache this...
			if (pc_symbol != NULL){
				if (!enabled && enabler != NULL && strcmp(pc_symbol, enabler)==0){
					enabled=1;
				}else if (!enabled && PC_comp != 0 && PC==PC_comp){
					enabled=1;
				}

				if(enabled){
					if(unknowns != 0){
						fprintf(fd,"Jumped through %d Unknown symbols\n", unknowns);
					}
					fprintf(fd, "%lx" "\t" "%s" "\t" "%s:%d\n", PC, pc_symbol!=NULL?pc_symbol:"Unknown", pc_file!=NULL?pc_file:"Unknown", pc_line); 
					}
					unknowns=0;
				}else{
					++unknowns;
				}
		}
		curr = curr->next;
	}
}
