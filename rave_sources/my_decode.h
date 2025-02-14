int MyDissasembler(char * instrbuff, int instr){
const char * vregs[]={"v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "v18", "v19", "v20", "v21", "v22", "v23", "v24", "v25", "v26", "v27", "v28", "v29", "v30", "v31"};
const char * iregs[]={"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};
const char * fregs[]={"ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7", "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5", "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7", "fs8", "fs9", "fs10", "fs11", "ft8", "ft9", "ft10", "ft11"};
int size=0;
#define OPV 87
#define LOADFP 7
#define STOREFP 39
#define AMO 47
unsigned int rs1 = (instr>>15)&0x1F;
unsigned int rs2 = (instr>>20)&0x1F;
unsigned int dst = (instr>>7)&0x1F;
unsigned int major = (instr)&0x7F;
unsigned int masked = (instr>>25)&0x01;
unsigned int mop=(instr>>26)&0x7;
switch(major){
	case LOADFP:
		switch(mop){
			case 0: 
				size = sprintf(instrbuff, "%08x vle.v %s, (%s)", instr, vregs[dst], iregs[rs1]); break;
			case 2: 
				size = sprintf(instrbuff, "%08x vlse.v %s, (%s), %s", instr, vregs[dst], iregs[rs1], iregs[rs2]); break;
			case 3: 
				size = sprintf(instrbuff, "%08x vlxe.v %s, (%s), %s", instr, vregs[dst], iregs[rs1], vregs[rs2]); break;
			default: break;
		}
		break;
	case STOREFP:
		switch(mop){
			case 0: 
				size = sprintf(instrbuff, "%08x vse.v %s, (%s)", instr, vregs[dst], iregs[rs1]); break;
			case 2: 
				size = sprintf(instrbuff, "%08x vsse.v %s, (%s), %s", instr, vregs[dst], iregs[rs1], iregs[rs2]); break;
			case 3: 
				size = sprintf(instrbuff, "%08x vsxe.v %s, (%s), %s", instr, vregs[dst], iregs[rs1], vregs[rs2]); break;
			default: break;
		}
		break;
		break;
	case AMO:
		break;
	case OPV:
		if ((((instr>>25) & 0x7F)==64) && (((instr>>12)&0x7)==7)){ //is vsetvl
			size = sprintf(instrbuff, "%08x vsetvl %s, %s, %s", instr, iregs[dst], iregs[rs1], iregs[rs2]);
			break;
		}else if ((((instr>>31) & 0x1)==0) && (((instr>>12)&0x7)==7)){ //is vsetvli
			unsigned int sew = 1 << (((instr>>22)&0x7)+3);
			unsigned int lmul = (((instr>>20)&0x3)+1);
			size = sprintf(instrbuff, "%08x vsetvli %s, %s, e%d, m%d", instr, iregs[dst], iregs[rs1], sew, lmul);
			break;
		}
		unsigned int funct6=(instr>>26)&0x3F;
		unsigned int funct3=(instr>>12)&0x7;
		switch(funct6){
			case 0:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vadd.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vadd.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredsum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfadd.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 1:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredand.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfredsum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 2:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredor.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfsub.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 3:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vrsub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vrsub.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredxor.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfredosum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 4:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vminu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vminu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredminu.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmin.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmin.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 5:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmin.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmin.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredmin.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfredmin.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 6:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmaxu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmaxu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredmaxu.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmax.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmax.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 7:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmax.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmax.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vredmax.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfredmax.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 8:
				switch(funct3){
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfsgnj.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfsgnj.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 9:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vand.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vand.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vand.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfsgnjn.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfsgnjn.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 10:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vor.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vor.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vor.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfsgnjx.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfsgnjx.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 11:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vxor.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vxor.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vxor.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 12:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vrgather.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vrgather.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vrgather.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmv.f.s. %s, %s", instr, fregs[dst], vregs[rs2]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 13:
				switch(funct3){
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmv.s.x. %s, %s", instr, vregs[dst], iregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmv.s.f. %s, %s", instr, vregs[dst], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 14:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vslideup.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vslideup.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vslide1up.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 15:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vslidedown.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vslidedown.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vslide1down.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 16:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vadc.vvm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vadc.vxm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vadc.vim %s, %s, %d, v0", instr, vregs[dst], vregs[rs2], rs1); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 17:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmadc.vvm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmadc.vxm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmadc.vim %s, %s, %d, v0", instr, vregs[dst], vregs[rs2], rs1); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 18:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsbc.vvm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsbc.vxm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 19:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmsbc.vvm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsbc.vxm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 20:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmpopc.m %s, %s", instr, iregs[dst], vregs[rs2]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 21:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmfirst.m %s, %s", instr, iregs[dst], vregs[rs2]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 22:
				switch(funct3){
					case 2: //OPMVV
						switch(rs1){
							case 1:
								size = sprintf(instrbuff, "%08x vmsbf.m %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 2:
								size = sprintf(instrbuff, "%08x vmsof.m %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 3:
								size = sprintf(instrbuff, "%08x vmsif.m %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 16:
								size = sprintf(instrbuff, "%08x viota.m %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 17:
								size = sprintf(instrbuff, "%08x vid.v %s", instr, vregs[dst]); break;
							default:
							 	size = sprintf(instrbuff, "illegal"); break;
						}break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 23:
				switch(funct3){
					case 0: //OPIVV
						if (!masked) {size = sprintf(instrbuff, "%08x vmv.v.v %s, %s", instr, vregs[dst], vregs[rs1]); break;}
						else {size = sprintf(instrbuff, "%08x vmerge.vvm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;}
					case 4: //OPIVX
						if (!masked) {size = sprintf(instrbuff, "%08x vmv.v.x %s, %s", instr, vregs[dst], iregs[rs1]); break;}
						else {size = sprintf(instrbuff, "%08x vmerge.vxm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;}
					case 3: //OPIVI
						if (!masked) {size = sprintf(instrbuff, "%08x vmv.v.i %s, %d", instr, vregs[dst], rs1); break;}
						else {size = sprintf(instrbuff, "%08x vmerge.vim %s, %s, %d, v0", instr, vregs[dst], vregs[rs2], rs1); break;}
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vcompress.vm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						if (!masked) {size = sprintf(instrbuff, "%08x vfmv.v.f %s, %s", instr, vregs[dst], fregs[rs1]); break;}
						else {size = sprintf(instrbuff, "%08x vfmerge.vfm %s, %s, %s, v0", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;}
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 24:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmseq.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmseq.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmseq.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmandnot.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vmfeq.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmfeq.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 25:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmsne.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsne.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmsne.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmand.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vmfle.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmfle.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 26:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmsltu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsltu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmor.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vmford.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmford.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 27:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmslt.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmslt.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmxor.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vmflt.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmflt.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 28:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmsleu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsleu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmsleu.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmornot.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vmfne.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmfne.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 29:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vmsle.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsle.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmsle.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmnand.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmfgt.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 30:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsgtu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmsgtu.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmnor.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 31:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vmsgt.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vmsgt.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmxnor.mm %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vmfge.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 32:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsaddu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsaddu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vsaddu.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vdivu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vdivu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfdiv.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfdiv.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 33:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsadd.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vsadd.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vdiv.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vdiv.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfrdiv.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 34:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vssubu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vssubu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vremu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vremu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						switch(rs1){
							case 0:
								size = sprintf(instrbuff, "%08x vfcvt.xu.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 1:
								size = sprintf(instrbuff, "%08x vfcvt.x.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 2:
								size = sprintf(instrbuff, "%08x vfcvt.f.xu.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 3:
								size = sprintf(instrbuff, "%08x vfcvt.f.x.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 8:
								size = sprintf(instrbuff, "%08x vfwcvt.xu.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 9:
								size = sprintf(instrbuff, "%08x vfwcvt.x.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 10:
								size = sprintf(instrbuff, "%08x vfwcvt.f.xu.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 11:
								size = sprintf(instrbuff, "%08x vfwcvt.f.x.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 12:
								size = sprintf(instrbuff, "%08x vfwcvt.f.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 16:
								size = sprintf(instrbuff, "%08x vfncvt.xu.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 17:
								size = sprintf(instrbuff, "%08x vfncvt.x.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 18:
								size = sprintf(instrbuff, "%08x vfncvt.f.xu.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 19:
								size = sprintf(instrbuff, "%08x vfncvt.f.x.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 20:
								size = sprintf(instrbuff, "%08x vfncvt.f.f.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							default:
							 	size = sprintf(instrbuff, "illegal"); break;
						}break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 35:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vssub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vssub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vrem.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vrem.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						switch(rs1){
							case 0:
								size = sprintf(instrbuff, "%08x vfsqrt.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							case 16:
								size = sprintf(instrbuff, "%08x vfclass.v %s, %s", instr, vregs[dst], vregs[rs2]); break;
							default:
							 	size = sprintf(instrbuff, "illegal"); break;
						}break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 36:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vaadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vaadd.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vaadd.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmulhu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmulhu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmul.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmul.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 37:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsll.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsll.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vsll.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmul.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmul.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 38:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vasub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vasub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmulhsu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmulhsu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 39:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsmul.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsmul.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmulh.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmulh.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfrsub.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 40:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsrl.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsrl.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vsrl.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmadd.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 41:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vsra.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vsra.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vsra.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmadd.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfnmadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfnmadd.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 42:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vssrl.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vssrl.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vssrl.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmsub.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 43:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vssra.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vssra.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vssra.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vnmsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vnmsub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfnmsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfnmsub.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 44:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vnsrl.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vnsrl.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vnsrl.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmacc.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 45:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vnsra.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vnsra.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vnsra.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vmacc.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfnmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfnmacc.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 46:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vnclipu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vnclipu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vnclipu.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfmsac.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfmsac.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 47:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vnclip.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vnclip.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 3: //OPIVI
						size = sprintf(instrbuff, "%08x vnclip.vi %s, %s, %d", instr, vregs[dst], vregs[rs2], rs1); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vnmsac.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vnmsac.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfnmsac.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfnmsac.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 48:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vwredsumu.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwaddu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwaddu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwadd.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 49:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vwredsum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwadd.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwadd.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwredsum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 50:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwsubu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwsubu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwsub.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 51:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwsub.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwsub.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwredosum.vs %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 52:
				switch(funct3){
					case 2: //OPMVV
					case 6: //OPMVX
					case 1: //OPFVV
					case 5: //OPFVF
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 53:
				switch(funct3){
					case 2: //OPMVV
					case 6: //OPMVX
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 54:
				switch(funct3){
					case 2: //OPMVV
					case 6: //OPMVX
					case 1: //OPFVV
					case 5: //OPFVF
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 55:
				switch(funct3){
					case 2: //OPMVV
					case 6: //OPMVX
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 56:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vdotu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmulu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmulu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwmul.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwmul.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 57:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vdot.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfdot.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 58:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmulsu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmulsu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 59:
				switch(funct3){
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmul.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmul.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 60:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vwsmaccu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vwsmaccu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmaccu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmaccu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwmacc.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 61:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vwsmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vwsmacc.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmacc.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwnmacc.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwnmacc.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 62:
				switch(funct3){
					case 0: //OPIVV
						size = sprintf(instrbuff, "%08x vwsmaccsu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vwsmaccsu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 2: //OPMVV
						size = sprintf(instrbuff, "%08x vwmaccsu.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmaccsu.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwmsac.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwmsac.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			case 63:
				switch(funct3){
					case 4: //OPIVX
						size = sprintf(instrbuff, "%08x vwsmaccus.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 6: //OPMVX
						size = sprintf(instrbuff, "%08x vwmaccus.vx %s, %s, %s", instr, vregs[dst], vregs[rs2], iregs[rs1]); break;
					case 1: //OPFVV
						size = sprintf(instrbuff, "%08x vfwnmsac.vv %s, %s, %s", instr, vregs[dst], vregs[rs2], vregs[rs1]); break;
					case 5: //OPFVF
						size = sprintf(instrbuff, "%08x vfwnmsac.vf %s, %s, %s", instr, vregs[dst], vregs[rs2], fregs[rs1]); break;
					default:
						size = sprintf(instrbuff, "illegal"); break;
				} break;
			default:
				size = sprintf(instrbuff, "illegal"); break;
				break;
		}
		break;
	default:
		size = sprintf(instrbuff, "illegal"); break;
		break;
}
return size;
}
