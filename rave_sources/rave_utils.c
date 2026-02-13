//include/hw/core/cpu.h (0.7 :307 (def) ||||| 1.0 : 323 (def CPUState) //Util for knowing OFFSET REGS
#define sizeof_ulong sizeof(uint64_t)

//qemu_get_cpu returns an ArchCPU, which has a CPURISCVState (typdef of CPUArchState), 
//ArchCPU is defined in target/riscv/cpu.h (l:277 for 0.7, l:444 for 1.0)
//CPUArchState is defined in target/riscv/cpu.h (l:114 for 0.7, l:161 for 1.0)
//In accel/tcg/plugin-gen.c (l:175 for 0.7, l:165 for 1.0) is a good place to put : printf("Offset is %ld\n",offsetof(ArchCPU, env));
#ifdef EPI_07
#define OFFSET_CPUState (33552) //For 0.7
#define OFFSET_REGS (sizeof_ulong*32 + sizeof(uint64_t)*32 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#else
//#define OFFSET_CPUState (832) //For 1.0
#define OFFSET_CPUState (10176) //For 1.0
#define OFFSET_REGS (sizeof_ulong*32*2 + sizeof(uint64_t)*(32*RV_VLEN_MAX/64))
#endif

#define RV_VLEN_MAX (256*64)

static int64_t qemu_get_vl(uint8_t * cpu){
	return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*2);
}
static int64_t qemu_get_vtype(uint8_t * cpu){
	return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*4);
}
static int64_t qemu_get_xreg(uint8_t * cpu, int reg){
	return *(int64_t*)(cpu + OFFSET_CPUState + sizeof_ulong*reg); 
}

void *qemu_get_cpu(int index);

static char contains_string(char * str, const char * find){
	int slen = strlen(str);
	int flen = strlen(find);
	int progress = 0;
	for(int i=0; i<slen; ++i){
		if (str[i] == find[progress]) ++progress;
		else if (str[i] == find[0]) progress=1;
		else progress = 0;
		if (progress == flen) return 1;
	}
	return 0;
}

extern int cpu_memory_rw_debug(uint8_t *cpu, uint64_t addr, uint8_t *buf, int len, int is_write);
static void rave_read_string(unsigned int cpu_index, uint32_t insn_opcode, char * string, uint64_t maxlen){
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	uint64_t string_addr = qemu_get_xreg(cpu,src1);
	uint64_t len = qemu_get_xreg(cpu,src2);

	//Read string from guest memory
	uint64_t i;
	for(i=0; i<maxlen && i<len; ++i){
		cpu_memory_rw_debug(cpu, string_addr + i, (uint8_t*)&string[i], 1, 0);
		if (string[i] == '\0') break;
	}
	string[i] = '\0';
}

