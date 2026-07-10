/*********************************************************/
// Copyright (C) 2026 Barcelona Supercomputing Center-Centro Nacional de Supercomputación
// SPDX-License-Identifier: BSD-3-Clause
/*********************************************************/
// * Author: Pablo Vizcaino
// * Email:  pablo.vizcaino@bsc.es
/*********************************************************/

#include "instr_data.h"
#include "utils.h" //For contains_string(char*)
#include "rave2prv.h"
#include "instr2prv.h"

int64_t get_loop_offset(uint32_t insn_opcode){
	int64_t offset;
	if ((insn_opcode&0x3)!=0x3){ //Compressed
		offset = (((insn_opcode>>3)&0x3)<<1) + (((insn_opcode>>10)&0x3)<<3) + (((insn_opcode>>2)&0x1)<<5) + (((insn_opcode>>5)&0x3)<<6) + (((insn_opcode>>12)&0x1)<<8);
		//Sign extend the 9 bit number
		offset <<= (64-9);
		offset >>= (64-9);
	}else{
		offset = (((insn_opcode>>31)&0x1)<<12) + (((insn_opcode>>7)&0x1)<<11) + (((insn_opcode>>25)&0x3F)<<5) + (((insn_opcode>>8)&0xF)<<1);
		//Sign extend the 13 bit number
		offset <<= (64-13);
		offset >>= (64-13);
	}
	return offset;
}

int16_t instr_set_vector_type(uint32_t insn_opcode){
				int16_t type = T_VECTOR; 
				int16_t subtype = T_OTHER;
				int16_t subsubtype = T_NOTYPE;
				int16_t subsubsubtype = T_NOTYPE;


				int opcode = get_bit_field(insn_opcode,6,0); 
				int funct6,funct3,mop,vs1;
				switch (opcode){
								case MAJOR_LOAD:
												subtype = T_LOAD;
#ifdef RVV_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0 || mop == 4) subsubtype = T_UNIT;
												else if (mop == 2 || mop == 6) subsubtype = T_STRIDE;
												else if (mop == 3 || mop == 7) subsubtype = T_INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0){
													unsigned int rs2 = get_bit_field(insn_opcode,24,20);
													if (rs2 == 8) subsubtype = T_SPILL;
													else subsubtype = T_UNIT;
												}
												else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 1 || mop == 3) subsubtype = T_INDEX; 
#endif
												break;
								case MAJOR_STORE:
												subtype = T_STORE;
#ifdef RVV_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0) subsubtype = T_UNIT;
												else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 3 || mop == 7) subsubtype = T_INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0){ 
													unsigned int rs2 = get_bit_field(insn_opcode,24,20);
													if (rs2 == 8) subsubtype = T_SPILL;
													else subsubtype = T_UNIT;
												}else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 1 || mop == 3) subsubtype = T_INDEX; 
#endif
												break;
								case MAJOR_ARITH:
												funct3 = get_bit_field(insn_opcode,14,12);
												funct6 = get_bit_field(insn_opcode,31,26);
												if (funct3 == 1 || funct3 == 5){ // OPFVV, OPFVF
																if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask gen
																				subtype = T_MASK;
#ifdef RVV_07
																}else if (funct6 != 0b001100 && //vfmv.f.s
																					funct6 != 0b001101 && //vfmv.s.f
																					funct6 != 0b010111 && //vfmerge, vfmv
																					funct6 != 0b100010 && //fconverts
																					(funct6 != 0b100011 || get_bit_field(insn_opcode,19,15)!=0b10000)){ //fclass
#else
																}else if (funct6 != 0b001110 && //vfslide1up
																					funct6 != 0b001111 && //vfslide1down
																					funct6 != 0b010000 && //vfmv.f.s or vfmv.s.f
																					funct6 != 0b010010 && //converts
																					funct6 != 0b010111 && //vfmerge, vfmv
																					(funct6 != 0b010011 || get_bit_field(insn_opcode,19,15)!=0b10000)){ //fclass
#endif
																				if (funct6 == 1 || funct6 ==3 || funct6 == 5 || funct6 == 7 || funct6 == 49 || funct6 == 51){
																					subtype = T_REDUCTION;
																				}else{
																					subtype = T_ARITH;
																					subsubsubtype = (funct6 >= 40 && funct6 <= 48) ? T_FUSED : T_SINGLE;
																				}

																				subsubtype = T_FP;
																}
												}else if (funct3 == 0 || funct3 == 3 || funct3 == 4){ //OPIVV, OPIVI, OPIVX
																if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask gen
																				subtype = T_MASK;
																}else if (funct6 != 0b001100 && //vrgather
																					funct6 != 0b001110 && //slideup, gatherei16 in 1.0
																					funct6 != 0b001111 && //slidedown
																					funct6 != 0b101110 && //vnclipu
																					funct6 != 0b101111 && //vnclip
																					funct6 != 0b010111 //vmerge/vmv
#ifndef RVV_07																																
																					&& (funct6 != 0b100111 || funct3 != 3) //vmv1r
#endif																																							
																					){ 
																		if (funct6 == 48 || funct6 == 49){
																				subtype = T_REDUCTION;
																		}else{
																				subtype = T_ARITH;
																				subsubsubtype = T_SINGLE;
																		}
																				subsubtype = T_INT;
																}
												}else if (funct3 == 2 || funct3 == 6){ //OPMVV, OPMVX,
#ifdef RVV_07
																if (funct6 == 0b010110){ //VMUNARY0
																	vs1 = get_bit_field(insn_opcode,19,15);
																	if (vs1 >= 0b00001 && vs1 <= 0b00011){
																				subtype = T_MASK;
																	}
#else
																if (funct6 == 0b010100){ //VMUNARY0
																	vs1 = get_bit_field(insn_opcode,19,15);
																	if (vs1 >= 0b00001 && vs1 <= 0b00011){
																				subtype = T_MASK;
																	}
#endif
																}else if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask arith
																				subtype = T_MASK;
#ifdef RVV_07
																}else if (funct6 != 0b001101 && //vmv.s.x
																					funct6 != 0b001110 && //slide1up
																					funct6 != 0b001111 && //slide1down
																					funct6 != 0b010100 && //popc
																					funct6 != 0b010101 && //vmfirst
																					funct6 != 0b001100 && //vext
																					funct6 != 0b010111){ //vmcompress
#else
																}else if (funct6 != 0b010000 && //vmv.s.x, vmv.x.s, vpopc, vfirst
																					funct6 != 0b001110 && //slide1up
																					funct6 != 0b001111 && //slide1down
																					funct6 != 0b010010 && //vzext
																					funct6 != 0b010111){ //vmcompress
#endif
																				if ((funct6 >= 0 && funct6 <= 7) /*|| (funct6 == 48 || funct6 == 49)*/){
																					subtype = T_REDUCTION;
																				}else{
																					subtype = T_ARITH;
																					subsubsubtype = (funct6 == 41 || funct6 == 43 || funct6 == 45 || funct6 == 47) ? T_FUSED : T_SINGLE;
																				}
																				subsubtype = T_INT;
																}
												}
												break;
								default:
												break;
				}
				return (type | subtype | subsubtype | subsubsubtype);
}

uint16_t instr_set_scalar_type(uint32_t insn_opcode){

	uint16_t type = T_SCALAR;
	int f7 = (insn_opcode & 0x7F);
	int quadrant = (insn_opcode&0x3);
	if (quadrant!=0x3){ //Compressed
		int f3 = (insn_opcode>>13)&0x7;

		if  (quadrant==1){
		 	if (f3 >= 6){
				type |= T_BRANCH;
			}else if (f3 == 5){
				type |= T_JUMP;
			}
		}
		else if (quadrant==0 && f3!=0){
			type |= T_MEMORY;
		}
		else if (quadrant==2){
			int rs1 = (insn_opcode >> 7)&0x1F;
			int rs2 = (insn_opcode >> 2)&0x1F;
		  if ((f3>=1 && f3 <=3) || (f3>=5)){
				type |= T_MEMORY;
			}else if (f3==4 && rs1!=0 && rs2==0){
				type |= T_JUMP;
			}
		}
	}else{ //Not Compressed
		if (f7 == 0x063){
		 	type |= T_BRANCH;
		}
		else if (f7 == 0b0000011 || f7 == 0b0100011 || f7 == 0b0000111 || f7 == 0b0100111){ 
		 	type |= T_MEMORY;
		}
		else if (f7 == 0b1010011) type |= T_SINGLE;
		else if (f7 == 0b1000011) type |= T_FUSED;
		else if (f7 == 0b1101111 || f7 == 0b1100111) type |= T_JUMP; 
	}

	return type;
}

instr_data * fill_instr_struct(uint64_t pc, char * instr, uint32_t insn_opcode, int PRINT_PRV){

	char * instr_fields[8]; //8 is more than enough

	//Look for fields in char* instr 
	int field_start=0;
	#ifdef RVV_07
	int field_idx=-1; //Skip first
	#else
	int field_idx=0; 
	#endif
	int reading_field=0;
	for(int i=0;; ++i){
		char c = instr[i];
		if (c==',' || c==' ' || c=='"' || c=='(' || c==')' || c=='\0'){
			if (reading_field){
				reading_field=0;
				if (field_idx >= 0){
					int field_length = (i-field_start+1);
					instr_fields[field_idx]	= malloc(sizeof(char)*field_length);
					for(int j=0; j<field_length-1; ++j) instr_fields[field_idx][j] = instr[field_start+j];
					instr_fields[field_idx][field_length-1]='\0';
	
				}
				field_idx++;
	
				if (field_idx > 7){
				 	printf("Too many fields\n");
					break;
				}
			}
		}else{
			if (reading_field==0) field_start=i;
			reading_field=1;
		}
		if (c=='\0') break;
	}


	//Fill fields in struct
	instr_data * data = (instr_data*)malloc(sizeof(instr_data));

	my_strcpy(data->asm_string,instr);
	data -> instr32 = insn_opcode;
	
	data -> PC = pc;
	data -> dst =  (field_idx > 1) ? reg2prv(instr_fields[1]) : 0;
	data -> src1 = (field_idx > 2) ? reg2prv(instr_fields[2]) : 0; 
	data -> src2 = (field_idx > 3) ? reg2prv(instr_fields[3]) : 0;
	data -> src3 = -1;

	if (contains_string(instr_fields[0], "vset")){
		data -> type = T_VSETVL;
		if (PRINT_PRV) data -> paraver_code = instr2prv(insn_opcode);
	}else if (instr_fields[0][0]=='v'){
		data -> type = instr_set_vector_type(insn_opcode);

		if (is_subtype(data->type, T_STORE)){
			//change it back to "memory" (general)
			data -> type &= ~T_STORE; 
			data -> type |= T_MEMORY;
			data -> src3 = data -> dst;
			//data -> dst = 0;
		}
		if (is_subtype(data->type, T_ARITH)){
			if (is_subsubsubtype(data->type, T_FUSED)){
				data -> src3 = data -> dst;
			}
		}
		if (PRINT_PRV) data -> paraver_code = instr2prv(insn_opcode);
	}else{
		data -> type = instr_set_scalar_type(insn_opcode); 
		if (PRINT_PRV){
			int opcode = get_bit_field(insn_opcode,6,0);
			int funct3 = get_bit_field(insn_opcode,14,12);
			int	funct6 = get_bit_field(insn_opcode,31,26);
		 	data -> paraver_code = 1010 + (opcode | (funct3<<7) | (funct6<<10));
		}
	}
	for(int i=0; i<field_idx; ++i){
		//printf("%s\n",instr_fields[i]);
		free(instr_fields[i]);
	}
	return data;
}
