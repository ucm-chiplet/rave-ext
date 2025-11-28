uint64_t base=-1;
static uint64_t find_binary_base(void) {
    FILE *fp = fopen("/proc/self/maps", "r");
    if (!fp) {
        perror("fopen /proc/self/maps");
        return 0;
    }
    char line[512];
    uint64_t base_addr = 0;
#if 0
    while (fgets(line, sizeof(line), fp)){
			printf("Line is %s\n",line);
		}
		fflush(stdout);
#else
    char * ret = fgets(line, sizeof(line), fp);
#endif
    sscanf(ret, "%" SCNx64 "-", &base_addr);
    fclose(fp);
    return base_addr;
}

#define HASH_SIZE 1 //1024

struct PC_node{
	uint64_t PC;
	long freq;
	double weight;
	struct PC_node * next;
	struct PC_node * prev;
};
typedef struct PC_node PC_node;

//PC_node * PC_hash[HASH_SIZE] = {0};
PC_node * first_PC_node;
PC_node * last_PC_node;

/*
static int hash(uint64_t PC){
	return 0; //PC&(HASH_SIZE-1);
}
*/
/*
#define max_sample_freq 512
#define log_sample 9
#define sample_variability 67
int sample_countdown = max_sample_freq;
*/

static void update_PC(uint64_t PC, uint64_t weight){
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

#include "elfutils/libdwfl.h"
Dwfl *dwfl;

static const Dwfl_Callbacks dwfl_callbacks = {
    .find_elf = dwfl_build_id_find_elf,
    .find_debuginfo = dwfl_standard_find_debuginfo,
};

static int get_first_module_base(Dwfl_Module *mod, void **userdata,
                          const char *name, Dwarf_Addr _base, void *arg) {
				base = _base;
        return 1; 
}

static void init_dwfl(const char *binary_path) {
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


static int resolve_pc_to_source(Dwarf_Addr pc, const char ** symbol, const char **filename, int *line, int *column) {

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

#if 0
static int module_callback(Dwfl_Module *mod, void **userdata,
                           const char *name, Dwarf_Addr base, void *arg) {
    GElf_Addr bias = 0;
    Elf *elf = dwfl_module_getelf(mod, &bias);
    printf("[DWFL] Module: %s, base: 0x%" PRIx64 ", bias: 0x%" PRIx64 "\n",
           name ? name : "<unnamed>", base, bias);

    if (!elf) {
        printf("  [!] No ELF handle available\n");
        return DWARF_CB_OK;
    }
		
    size_t phnum;
    if (elf_getphdrnum(elf, &phnum) != 0) {
        printf("  [!] elf_getphnum failed\n");
        return DWARF_CB_OK;
    }

    for (size_t i = 0; i < phnum; i++) {
        GElf_Phdr phdr;
        if (!gelf_getphdr(elf, i, &phdr))
            continue;

        if (phdr.p_type == PT_LOAD) {
            printf("  LOAD segment: 0x%" PRIx64 " - 0x%" PRIx64 ", flags: 0x%x\n",
                   (uint64_t)(phdr.p_vaddr + bias),
                   (uint64_t)(phdr.p_vaddr + phdr.p_memsz + bias),
                   phdr.p_flags);
        }
    }

    return DWARF_CB_OK;
}

static void print_valid_ranges(Dwfl *dwfl) {
    ptrdiff_t offset = 0;
    while (dwfl_getmodules(dwfl, module_callback, NULL, offset) > 0) {
        offset++;
    }
}
#endif

#if 1
static void print_loop_profile(FILE * fd){
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
#else
static void dfs_profile_recursive(FILE * fd, region_node_t* curr, int accumulate, double global, double local){
	if (curr==NULL) return;
	//Order its siblings by weight
	double last_max = DBL_MAX;
	while (1){
		double max = 0;
		region_node_t* sibling = curr->parent != NULL ? curr->parent->first_child : curr; 
		region_node_t* max_sibling = NULL;
		int siblings_left = 0;
		while(sibling != NULL){
			double weight = get_tot_instr(accumulate ? &sibling->region.acc_counters : &sibling->region.delta_counters);
			if (weight < last_max){
				++siblings_left;
				if (weight > max){
				 	max = weight;
					max_sibling = sibling;
				}
			}
			sibling = sibling->next_sibling;
		}
		if (max_sibling == NULL) break;
		last_max = max;
		//Print region name
		indent(fd, max_sibling->region.nesting + 1, siblings_left==1);
		P_NAME(fd,"%s ", max_sibling->region.name); 
		//Add spaces so they are all the same size
		int len = strlen(max_sibling->region.name);
		int spaces = 3 + track_regions.max_name - len - max_sibling->region.nesting*2;
		for(int i=0; i<spaces; ++i) fprintf(fd,".");
		fprintf(fd," executions: "); P_NUMBER(fd,"%d",max_sibling->region.executions);
		fprintf(fd,", total instr: "); P_NUMBER(fd,"%.0f",max);
		fprintf(fd," ("); P_PERCENTAGE(fd, "%.2f %%", 100.0*max/global);
		fprintf(fd," of total, "); P_PERCENTAGE(fd,"%.2f %%",100.0*max/local);
		fprintf(fd," of parent)\n"); 
		region_node_t* child = max_sibling->first_child;
		dfs_profile_recursive(fd, child, accumulate, global, max);
	}
}

static void print_region_profile(FILE * fd, int accumulate){
	fprintf(fd,"-------------------" " PROFILE " "--------------------" "\n");

	reset_indent();
	ic.spaces = 1;
	double global = get_tot_instr(&track_regions.first_region->region.delta_counters);
	dfs_profile_recursive(fd, track_regions.first_region, accumulate, global, global);

	ic.prev_nest=0;

	fprintf(fd, "------------------------------------------------\n");
	fflush(fd);
}
#endif
