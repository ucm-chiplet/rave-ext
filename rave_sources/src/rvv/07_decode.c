/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "07_decode.h"

int MyDissasembler(char * instrbuff, unsigned int instr){
	const char * vregs[]={"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"};
	const char * iregs[]={"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
	const char * fregs[]={"ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7", "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"};
	int size=0;	
#define OPV 0x57 
#define LOADFP 0x07
#define STOREFP 0x27 
#define AMO 0x30
	unsigned int rs1 = (instr>>15)&0x1F;
	unsigned int rs2 = (instr>>20)&0x1F;
	unsigned int dst = (instr>>7)&0x1F;
	unsigned int major = (instr)&0x7F;
	unsigned int masked = ((instr>>25)&0x01)?0:1;
	unsigned int funct3=(instr>>12)&0x7;
	unsigned int mop=(instr>>26)&0x7;
	unsigned int nf;
	switch(major){
		case LOADFP:
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0: 
					switch(funct3){
						case 0:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlbu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8bu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlbuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8buff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
						case 5:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlhu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8hu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlhuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8huff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
						case 6:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlwu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8wu.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlwuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8wuff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
						case 7:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vle.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vleff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8eff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
					} break;
				case 2: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlsbu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8bu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlshu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8hu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlswu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8wu.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 7:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlse.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
					} break;
				case 3: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxbu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8bu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxhu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8hu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxwu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8wu.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 7:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxe.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
					} break;
				case 4: 
					switch(funct3){
						case 0:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlb.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlbff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8bff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
						case 5:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlh.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlhff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8hff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
						case 6:
							switch(rs2){
								case 0:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlw.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
								case 16:
									switch(nf){
										case 0:
											size = sprintf(instrbuff, "vlwff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 1:
											size = sprintf(instrbuff, "vlseg2wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 2:
											size = sprintf(instrbuff, "vlseg3wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 3:
											size = sprintf(instrbuff, "vlseg4wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 4:
											size = sprintf(instrbuff, "vlseg5wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 5:
											size = sprintf(instrbuff, "vlseg6wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 6:
											size = sprintf(instrbuff, "vlseg7wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
										case 7:
											size = sprintf(instrbuff, "vlseg8wff.v %s, (%s)", vregs[dst], iregs[rs1]); break;
									}break;
							}; break;
					} break;
				case 6: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlsb.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlsh.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlsw.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlsseg2w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlsseg3w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlsseg4w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlsseg5w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlsseg6w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlsseg7w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlsseg8w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
					} break;
				case 7: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxb.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxh.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vlxw.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vlxseg2w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vlxseg3w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vlxseg4w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vlxseg5w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vlxseg6w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vlxseg7w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vlxseg8w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
					} break;
			}
			break;
		case STOREFP:
			nf=(instr>>29)&0x7;
			switch(mop){
				case 0: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsb.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 1:
									size = sprintf(instrbuff, "vsseg2b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 2:
									size = sprintf(instrbuff, "vsseg3b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 3:
									size = sprintf(instrbuff, "vsseg4b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 4:
									size = sprintf(instrbuff, "vsseg5b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 5:
									size = sprintf(instrbuff, "vsseg6b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 6:
									size = sprintf(instrbuff, "vsseg7b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 7:
									size = sprintf(instrbuff, "vsseg8b.v %s, (%s)", vregs[dst], iregs[rs1]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsh.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 1:
									size = sprintf(instrbuff, "vsseg2h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 2:
									size = sprintf(instrbuff, "vsseg3h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 3:
									size = sprintf(instrbuff, "vsseg4h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 4:
									size = sprintf(instrbuff, "vsseg5h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 5:
									size = sprintf(instrbuff, "vsseg6h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 6:
									size = sprintf(instrbuff, "vsseg7h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 7:
									size = sprintf(instrbuff, "vsseg8h.v %s, (%s)", vregs[dst], iregs[rs1]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsw.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 1:
									size = sprintf(instrbuff, "vsseg2w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 2:
									size = sprintf(instrbuff, "vsseg3w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 3:
									size = sprintf(instrbuff, "vsseg4w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 4:
									size = sprintf(instrbuff, "vsseg5w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 5:
									size = sprintf(instrbuff, "vsseg6w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 6:
									size = sprintf(instrbuff, "vsseg7w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 7:
									size = sprintf(instrbuff, "vsseg8w.v %s, (%s)", vregs[dst], iregs[rs1]); break;
							}break;
						case 7:
							size = sprintf(instrbuff, "vse.v %s, (%s)", vregs[dst], iregs[rs1]); break;
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vse.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 1:
									size = sprintf(instrbuff, "vsseg2e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 2:
									size = sprintf(instrbuff, "vsseg3e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 3:
									size = sprintf(instrbuff, "vsseg4e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 4:
									size = sprintf(instrbuff, "vsseg5e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 5:
									size = sprintf(instrbuff, "vsseg6e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 6:
									size = sprintf(instrbuff, "vsseg7e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
								case 7:
									size = sprintf(instrbuff, "vsseg8e.v %s, (%s)", vregs[dst], iregs[rs1]); break;
							}break;
					} break;

				case 2: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vssb.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vssseg2b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vssseg3b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vssseg4b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vssseg5b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vssseg6b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vssseg7b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vssseg8b.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vssh.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vssseg2h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vssseg3h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vssseg4h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vssseg5h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vssseg6h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vssseg7h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vssseg8h.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vssw.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vssseg2w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vssseg3w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vssseg4w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vssseg5w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vssseg6w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vssseg7w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vssseg8w.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
						case 7:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsse.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vssseg2e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vssseg3e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vssseg4e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vssseg5e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vssseg6e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vssseg7e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vssseg8e.v %s, (%s), %s", vregs[dst], iregs[rs1], iregs[rs2]); break;
							}break;
					} break;
				case 3: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsxb.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsxseg2b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsxseg3b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsxseg4b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsxseg5b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsxseg6b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsxseg7b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsxseg8b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsxh.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsxseg2h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsxseg3h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsxseg4h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsxseg5h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsxseg6h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsxseg7h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsxseg8h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsxw.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsxseg2w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsxseg3w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsxseg4w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsxseg5w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsxseg6w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsxseg7w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsxseg8w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 7:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsxe.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsxseg2e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsxseg3e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsxseg4e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsxseg5e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsxseg6e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsxseg7e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsxseg8e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
					} break;
				case 7: 
					switch(funct3){
						case 0:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsuxb.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsuxseg2b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsuxseg3b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsuxseg4b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsuxseg5b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsuxseg6b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsuxseg7b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsuxseg8b.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 5:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsuxh.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsuxseg2h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsuxseg3h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsuxseg4h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsuxseg5h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsuxseg6h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsuxseg7h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsuxseg8h.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 6:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsuxw.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsuxseg2w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsuxseg3w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsuxseg4w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsuxseg5w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsuxseg6w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsuxseg7w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsuxseg8w.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
						case 7:
							switch(nf){
								case 0:
									size = sprintf(instrbuff, "vsuxe.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vsuxseg2e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vsuxseg3e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vsuxseg4e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 4:
									size = sprintf(instrbuff, "vsuxseg5e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 5:
									size = sprintf(instrbuff, "vsuxseg6e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 6:
									size = sprintf(instrbuff, "vsuxseg7e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
								case 7:
									size = sprintf(instrbuff, "vsuxseg8e.v %s, (%s), %s", vregs[dst], iregs[rs1], vregs[rs2]); break;
							}break;
					} break;
			}
			break;
		case AMO:
			break;
		case OPV:
			if ((((instr>>25) & 0x7F)==64) && (((instr>>12)&0x7)==7)){ //is vsetvl
				size = sprintf(instrbuff, "vsetvl %s, %s, %s", iregs[dst], iregs[rs1], iregs[rs2]);
				break;
			}else if ((((instr>>31) & 0x1)==0) && (((instr>>12)&0x7)==7)){ //is vsetvli
				unsigned int sew = 1 << (((instr>>22)&0x7)+3);
				unsigned int lmul = (((instr>>20)&0x3)+1);
				size = sprintf(instrbuff, "vsetvli %s, %s, e%d, m%d", iregs[dst], iregs[rs1], sew, lmul);
				break;
			}
			unsigned int funct6=(instr>>26)&0x3F;
			switch(funct6){
				case 0:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vadd.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vadd.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredsum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfadd.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 1:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredand.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfredsum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 2:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredor.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfsub.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 3:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vrsub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vrsub.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredxor.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfredosum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 4:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vminu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vminu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredminu.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmin.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmin.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 5:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmin.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmin.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredmin.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfredmin.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 6:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmaxu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmaxu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredmaxu.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmax.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmax.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 7:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmax.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmax.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vredmax.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfredmax.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 8:
					switch(funct3){
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfsgnj.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfsgnj.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 9:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vand.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vand.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vand.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfsgnjn.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfsgnjn.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 10:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vor.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vor.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vor.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfsgnjx.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfsgnjx.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 11:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vxor.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vxor.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							if (rs1==31) size = sprintf(instrbuff, "vnot.v %s, %s", vregs[dst], vregs[rs2]);
							else size = sprintf(instrbuff, "vxor.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1);
							break;
					} break;
				case 12:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vrgather.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vrgather.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vrgather.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							if (rs1==0) size = sprintf(instrbuff, "vmv.x.s %s, %s", iregs[dst], vregs[rs2]);
							else size = sprintf(instrbuff, "vext.x.v %s, %s, %s", iregs[dst], vregs[rs2], iregs[rs1]);
							break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmv.f.s %s, %s", fregs[dst], vregs[rs2]); break;
					} break;
				case 13:
					switch(funct3){
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmv.s.x %s, %s", vregs[dst], iregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmv.s.f %s, %s", vregs[dst], fregs[rs1]); break;
					} break;
				case 14:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vslideup.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vslideup.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vslide1up.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 15:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vslidedown.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vslidedown.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vslide1down.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 16:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vadc.vvm %s, %s, %s, v0", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vadc.vxm %s, %s, %s, v0", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vadc.vim %s, %s, %d, v0", vregs[dst], vregs[rs2], rs1); break;
					} break;
				case 17:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmadc.vvm %s, %s, %s, v0", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmadc.vxm %s, %s, %s, v0", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmadc.vim %s, %s, %d, v0", vregs[dst], vregs[rs2], rs1); break;
					} break;
				case 18:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsbc.vvm %s, %s, %s, v0", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsbc.vxm %s, %s, %s, v0", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 19:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmsbc.vvm %s, %s, %s, v0", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsbc.vxm %s, %s, %s, v0", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 20:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmpopc.m %s, %s", iregs[dst], vregs[rs2]); break;
					} break;
				case 21:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmfirst.m %s, %s", iregs[dst], vregs[rs2]); break;
					} break;
				case 22:
					switch(funct3){
						case 2: //OPMVV
							switch(rs1){
								case 1:
									size = sprintf(instrbuff, "vmsbf.m %s, %s", vregs[dst], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vmsof.m %s, %s", vregs[dst], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vmsif.m %s, %s", vregs[dst], vregs[rs2]); break;
								case 16:
									size = sprintf(instrbuff, "viota.m %s, %s", vregs[dst], vregs[rs2]); break;
								case 17:
									size = sprintf(instrbuff, "vid.v %s", vregs[dst]); break;
							}break;
					} break;
				case 23:
					switch(funct3){
						case 0: //OPIVV
							if (!masked) {size = sprintf(instrbuff, "vmv.v.v %s, %s", vregs[dst], vregs[rs1]); break;}
							else {size = sprintf(instrbuff, "vmerge.vvm %s, %s, %s, v0", vregs[dst], vregs[rs2], vregs[rs1]); break;}
						case 4: //OPIVX
							if (!masked) {size = sprintf(instrbuff, "vmv.v.x %s, %s", vregs[dst], iregs[rs1]); break;}
							else {size = sprintf(instrbuff, "vmerge.vxm %s, %s, %s, v0", vregs[dst], vregs[rs2], iregs[rs1]); break;}
						case 3: //OPIVI
							if (!masked) {size = sprintf(instrbuff, "vmv.v.i %s, %d", vregs[dst], rs1); break;}
							else {size = sprintf(instrbuff, "vmerge.vim %s, %s, %d, v0", vregs[dst], vregs[rs2], rs1); break;}
						case 2: //OPMVV
							size = sprintf(instrbuff, "vcompress.vm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							if (!masked) {size = sprintf(instrbuff, "vfmv.v.f %s, %s", vregs[dst], fregs[rs1]); break;}
							else {size = sprintf(instrbuff, "vfmerge.vfm %s, %s, %s, v0", vregs[dst], vregs[rs2], fregs[rs1]); break;}
					} break;
				case 24:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmseq.vv %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmseq.vx %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmseq.vi %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmandnot.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vmfeq.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmfeq.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 25:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmsne.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsne.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmsne.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							if (rs2==rs1)	size = sprintf(instrbuff, "vmcpy.m %s, %s", vregs[dst], vregs[rs1]);
							else size = sprintf(instrbuff, "vmand.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]);
							break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vmfle.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmfle.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 26:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmsltu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsltu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmor.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vmford.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmford.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 27:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmslt.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmslt.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							if (dst==rs1 && dst==rs2) size = sprintf(instrbuff, "vmclr.m %s", vregs[dst]); 
							else size = sprintf(instrbuff, "vmxor.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]);
							break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vmflt.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmflt.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 28:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmsleu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsleu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmsleu.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmornot.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vmfne.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmfne.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 29:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vmsle.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsle.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmsle.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							if (rs1==rs2) size = sprintf(instrbuff, "vmnot.m %s, %s", vregs[dst], vregs[rs2]);
							else size = sprintf(instrbuff, "vmnand.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]);
							break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmfgt.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 30:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsgtu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmsgtu.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmnor.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 31:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vmsgt.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vmsgt.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							if (dst==rs2 && dst==rs1) size = sprintf(instrbuff, "vmset.m %s", vregs[dst]);
							else size = sprintf(instrbuff, "vmxnor.mm %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]);
							break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vmfge.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 32:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsaddu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsaddu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vsaddu.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vdivu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vdivu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfdiv.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfdiv.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 33:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsadd.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vsadd.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vdiv.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vdiv.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfrdiv.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 34:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vssubu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vssubu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vremu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vremu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							switch(rs1){
								case 0:
									size = sprintf(instrbuff, "vfcvt.xu.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 1:
									size = sprintf(instrbuff, "vfcvt.x.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 2:
									size = sprintf(instrbuff, "vfcvt.f.xu.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 3:
									size = sprintf(instrbuff, "vfcvt.f.x.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 8:
									size = sprintf(instrbuff, "vfwcvt.xu.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 9:
									size = sprintf(instrbuff, "vfwcvt.x.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 10:
									size = sprintf(instrbuff, "vfwcvt.f.xu.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 11:
									size = sprintf(instrbuff, "vfwcvt.f.x.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 12:
									size = sprintf(instrbuff, "vfwcvt.f.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 16:
									size = sprintf(instrbuff, "vfncvt.xu.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 17:
									size = sprintf(instrbuff, "vfncvt.x.f.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 18:
									size = sprintf(instrbuff, "vfncvt.f.xu.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 19:
									size = sprintf(instrbuff, "vfncvt.f.x.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 20:
									size = sprintf(instrbuff, "vfncvt.f.f.v %s, %s", vregs[dst], vregs[rs2]); break;
							}break;
					} break;
				case 35:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vssub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vssub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vrem.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vrem.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							switch(rs1){
								case 0:
									size = sprintf(instrbuff, "vfsqrt.v %s, %s", vregs[dst], vregs[rs2]); break;
								case 16:
									size = sprintf(instrbuff, "vfclass.v %s, %s", vregs[dst], vregs[rs2]); break;
							}break;
					} break;
				case 36:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vaadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vaadd.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vaadd.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmulhu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmulhu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmul.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmul.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 37:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsll.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsll.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vsll.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmul.vv %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmul.vx %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 38:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vasub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vasub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmulhsu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmulhsu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 39:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsmul.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsmul.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmulh.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmulh.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfrsub.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 40:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsrl.vv %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsrl.vx %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vsrl.vi %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmadd.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 41:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vsra.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vsra.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vsra.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmadd.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfnmadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfnmadd.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 42:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vssrl.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vssrl.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vssrl.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmsub.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 43:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vssra.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vssra.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vssra.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vnmsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vnmsub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfnmsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfnmsub.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 44:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vnsrl.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vnsrl.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vnsrl.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmacc.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 45:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vnsra.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vnsra.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vnsra.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vmacc.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfnmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfnmacc.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 46:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vnclipu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vnclipu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vnclipu.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfmsac.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfmsac.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 47:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vnclip.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vnclip.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 3: //OPIVI
							size = sprintf(instrbuff, "vnclip.vi %s, %s, %d", vregs[dst], vregs[rs2], rs1); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vnmsac.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vnmsac.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfnmsac.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfnmsac.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 48:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vwredsumu.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwaddu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwaddu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwadd.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 49:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vwredsum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwadd.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwadd.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwredsum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 50:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwsubu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwsubu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwsub.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 51:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwsub.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwsub.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwredosum.vs %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 52:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwaddu.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwaddu.wx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwadd.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwadd.wf %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 53:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwadd.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwadd.wx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 54:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwsubu.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwsubu.wx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwsub.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwsub.wf %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 55:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwsub.wv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwsub.wx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 56:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vdotu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmulu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmulu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwmul.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwmul.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 57:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vdot.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfdot.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 58:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmulsu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmulsu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
					} break;
				case 59:
					switch(funct3){
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmul.vv %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmul.vx %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
					} break;
				case 60:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vwsmaccu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vwsmaccu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmaccu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmaccu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwmacc.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 61:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vwsmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vwsmacc.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmacc.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwnmacc.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwnmacc.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 62:
					switch(funct3){
						case 0: //OPIVV
							size = sprintf(instrbuff, "vwsmaccsu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 4: //OPIVX
							size = sprintf(instrbuff, "vwsmaccsu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 2: //OPMVV
							size = sprintf(instrbuff, "vwmaccsu.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmaccsu.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwmsac.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwmsac.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
				case 63:
					switch(funct3){
						case 4: //OPIVX
							size = sprintf(instrbuff, "vwsmaccus.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 6: //OPMVX
							size = sprintf(instrbuff, "vwmaccus.vx %s, %s, %s", vregs[dst], vregs[rs2], iregs[rs1]); break;
						case 1: //OPFVV
							size = sprintf(instrbuff, "vfwnmsac.vv %s, %s, %s", vregs[dst], vregs[rs2], vregs[rs1]); break;
						case 5: //OPFVF
							size = sprintf(instrbuff, "vfwnmsac.vf %s, %s, %s", vregs[dst], vregs[rs2], fregs[rs1]); break;
					} break;
					break;
			}
			break;
	}
	//if (size==0) printf("f6: %d f3: %d maj: %02x , nf: %d mop: %d rs1: %d rs2: %d\n",(instr>>26)&0x3F,funct3,major,nf,mop,rs1,rs2);
	if (size==0) size = sprintf(instrbuff, "illegal");
	return size;
}
