/*
 * Copyright (C) 2021, Alexandre Iooss <erdnaxe@crans.org>
 *
 * Log instruction execution with memory access.
 *
 * License: GNU GPL, version 2 or later.
 *   See the COPYING file in the top-level directory.
 */
#include <glib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <qemu-plugin.h>
#include <fcntl.h>


//#define TIMEDEBUG


#ifdef TIMEDEBUG
uint64_t getmicros(){
	#if 0
	struct timeval tp;
	gettimeofday(&tp,NULL);
	return tp.tv_sec *1e6 + tp.tv_usec;
	#else
	uint64_t a,d;
	asm volatile("rdtsc" : "=a" (a), "=d" (d));
	return (a | (d << 32));
	#endif
}
uint64_t time_trans=0;
uint64_t time_vcpu_exe=0;
uint64_t time_vcpu_control=0;
int num_trans=0;
int num_vcpu_exe=0;
int num_vcpu_control=0;
#endif

////////////////////////////////    Control variables    /////////////////////////////////
static char PRINT_LOGFILE = 0;
static char PRINT_SCALAR = 0;
static char PRINT_ADDR = 0;
static char PRINT_PRV = 0;
static char PRINT_REPORT = 0;
static char PRINT_CSV = 0;
static char TRACE_ENABLED = 1; //Enabled by default 
static FILE * FD_PRV;
static FILE * FD_PCF;
static FILE * FD_ROW;
static FILE * FD_CSV;

//#define EPI_07

#ifdef EPI_07
	#undef EPI_10
#else
	#define EPI_10
#endif
///////////////////////////////////////////////////////////////////////////////////////////

#include "qemu_counters.h"
#include "qemu2prv.h"
#include "instr_data.h"


char contains_string(char * str, const char * find){
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


//////////////////////////////////////////////////////////////////////////////////////

QEMU_PLUGIN_EXPORT int qemu_plugin_version = QEMU_PLUGIN_VERSION;

/* Store last executed instruction on each vCPU as a GString */

/**
 * Log instruction execution
 */
static uint64_t qemu_trace_timestamp=0;

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

uint64_t qemu_get_vl(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*2);
}
uint64_t qemu_get_vtype(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*4);
}
uint64_t qemu_get_pc(uint8_t * cpu){
		return *(uint64_t*)(cpu + OFFSET_CPUState + OFFSET_REGS + sizeof_ulong*5);
}
uint64_t qemu_get_xreg(uint8_t * cpu, int reg){
		return *(uint64_t*)(cpu + OFFSET_CPUState + sizeof_ulong*reg); 
}

void *qemu_get_cpu(int index);

static int QEMU_REGION_EVENT = 1000;


static void vcpu_qemu_event(unsigned int cpu_index, uint32_t insn_opcode){
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif
	if (!TRACE_ENABLED) return;

	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;

	int qemu_trace_event = qemu_get_xreg(cpu,src1);
	if (qemu_trace_event != QEMU_REGION_EVENT) return;
	int qemu_trace_value = qemu_get_xreg(cpu,src2);


	if (0 && PRINT_LOGFILE){ //Disabled for now
		event_info * event = find_event(qemu_trace_event); 
		char * string = g_strdup_printf("QEMU_EVENT %d (%s) VALUE %d (%s)\n", qemu_trace_event, event==NULL?"Event name not found" :  event->name, 
																																					qemu_trace_value, get_event_value_name(event,qemu_trace_value)); 
		qemu_plugin_outs(string);
		free(string);
	} 

	qemu_eventandcounters(qemu_trace_event, qemu_trace_value);
	if (PRINT_PRV){
		int row=1;
		fprintf(FD_PRV,"2:%d:1:1:%d:%llu:%d:%d\n",row,row,qemu_trace_timestamp,qemu_trace_event,qemu_trace_value);
		if (!PRINT_SCALAR) fprintf(FD_PRV,"2:%d:1:1:%d:%d:"event_instruction":%d\n", row,row, qemu_trace_timestamp, 1000);
		row=2;
		fprintf(FD_PRV,"2:%d:1:1:%d:%llu:%d:%d\n",row,row,qemu_trace_timestamp,qemu_trace_event,qemu_trace_value);
	}

#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_vcpu_control += time2-time1;
	num_vcpu_control++;
#endif
}

static int qemu_name_offset=-1; //-1: wait for name
static char qemu_event_name[128];
static int qemu_event_number=-1;
static int qemu_value_number=-1;
static char qemu_event_name_first_digit=1;

static void vcpu_qemu_name_event_toggle(unsigned int cpu_index, uint32_t insn_opcode){

		if (qemu_name_offset>0){//End of name
			qemu_event_name[qemu_name_offset]='\0';
			if(qemu_value_number!=-1) add_value_to_event(qemu_event_number,qemu_value_number,qemu_event_name);
			else add_event(qemu_event_number,qemu_event_name);
			qemu_name_offset =-1;
			qemu_event_number=-1;
			qemu_value_number=-1;
		}else{ //Start of name
			qemu_event_name_first_digit=1;
			qemu_name_offset = 0;
		}
}

static void vcpu_qemu_name_event_value(unsigned int cpu_index, uint32_t insn_opcode){
	uint8_t *cpu = qemu_get_cpu(cpu_index);
	int src1 = (insn_opcode>>15)&0x1F;
	int src2 = (insn_opcode>>20)&0x1F;
	qemu_event_number = qemu_get_xreg(cpu,src1);
	qemu_value_number = qemu_get_xreg(cpu,src2);
}

static void vcpu_qemu_name_char(unsigned int cpu_index, uint32_t insn_opcode){

	int value = (insn_opcode>>12)&0xFFFFF;
	/*
	if (qemu_name_offset<0){ //Reading event and value
		if (qemu_event_number == -1){
			qemu_event_number = value;
		}else if (qemu_value_number == -1){
			qemu_value_number = value;
		}
	}else{ //Reading name
	*/	if (qemu_event_name_first_digit==1){
			qemu_event_name[qemu_name_offset] = (char)value;	
			qemu_event_name_first_digit=0;
		}else{
			qemu_event_name[qemu_name_offset++] += (char)(value<<4);
			qemu_event_name_first_digit=1;
		}
//	}
}

static int print_first_scalar = 1;

static void vcpu_restart_trace(unsigned int cpu_index, void *udata){
	//restart prv
	if (PRINT_PRV){
		print_first_scalar = 1;
		FD_PRV = freopen(NULL, "w+", FD_PRV);
		fprintf(FD_PRV,"#Paraver (00/00/0000 at 00:00):1_ns:1(2):1:1:(2:1)\n");
//		fprintf(FD_PRV,"2:1:1:1:1:%d:" event_instruction " 1001\n",qemu_trace_timestamp);
	}
	//restart global section?
	restart_region(global_region);
	qemu_trace_timestamp=0;
	TRACE_ENABLED=1;
}

static void vcpu_start_trace(unsigned int cpu_index, void *udata){
	print_first_scalar = 1;
	TRACE_ENABLED=1;
}
static void vcpu_stop_trace(unsigned int cpu_index, void *udata){
	TRACE_ENABLED=0;
	if (PRINT_PRV){
		fprintf(FD_PRV,"2:%d:1:1:%d:%llu:" clean_event "\n",1,1,qemu_trace_timestamp);
		fprintf(FD_PRV,"2:%d:1:1:%d:%llu:" clean_event "\n",2,2,qemu_trace_timestamp);
	}
}


static int last_row = 1;
static int last_vsetvl = 0;
static int scalar_instr_since_vector=0;
static int waiting_zero_stride = 0;
static void vcpu_insn_exec(unsigned int cpu_index, void *udata){
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif
				instr_data * instr = (instr_data*)udata;

				int row;
				uint64_t vl, vtype, sew, lmul;
				double lmul_value;
				int stride = 0;

				if ( instr->type == VSETVL || instr->type == SCALAR){ //SETVL or individual SCALAR
					row = 1;
					vl=0;
				}else{ //VECTOR
					row = 2;
					uint8_t *cpu = qemu_get_cpu(cpu_index);
					vl = qemu_get_vl(cpu); 
					vtype = qemu_get_vtype(cpu);
#ifdef EPI_07
					sew = (vtype >> 2)&0x7;
					lmul = vtype&0x3;
					lmul_value = (double)(1<<lmul); 
#else
					sew = (vtype >> 3)&0x7;
					lmul = vtype&0x7;
					lmul_value = (lmul < 4) ? (double)(1<<lmul) : (lmul==7)? 0.5 : (lmul==6)? 0.25 : 0.125;
#endif
					if (instr->v_majortype==MEMORY && instr->v_minortype==STRIDE){
						int src2 = (instr->instr32>>20)&0x1F;
						stride = qemu_get_xreg(cpu,src2);
					}
				}

				//  Logfile  //
				if (TRACE_ENABLED){
					if (PRINT_LOGFILE){
						if (!PRINT_SCALAR && instr->type!=SCALAR && scalar_instr_since_vector>0){
							char * string = g_strdup_printf("%d scalar instructions\n", scalar_instr_since_vector); 
	            qemu_plugin_outs(string);
							free(string);
						} 
						if (instr->type!=SCALAR || PRINT_SCALAR){ 
	            qemu_plugin_outs(instr->asm_string);
	            qemu_plugin_outs("\n");
							//char * string = g_strdup_printf(", %d scalar instructions before it\n", scalar_instr_since_vector); 
	            //qemu_plugin_outs(string);
							//free(string);
						}
					}

					//  PRV  //
					if (PRINT_PRV){
						char row_change = row != last_row?1:0;
						if (row_change){
							fprintf(FD_PRV,"2:%d:1:1:%d:%llu:" clean_event "\n",last_row,last_row,qemu_trace_timestamp);
						}
						//Scalar instructions should always be printed when: row changed(1), type changed (2), is first scalar in the trace (3)
						if (instr->type==SCALAR && !PRINT_SCALAR){
							if (row_change || last_vsetvl || print_first_scalar)						
								fprintf(FD_PRV,"2:%d:1:1:%d:%llu:"event_instruction":%d\n", row,row, qemu_trace_timestamp, 1000);
						}else{ //PRINT_SCALAR || instr!=SCALAR
							fprintf(FD_PRV,"2:%d:1:1:%d"
														":%llu"    //timestamp
														":"event_pc":%ld" //PC
														":"event_scalb":%d" //scalar before
														":"event_dst":%d" //dst
														":"event_src1":%d" //src1
														":"event_src2":%d" //src2
														":"event_instruction":%d" //instr
														":"event_vl":%lu"
														":"event_sew":%lu"
														":"event_lmul":%lu",
														row,row,
														qemu_trace_timestamp,
														instr->PC,
														scalar_instr_since_vector,
														instr->dst,
														instr->src1,
														instr->src2,
														instr->paraver_code,
														vl,
														sew,
														lmul);
							if (instr->type==VECTOR && instr->v_majortype==MEMORY && instr->v_minortype==STRIDE){
								fprintf(FD_PRV,":"event_stride":%d",stride);
								waiting_zero_stride = 1;
							}else if (waiting_zero_stride){
								fprintf(FD_PRV,":"event_stride":0");
								waiting_zero_stride = 0;
							}	
							fprintf(FD_PRV,"\n");
						}
					}
				}
				last_row = row;
				print_first_scalar = 0;
				last_vsetvl = instr->type == VSETVL;


				// Counters //
				if (instr->type == VECTOR && instr->v_majortype == MEMORY){
					int width = (instr->instr32 >> 12)&0x3; //3 instead of 7 to %4
					sew = width;
//					sew = width == 5 ? 1 : width == 6 ? 2 : width == 7 ? 3 : 0; //sew is eew for memory instr
				}

				if (instr->type == VECTOR) {
					scalar_instr_since_vector=0;
					++total_counters.vector_instr[sew];
					total_counters.velem[sew] += vl; //(vl*lmul_value); TODO: fix this
					if (instr->v_minortype == FP) ++total_counters.vfp_instr[sew];
					else if (instr->v_minortype == INT) ++total_counters.vint_instr[sew];
					else if (instr->v_majortype == MASK) ++total_counters.vmask_instr[sew];
					else if (instr->v_majortype == MEMORY){
						total_counters.moved_bytes_v += vl*(1<<(sew));
						if (instr->v_minortype == UNIT) ++total_counters.vunit_instr[sew];
						else if (instr->v_minortype == STRIDE){
							++total_counters.vstride_instr[sew];
							total_counters.agg_strides[sew] += stride;
						}
						else if (instr->v_minortype == INDEX) ++total_counters.vidx_instr[sew];
					}
				}else if (instr->type == VSETVL){ //TODO: should this be scalar?
					scalar_instr_since_vector=0;
					++total_counters.vsetvl_instr;
				}else{
					++scalar_instr_since_vector;
					++total_counters.scalar_instr;

					int opcode = (instr->instr32 & 0x3F);
					if (opcode == 0b0000011 || opcode == 0b0100011 || opcode == 0b0000111 || opcode == 0b0100111){ 
						int width = (instr->instr32 >> 12)&0x3; //3 instead of 7 to %4
						total_counters.moved_bytes_s += (1<<(width));
//B 0, 4
//H 1, 5
//W 2, 6
//D 3
					} 
				}
				++qemu_trace_timestamp;
#ifdef TIMEDEBUG
				uint64_t time2 = getmicros();
				time_vcpu_exe += time2-time1;
				num_vcpu_exe++;
#endif
}

/**
 * On translation block new translation
 *
 * QEMU convert code by translation block (TB). By hooking here we can then hook
 * a callback on each instruction and memory access.
 */

//For 0.7.1
#if 0
void CreateMC(int *fd1, int * fd2){
    if (pipe(fd1) == -1){
        perror("pipe");
        exit(-1);
    }
    if (pipe(fd2) == -1){
        perror("pipe");
        exit(-1);
    }
    int PID = fork();
    if (PID==-1){
        perror("PID");
        exit(-1);
    }
    if (PID==0){ //child
        if (dup2(fd1[0], STDIN_FILENO) == -1){ //Child now reads from fd[0]
            perror("dup2");
            exit(-1);
        }
        if (dup2(fd2[1], STDOUT_FILENO) == -1){ //writes to fd[1]
            perror("dup2");
            exit(-1);
        }

        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        execl("/apps/riscv/llvm/EPI-0.7/cross/development/bin/llvm-mc",
              "llvm-mc", "-disassemble", "-triple", "riscv64", "-mattr=+m,+f,+d,+c,+experimental-v",
              (char*) NULL);
        perror("execl");
    }
    //parent
    close(fd2[1]);
    close(fd1[0]);
}

int OpcodeToString(char * buffer, int value){
    sprintf(buffer, "%08x\n", value);
    char b1 = buffer[0];
    char b2 = buffer[1];
    char b3 = buffer[2];
    char b4 = buffer[3];
    char b5 = buffer[4];
    char b6 = buffer[5];
    char b7 = buffer[6];
    char b8 = buffer[7];
    int length = sprintf(buffer, "0x%c%c 0x%c%c 0x%c%c 0x%c%c\n", b7,b8,b5,b6,b3,b4,b1,b2);
	return length+1;
}

#define BUFFSIZE 64
#include <sys/wait.h>
int GetAssembly(char * buffer_in, int length, char * buffer_out, int * fd1, int *fd2){

	write(fd1[1], buffer_in, length);
	close(fd1[1]);
	if (wait(NULL) == -1) {
	    perror("wait");
	    exit(-1);
	}	
	int b = read(fd2[0], buffer_in, BUFFSIZE); //Read for llvm-mc
	int i=0;
	while(i<b && buffer_in[i]!='v') i++;
	for(int j=i; j<b; ++j) buffer_out[j-i] = buffer_in[j];
	CreateMC(fd1,fd2);
	return b-i;
} 

int pipe_helper[2]; // main -> helper
int pipe_helper2[2];// helper -> main
void helper_thread(){
		char buff[BUFFSIZE];
		int fd1[2],fd2[2];
		CreateMC(fd1,fd2);
		while(1){
			int r = read(pipe_helper[0],buff,19); //Read from main user
			if (r!=19) continue;
			int r2 = GetAssembly(buff, r, buff, fd1, fd2); //Get assembly from llvm-mc
			write(pipe_helper2[1],buff,r2); //Send data to main user
		}
}
#endif

#include "my_decode.h"


char is_qemu_restart_trace(uint32_t insn_opcode){
				return (insn_opcode == 0xffe00013)?1:0; //li x0, -2
}
char is_qemu_start_trace(uint32_t insn_opcode){
				return (insn_opcode == 0xffd00013)?1:0; //li x0, -3
}
char is_qemu_stop_trace(uint32_t insn_opcode){
				return (insn_opcode == 0xffc00013)?1:0; //li x0, -4
}
char is_qemu_name_event_value(uint32_t insn_opcode){
//and 0020f033
//	0000 0000 0010) (0000 1)(111) (0000 0)(011 0011)
//and 0020e033
//  0000 0000 0010 (0000 1)(110) (0000 0)(011 0011)
	char is_andx0 = (((insn_opcode&0xFFF)==0x033) && (((insn_opcode>>12)&0x7) == 0x7))?1:0;
	return is_andx0;
}
char is_qemu_name_char(uint32_t insn_opcode){
	char is_luix0 = ((insn_opcode&0xFFF)==0x037)?1:0;
	return is_luix0;
}
char is_qemu_name_event_toggle(uint32_t insn_opcode){
	char is_lix0 = (insn_opcode==0xfff00013)?1:0;
	return is_lix0;
}
char is_qemu_event(uint32_t insn_opcode){
	//char is_orx0 = ((insn_opcode&0xFFF)==0x033)?1:0;
	char is_orx0 = (((insn_opcode&0xFFF)==0x033) && (((insn_opcode>>12)&0x7) == 0x6))?1:0;
	return is_orx0;
}

static void vcpu_tb_trans(qemu_plugin_id_t id, struct qemu_plugin_tb *tb)
{
#ifdef TIMEDEBUG
	uint64_t time1 = getmicros();
#endif

	struct qemu_plugin_insn *insn;
	uint64_t insn_vaddr;
	uint32_t insn_opcode;
	char *insn_disas;

	size_t n = qemu_plugin_tb_n_insns(tb);
	for (size_t i = 0; i < n; i++) {
		/*
		 * `insn` is shared between translations in QEMU, copy needed data here.
		 * `output` is never freed as it might be used multiple times during
		 * the emulation lifetime.
		 * We only consider the first 32 bits of the instruction, this may be
		 * a limitation for CISC architectures.
		 */
		insn = qemu_plugin_tb_get_insn(tb, i);
		insn_vaddr = qemu_plugin_insn_vaddr(insn);
		insn_opcode = *((uint32_t *)qemu_plugin_insn_data(insn));
		insn_disas = qemu_plugin_insn_disas(insn);

		char is_illegal = contains_string(insn_disas,"ill");

		char * output;
		//Dissassembly
		char my_disas[64];
		char is_event=0;
		char is_vector=0;
		if (is_illegal){ //illegal instruction (vector, if we are on 0.7) 
#if 0
			char buffer[21];
			int length = OpcodeToString(buffer, insn_opcode); //int to string ("0xXX 0xXX 0xXX 0xXX")
			write(pipe_helper[1], buffer, length); //Send it to the helper
			int r = read(pipe_helper2[0], my_disas, 64);  //Get answer from the helper
			my_disas[r-1]='\0'; //Terminate it
#else
			MyDissasembler(my_disas, insn_opcode);
			free(insn_disas);
			insn_disas = my_disas;
#endif
			//			if (TRACE_ENABLED) printf("%08x: __%s\n",insn_vaddr,insn_disas); //killme
			//output = g_strdup_printf("vx%"PRIx64", 0x%"PRIx32",\"%s\"", insn_vaddr, insn_opcode, insn_disas);
			is_vector=1;
		}else{
			is_vector = contains_string(insn_disas," v");
		}

		if (is_vector){ //This includes vsetvl
			//output = g_strdup_printf("vx%"PRIx64", 0x%"PRIx32", \"%s\"", insn_vaddr, insn_opcode, insn_disas);
			instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
		}else if (is_qemu_event(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_qemu_event, QEMU_PLUGIN_CB_NO_REGS, insn_opcode);
		}else if (is_qemu_name_event_toggle(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_qemu_name_event_toggle, QEMU_PLUGIN_CB_NO_REGS, NULL);
		}else if (is_qemu_name_event_value(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_qemu_name_event_value, QEMU_PLUGIN_CB_NO_REGS, insn_opcode);
		}else if (is_qemu_name_char(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_qemu_name_char, QEMU_PLUGIN_CB_NO_REGS, insn_opcode);
		}else if (is_qemu_restart_trace(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_restart_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		}else if (is_qemu_start_trace(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_start_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		}else if (is_qemu_stop_trace(insn_opcode)){
			qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_stop_trace, QEMU_PLUGIN_CB_R_REGS, NULL);
		}else{ //Scalar instruction
			//output = g_strdup_printf("0x%"PRIx64", 0x%"PRIx32", \"%s\"", insn_vaddr, insn_opcode, insn_disas); //normal
			if (PRINT_SCALAR){
				instr_data * insn_struct = fill_instr_struct(insn_vaddr, insn_disas, insn_opcode);
				qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct);
			}else{ //TODO: I need a callback for noprint
				instr_basic_data * insn_struct = (instr_basic_data *)malloc(sizeof(instr_basic_data));
				insn_struct->type = SCALAR; insn_struct->instr32  = insn_opcode;
				qemu_plugin_register_vcpu_insn_exec_cb(insn, vcpu_insn_exec, QEMU_PLUGIN_CB_R_REGS, insn_struct /*scalar_empty_struct*/);
			}
		}
	}
#ifdef TIMEDEBUG
	uint64_t time2 = getmicros();
	time_trans += time2-time1;
	num_trans++;
#endif

}


/**
 * On plugin exit, print last instruction in cache
 */
static void plugin_exit(qemu_plugin_id_t id, void *p)
{
		qemu_eventandcounters(-1, 0); //End Global event
		if(PRINT_REPORT){
			print_report();
		}
		if (PRINT_CSV){
			print_csv(FD_CSV);
		}
		
		free_regions();
#ifdef TIMEDEBUG
		printf("Cycles in Translation: %.4f %d times, %lu (%.2f %%)\n", (double)time_trans/num_trans, num_trans, time_trans, (double)time_trans/(time_trans+time_vcpu_exe+time_vcpu_control));
		printf("Cycles in VCPU_exe: %.4f %d times, %lu (%.2f %%)\n", (double)time_vcpu_exe/num_vcpu_exe, num_vcpu_exe, time_vcpu_exe, (double)time_vcpu_exe/(time_trans+time_vcpu_exe+time_vcpu_control));
		printf("Cycles in VCPU_event: %.4f %d times, %lu (%.2f %%)\n", (double)time_vcpu_control/num_vcpu_control, num_vcpu_control, time_vcpu_control, (double)time_vcpu_control/(time_trans+time_vcpu_exe+time_vcpu_control));
#endif

		if (PRINT_PRV){
			events_and_values_to_pcf(FD_PCF);
			fclose(FD_PRV);
			fclose(FD_PCF);
			fclose(FD_ROW);
		}
}

/**
 * Install the plugin
 */



QEMU_PLUGIN_EXPORT int qemu_plugin_install(qemu_plugin_id_t id,
                                           const qemu_info_t *info, int argc,
                                           char **argv)
{
	for(int i=0; i<argc; ++i){
			if (contains_string(argv[i], "PRINT_SCALAR")) PRINT_SCALAR = 1;
			else if (contains_string(argv[i], "PRINT_ADDR")) PRINT_ADDR = 1;
			else if (contains_string(argv[i], "PRINT_PRV")) PRINT_PRV = 1;
			else if (contains_string(argv[i], "PRINT_LOGFILE")) PRINT_LOGFILE = 1;
			else if (contains_string(argv[i], "PRINT_REPORT")) PRINT_REPORT = 1;
			else if (contains_string(argv[i], "PRINT_CSV")) PRINT_CSV = 1;
			else if (contains_string(argv[i], "PRV_NAME")){
							//++i;
							int j; for(j=0; j<strlen(argv[i]); ++j)	if (argv[i][j] == '=') break;
							setup_paraver_trace(&argv[i][j+1]);
							//setup_paraver_trace(argv[i]);
							fprintf(FD_PRV,"#Paraver (00/00/0000 at 00:00):1_ns:1(2):1:1:(2:1)\n");
			}
			else if (contains_string(argv[i], "CSV_NAME")){
				//++i;
				int j; for(j=0; j<strlen(argv[i]); ++j) if (argv[i][j] == '=') break;
				FD_CSV = fopen(&argv[i][j+1], "w+");
			}
	}
#if 0
    if (pipe(pipe_helper) == -1){
        perror("pipe");
        exit(-1);
    }
    if (pipe(pipe_helper2) == -1){
        perror("pipe");
        exit(-1);
    }
	int pid = fork();
	if (pid==0) helper_thread();
#endif
	if (!PRINT_SCALAR){
			scalar_empty_struct = (instr_data*)malloc(sizeof(instr_data));
			scalar_empty_struct->PC=0;
			scalar_empty_struct->paraver_code=1000;
			scalar_empty_struct->src1=0;
			scalar_empty_struct->src2=0;
			scalar_empty_struct->src3=0;
			scalar_empty_struct->dst=0;
			scalar_empty_struct->type=SCALAR;
			scalar_empty_struct->asm_string='\0';
	}
	add_event(-1,"Global");
	qemu_eventandcounters(-1, 1); //Start global event

    /* Register translation block and exit callbacks */
    qemu_plugin_register_vcpu_tb_trans_cb(id, vcpu_tb_trans);
    qemu_plugin_register_atexit_cb(id, plugin_exit, NULL);


    return 0;
}
