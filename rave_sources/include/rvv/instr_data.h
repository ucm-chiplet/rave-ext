/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* [xxxx][tttt][MMMM][mmmm] */
#define T_NOTYPE 0x0000

#define T_SCALAR 0x1000
		#define T_BRANCH 0x0100
#define T_VECTOR 0x2000
#define T_VSETVL 0x3000

#define T_OTHER  0x0000
#define T_ARITH  0x0100
#define T_REDUCTION 0x0500
	#define T_FP     0x0010
	#define T_INT    0x0020
		#define T_SINGLE 0x0001
		#define T_FUSED	 0x0002

#define T_LOAD   0x0200
#define T_STORE  0x0300
#define T_MASK   0x0400
#define T_MEMORY T_LOAD //Until we report ld/st separately
	#define T_UNIT   0x0010
	#define T_STRIDE 0x0020
	#define T_INDEX  0x0030
	#define T_SPILL  0x0040


#define is_type(x,y) (((x^y)&0xF000)==0)
#define is_subtype(x,y) (((x^y)&0x0F00)==0)
#define is_subsubtype(x,y) (((x^y)&0x00F0)==0)
#define is_subsubsubtype(x,y) (((x^y)&0x000F)==0)

struct instr_data{
	//enum instr_type type;
	uint16_t type;
	uint32_t instr32; //Only for strided...and mem eew.. and scalar mem?
  uint64_t PC;

	uint32_t paraver_code;
	char * asm_string;
	short src1;
	short src2;
	short src3;
	short dst;
//	enum v_major_type v_majortype;
//	enum v_minor_type v_minortype;
};
typedef struct instr_data instr_data;

struct qemu_event{
	int event;
	int value;
};
typedef struct qemu_event qemu_event;

//instr_data * scalar_empty_struct;

#define MAJOR_LOAD 0b0000111
#define MAJOR_STORE 0b0100111
#define MAJOR_ARITH 0b1010111
#define get_bit_field(insn_opcode, high, low) ((insn_opcode >> low) & ((1<<(high-low+1))-1))
instr_data * fill_instr_struct(uint64_t pc, char * instr, uint32_t insn_opcode, int PRINT_PRV);
uint16_t instr_set_scalar_type(uint32_t insn_opcode);
int64_t get_loop_offset(uint32_t insn_opcode);




