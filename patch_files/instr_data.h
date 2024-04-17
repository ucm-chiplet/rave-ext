enum instr_type{SCALAR, VECTOR, VSETVL};
enum major_type{OTHER, ARITH, MEMORY, MASK};
enum minor_type{NOTYPE, FP, INT, UNIT, STRIDE, INDEX};

struct instr_data{
  uint64_t PC;
	uint32_t paraver_code;
	short src1;
	short src2;
	short src3;
	short dst;
	enum instr_type type;
	enum major_type majortype;
	enum major_type minortype;
};
typedef struct instr_data instr_data;

struct qemu_event{
	int event;
	int value;
};
typedef struct qemu_event qemu_event;

static instr_data * scalar_empty_struct;

extern char contains_string(char * str, const char * find);


static char* majornames[] = {"OTHER", "ARITH", "MEMORY", "MASK"};
static char* minornames[] = {"NOTYPE", "FP", "INT", "UNIT", "STRIDE", "INDEX"};

#define MAJOR_LOAD 0b0000111
#define MAJOR_STORE 0b0100111
#define MAJOR_ARITH 0b1010111
#define get_bit_field(insn_opcode, high, low) ((insn_opcode >> low) & ((1<<(high-low+1))-1))
void instr_set_type(uint32_t insn_opcode, enum major_type *majortype, enum minor_type *minortype){
				int opcode = get_bit_field(insn_opcode,6,0); 
				*majortype = OTHER;
				*minortype = NOTYPE;
				int funct6,funct3,mop,vs1;
				switch (opcode){
								case MAJOR_LOAD:
												*majortype = MEMORY;
#ifdef EPI_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0 || mop == 4) *minortype = UNIT;
												else if (mop == 2 || mop == 6) *minortype = STRIDE;
												else if (mop == 3 || mop == 7) *minortype = INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0) *minortype = UNIT;
												else if (mop == 2) *minortype = STRIDE;
												else if (mop == 1 || mop == 3) *minortype = INDEX; 
#endif
												break;
								case MAJOR_STORE:
												*majortype = MEMORY;
#ifdef EPI_07
												mop = get_bit_field(insn_opcode,28,26);
												if (mop == 0) *minortype = UNIT;
												else if (mop == 2) *minortype = STRIDE;
												else if (mop == 3 || mop == 7) *minortype = INDEX; 
#else
												mop = get_bit_field(insn_opcode,27,26);
												if (mop == 0) *minortype = UNIT;
												else if (mop == 2) *minortype = STRIDE;
												else if (mop == 1 || mop == 3) *minortype = INDEX; 
#endif
												break;
								case MAJOR_ARITH:
												funct3 = get_bit_field(insn_opcode,14,12);
												funct6 = get_bit_field(insn_opcode,31,26);
												if (funct3 == 1 || funct3 == 5){ // OPFVV, OPFVF
																if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask gen
																				*majortype = MASK;
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
																				*majortype = ARITH;
																				*minortype = FP;
																}
												}else if (funct3 == 0 || funct3 == 3 || funct3 == 4){ //OPIVV, OPIVI, OPIVX
																if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask gen
																				*majortype = MASK;
																}else if (funct6 != 0b001100 && //vrgather
																					funct6 != 0b001110 && //slideup, gatherei16 in 1.0
																					funct6 != 0b001111 && //slidedown
																					funct6 != 0b010111){ //vmerge/vmv
																				*majortype = ARITH;
																				*minortype = INT;
																}
												}else if (funct3 == 2 || funct3 == 6){ //OPMVV, OPMVX,
#ifdef EPI_07
																if (funct6 == 0b010110){ //VMUNARY0
																	vs1 = get_bit_field(insn_opcode,19,15);
																	if (vs1 >= 0b00001 && vs1 <= 0b00011){
																				*majortype = MASK;
																	}
#else
																if (funct6 == 0b010100){ //VMUNARY0
																	vs1 = get_bit_field(insn_opcode,19,15);
																	if (vs1 >= 0b00001 && vs1 <= 0b00011){
																				*majortype = MASK;
																	}
#endif
																}else if (funct6 >= 0b011000 && funct6 <= 0b011111){ //mask arith
																				*majortype = MASK;
#ifdef EPI_07
																}else if (funct6 != 0b001101 && //vmv.s.x
																					funct6 != 0b001110 && //slide1up
																					funct6 != 0b001111 && //slide1down
																					funct6 != 0b010100 && //popc
																					funct6 != 0b010101 && //vmfirst
																					funct6 != 0b010111){ //vmcompress
#else
																}else if (funct6 != 0b010000 && //vmv.s.x, vmv.x.s, vpopc, vfirst
																					funct6 != 0b001110 && //slide1up
																					funct6 != 0b001111 && //slide1down
																					funct6 != 0b010010 && //vzext
																					funct6 != 0b010111){ //vmcompress
#endif
																				*majortype = ARITH;
																				*minortype = INT;
																}
												}
												break;
								default:
												break;
				}
}


instr_data * fill_instr_struct(uint64_t pc, char * instr, uint32_t insn_opcode){
	int offsets[8]; //code, dst, src1, src2
	int idx=0;
	char looking=1;
	//Look for fields in char* instr
	for(int i=0; instr[i]!='\0'; ++i){
		char c = instr[i];
		if (c==',' || c==' ' || c=='"' || c=='(' || c==')'){
			instr[i]='\0';
			looking=1;
		}else if (looking){
			looking=0;
			offsets[idx++] = i;
		}
	}
	//Fill fields in struct
	instr_data * data = (instr_data*)malloc(sizeof(instr_data));
	
	data -> PC = pc;
	data -> dst = (idx > 2) ? reg2prv(&instr[offsets[2]]) : 0;
	data -> src1 = (idx > 3) ? reg2prv(&instr[offsets[3]]) : 0; 
	data -> src2 = (idx > 4) ? reg2prv(&instr[offsets[4]]) : 0;

	if (contains_string(&instr[offsets[1]], "vset")){
		data -> type = VSETVL;
		if (PRINT_PRV) data -> paraver_code = instr2prv(&instr[offsets[1]]);
	}else if (instr[offsets[1]]=='v'){
		data -> type = VECTOR;
		instr_set_type(insn_opcode, &data->majortype, &data->minortype);
		//if (data->majortype==OTHER) printf("%s\t%s\t%s\n",majornames[data->majortype], minornames[data->minortype],&instr[offsets[1]]);
		if (PRINT_PRV) data -> paraver_code = instr2prv(&instr[offsets[1]]);
	}else{
		data -> type = SCALAR;
		if (PRINT_PRV){
			int opcode = get_bit_field(insn_opcode,6,0);
			int funct3 = get_bit_field(insn_opcode,14,12);
			int	funct6 = get_bit_field(insn_opcode,31,26);
		 	data -> paraver_code = 1000 + (opcode | (funct3<<7) | (funct6<<10));
		}
	}
	return data;
}
