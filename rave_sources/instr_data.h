//enum instr_type{SCALAR, VECTOR, VSETVL};
//enum v_major_type{OTHER, ARITH, MEMORY, MASK};
//enum v_minor_type{NOTYPE, FP, INT, UNIT, STRIDE, INDEX};

/* [xxxx][tttt][MMMM][mmmm] */
#define T_NOTYPE 0x0000

#define T_SCALAR 0x0100
#define T_VECTOR 0x0200
#define T_VSETVL 0x0300

#define T_OTHER  0x0000
#define T_ARITH  0x0010
#define T_LOAD   0x0020
#define T_STORE  0x0030
#define T_MASK   0x0040
#define T_MEMORY T_LOAD //Until we report ld/st separately

#define T_FP     0x0001
#define T_INT    0x0002
#define T_UNIT   0x0003
#define T_STRIDE 0x0004
#define T_INDEX  0x0005


#if 1
#define is_type(x,y) (((x^y)&0x0F00)==0)
#define is_subtype(x,y) (((x^y)&0x00F0)==0)
#define is_subsubtype(x,y) (((x^y)&0x000F)==0)
#endif

struct instr_basic_data{
	uint16_t type;
	//enum instr_type type;
	uint32_t instr32;
  uint64_t PC;
};
typedef struct instr_basic_data instr_basic_data;

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

//static instr_data * scalar_empty_struct;

extern char contains_string(char * str, const char * find);


#define MAJOR_LOAD 0b0000111
#define MAJOR_STORE 0b0100111
#define MAJOR_ARITH 0b1010111
#define get_bit_field(insn_opcode, high, low) ((insn_opcode >> low) & ((1<<(high-low+1))-1))
int16_t instr_set_type(uint32_t insn_opcode){
				int16_t type = T_VECTOR; 
				int16_t subtype = T_OTHER;
				int16_t subsubtype = T_NOTYPE;

				int opcode = get_bit_field(insn_opcode,6,0); 
				int funct6,funct3,mop,vs1;
				switch (opcode){
								case MAJOR_LOAD:
												subtype = T_LOAD;
#ifdef EPI_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0 || mop == 4) subsubtype = T_UNIT;
												else if (mop == 2 || mop == 6) subsubtype = T_STRIDE;
												else if (mop == 3 || mop == 7) subsubtype = T_INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0) subsubtype = T_UNIT;
												else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 1 || mop == 3) subsubtype = T_INDEX; 
#endif
												break;
								case MAJOR_STORE:
												subtype = T_STORE;
#ifdef EPI_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0) subsubtype = T_UNIT;
												else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 3 || mop == 7) subsubtype = T_INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0) subsubtype = T_UNIT;
												else if (mop == 2) subsubtype = T_STRIDE;
												else if (mop == 1 || mop == 3) subsubtype = T_INDEX; 
#endif
												break;
								case MAJOR_ARITH:
												funct3 = get_bit_field(insn_opcode,14,12);
												funct6 = get_bit_field(insn_opcode,31,26);
												if (funct3 == 1 || funct3 == 5){ // OPFVV, OPFVF
																if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask gen
																				subtype = T_MASK;
#ifdef EPI_07
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
																				subtype = T_ARITH;
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
																					funct6 != 0b010111){ //vmerge/vmv
																				subtype = T_ARITH;
																				subsubtype = T_INT;
																}
												}else if (funct3 == 2 || funct3 == 6){ //OPMVV, OPMVX,
#ifdef EPI_07
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
#ifdef EPI_07
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
																				subtype = T_ARITH;
																				subsubtype = T_INT;
																}
												}
												break;
								default:
												break;
				}
				return (type | subtype | subsubtype);
}


instr_data * fill_instr_struct(uint64_t pc, char * instr, uint32_t insn_opcode){

	char * instr_fields[8]; //8 is more than enough

	//Look for fields in char* instr 
	int field_start=0;
	#ifdef EPI_07
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
	data -> asm_string = g_strdup_printf("%s", instr); 
	data -> instr32 = insn_opcode;
	
	data -> PC = pc;
	data -> dst =  (field_idx > 1) ? reg2prv(instr_fields[1]) : 0;
	data -> src1 = (field_idx > 2) ? reg2prv(instr_fields[2]) : 0; 
	data -> src2 = (field_idx > 3) ? reg2prv(instr_fields[3]) : 0;
	data -> src3 = 0;

	if (contains_string(instr_fields[0], "vset")){
		data -> type = T_VSETVL;
		if (PRINT_PRV) data -> paraver_code = instr2prv(instr_fields[0]); //Could be simplified
	}else if (instr_fields[0][0]=='v'){
		data -> type = instr_set_type(insn_opcode);

		if (is_subtype(data->type, T_STORE)){
			//change it back to "memory" (general)
			data -> type &= 0x0F0F;
			data -> type |= T_MEMORY;
			data -> src3 = data -> dst;
			data -> dst = 0;
		}
		if (PRINT_PRV) data -> paraver_code = instr2prv(instr_fields[0]);
	}else{
		data -> type = T_SCALAR;
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
