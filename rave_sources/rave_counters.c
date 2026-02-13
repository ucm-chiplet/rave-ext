#define SEWS 4
struct rave_counters{
				double scalar_instr;
				double vsetvl_instr;
				double vector_instr[SEWS];
				double velem[SEWS];

				//Memory
				double velem_mem[SEWS];
				double vunit_instr[SEWS];
				double vstride_instr[SEWS];
				double agg_strides[SEWS];
				double vidx_instr[SEWS];
				double vspill_instr[SEWS];

				//Arith
				double velem_arith[SEWS];
				double vfp_instr[SEWS];
				double vint_instr[SEWS];

				//Reductions
				double velem_reductions[SEWS];
				double vfp_reductions[SEWS];
				double vint_reductions[SEWS];

				//Masks
				double velem_mask[SEWS];
				double vmask_instr[SEWS];

				//General
				double moved_bytes_s;
				double moved_bytes_v;
				double scalarflops;
				double vectorflops;
};
typedef struct rave_counters rave_counters;

/*
static double get_tot_instr(rave_counters * c){
			double totinstr = c->scalar_instr + c->vsetvl_instr;
			for(int s=0; s<SEWS; ++s) totinstr += c->vector_instr[s];
			return totinstr;
}
*/

static void reset_counters(rave_counters * c){
	double * ptr = (double *)c;
	for(int i=0; i<sizeof(rave_counters)/sizeof(double); ++i){
		ptr[i]=0.0;
	}
}

//c1 = c2
static void copy_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c]; 
	}
}

//c1 += c2;
static void add_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] += c2_ptr[c]; 
	}
}

#if 0
//c1 = moving_avg(c1,c2)
static void avg_counters(rave_counters * c1, rave_counters * c2, int n){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] += (c2_ptr[c]-c1_ptr[c])/n;
	}
}
#endif
// c1 = c2*mult
static void mul_counters(rave_counters * c1, rave_counters * c2, double mult){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c] * mult;
	}
}

//c1 = c2-c1
static void update_counters(rave_counters * c1, rave_counters * c2){
	double * c1_ptr = (double *)c1;
	double * c2_ptr = (double *)c2;
	for(int c=0; c<sizeof(rave_counters)/sizeof(double); ++c){
		c1_ptr[c] = c2_ptr[c] - c1_ptr[c]; 
	}
}




#define PERCENTAGE(fd,x,y,fin)\
	if (x>0) {\
		P_PERCENTAGE(fd, " (%.2f %%)", ((y)==0?0:(100.0*(x))/(y)));\
	}\
	fprintf(fd,"%c",fin);

static void print_counters_human(FILE * fd, rave_counters * counters){
	double scalinstr = counters->scalar_instr + counters->vsetvl_instr;
	double vecinstr = 0;
	for(int s=0; s<SEWS; ++s) vecinstr += counters->vector_instr[s];
	double totinstr = scalinstr + vecinstr; 

	//Others...
	double totbytes = counters->moved_bytes_s + counters->moved_bytes_v;
	int level=ic.prev_nest;
	indent(fd,++level,0); fprintf(fd,"Moved bytes: "); P_NUMBER(fd, "%.0f\n", totbytes);
	indent(fd,++level,0); fprintf(fd,"Scalar: "); P_NUMBER(fd, "%.0f", counters->moved_bytes_s); PERCENTAGE(fd,counters->moved_bytes_s,totbytes,'\n');
	indent(fd,  level,1); fprintf(fd,"Vector: "); P_NUMBER(fd, "%.0f", counters->moved_bytes_v); PERCENTAGE(fd,counters->moved_bytes_v,totbytes,'\n');

	double totflops = counters->scalarflops + counters->vectorflops;
	indent(fd,--level,0); fprintf(fd,"FLOPs: "); P_NUMBER(fd, "%.0f\n", totflops);
	indent(fd,++level,0); fprintf(fd,"Scalar: "); P_NUMBER(fd, "%.0f", counters->scalarflops); PERCENTAGE(fd,counters->scalarflops,totflops,'\n');
	indent(fd,  level,1); fprintf(fd,"Vector: "); P_NUMBER(fd, "%.0f", counters->vectorflops); PERCENTAGE(fd,counters->vectorflops,totflops,'\n');

	//Print general counters
	indent(fd,--level,1); fprintf(fd,"Instructions: "); P_NUMBER(fd, "%.0f\n", totinstr);
	indent(fd,++level,0); fprintf(fd,"Scalar instr: "); P_NUMBER(fd, "%.0f", counters->scalar_instr); PERCENTAGE(fd,counters->scalar_instr, totinstr,'\n'); 
	indent(fd,  level,0); fprintf(fd,"Vsetvl instr: "); P_NUMBER(fd, "%.0f", counters->vsetvl_instr); PERCENTAGE(fd,counters->vsetvl_instr, totinstr,'\n');
	indent(fd,  level,1); fprintf(fd,"Vector instr: "); P_NUMBER(fd, "%.0f", vecinstr); PERCENTAGE(fd,vecinstr, totinstr,'\n'); 

#if 1
	++level;
	//Print SEW-specific counters (vec)
	if (!vecinstr) return;
	for(int s=0; s<SEWS; ++s){
		indent(fd, level, (s==SEWS-1)); 
		fprintf(fd,"SEW %d vector instr: ", 1<<(s+3)); P_NUMBER(fd, "%.0f", counters->vector_instr[s]); PERCENTAGE(fd,counters->vector_instr[s], vecinstr, counters->vector_instr[s]>0?' ':'\n');
		if (counters->vector_instr[s]>0){
			fprintf(fd, " [avg VL: "); P_VL(fd, "%.2f",counters->velem[s] / counters->vector_instr[s]); fprintf(fd," elements]\n");
			double  totvmem		= counters->vunit_instr[s] + counters->vstride_instr[s] + counters->vidx_instr[s] + counters->vspill_instr[s];
			double  totvarith	= counters->vfp_instr[s] + counters->vint_instr[s];
			double  totvred	= counters->vfp_reductions[s] + counters->vint_reductions[s];
			double  totvother	= counters->vector_instr[s] - totvmem - totvarith - totvred - counters->vmask_instr[s];

			indent(fd,++level,0); fprintf(fd,"Arith: "); P_NUMBER(fd,"%.0f",totvarith); PERCENTAGE(fd,totvarith, counters->vector_instr[s],totvarith>0?' ':'\n');
			if (totvarith>0){
			 	fprintf(fd, " [avg VL: "); P_VL(fd, "%.2f", counters->velem_arith[s] / totvarith); fprintf(fd," elements]\n");
				indent(fd,++level,0); fprintf(fd,"FP: "); P_NUMBER(fd,"%.0f", counters->vfp_instr[s]); PERCENTAGE(fd,counters->vfp_instr[s], totvarith,'\n');
				indent(fd,  level,1); fprintf(fd,"INT: "); P_NUMBER(fd,"%.0f", counters->vint_instr[s]); PERCENTAGE(fd,counters->vint_instr[s], totvarith,'\n');
				--level;
			}
			indent(fd,level,0); fprintf(fd,"Reduction: "); P_NUMBER(fd,"%.0f",totvred); PERCENTAGE(fd,totvred, counters->vector_instr[s],totvred>0?' ':'\n');
			if (totvred>0){
			 	fprintf(fd, " [avg VL: "); P_VL(fd, "%.2f", counters->velem_reductions[s] / totvred); fprintf(fd," elements]\n");
				indent(fd,++level,0); fprintf(fd,"FP: "); P_NUMBER(fd,"%.0f", counters->vfp_reductions[s]); PERCENTAGE(fd,counters->vfp_reductions[s], totvred,'\n');
				indent(fd,  level,1); fprintf(fd,"INT: "); P_NUMBER(fd,"%.0f", counters->vint_reductions[s]); PERCENTAGE(fd,counters->vint_reductions[s], totvred,'\n');
				--level;
			}

			indent(fd,level,0); fprintf(fd,"Memory: "); P_NUMBER(fd,"%.0f", totvmem); PERCENTAGE(fd,totvmem, counters->vector_instr[s], totvmem>0?' ':'\n');
			if (totvmem>0){
			 	fprintf(fd, " [avg VL: "); P_VL(fd,"%.2f",counters->velem_mem[s] / totvmem); fprintf(fd," elements]\n");
				indent(fd,++level,0); fprintf(fd,"unit: "); P_NUMBER(fd,"%.0f", counters->vunit_instr[s]); PERCENTAGE(fd,counters->vunit_instr[s], totvmem,'\n');
				indent(fd,  level,0); fprintf(fd,"strided: "); P_NUMBER(fd,"%.0f", counters->vstride_instr[s]); PERCENTAGE(fd,counters->vstride_instr[s], totvmem,'\n');
				if (counters->vstride_instr[s] > 0){
					indent(fd,++level,1); fprintf(fd,"Avg. Stride (B): "); P_NUMBER(fd,"%.2f\n", counters->agg_strides[s] / counters->vstride_instr[s]);
					--level;
				}
				indent(fd,level,0); fprintf(fd,"indexed: "); P_NUMBER(fd,"%.0f", counters->vidx_instr[s]); PERCENTAGE(fd,counters->vidx_instr[s], totvmem,'\n');
				indent(fd,level,1); fprintf(fd,"whole-register: "); P_NUMBER(fd,"%.0f", counters->vspill_instr[s]); PERCENTAGE(fd,counters->vspill_instr[s], totvmem,'\n');
				--level;
			}

			indent(fd,level,0);	fprintf(fd,"Mask: "); P_NUMBER(fd,"%.0f", counters->vmask_instr[s]); PERCENTAGE(fd,counters->vmask_instr[s], counters->vector_instr[s], counters->vmask_instr[s]>0?' ':'\n');
			if (counters->vmask_instr[s]>0) {
				fprintf(fd, " [avg VL: "); P_VL(fd, "%.2f", counters->velem_mask[s] / counters->vmask_instr[s]); fprintf(fd," elements]\n");
			}

			indent(fd,level,1); fprintf(fd,"Other: "); P_NUMBER(fd,"%.0f", totvother); PERCENTAGE(fd,totvother, counters->vector_instr[s], totvother>0?' ':'\n');
			if (totvother>0) {
				fprintf(fd, " [avg VL: "); P_VL(fd,"%.2f", (counters->velem[s]-counters->velem_arith[s]-counters->velem_reductions[s]-counters->velem_mem[s]-counters->velem_mask[s])/totvother); fprintf(fd," elements]\n");
			}
			--level;
		}
	}
#endif
}

static void print_csv_header(FILE * fd){
		fprintf(fd,"moved_bytes_s,moved_bytes_v");
		fprintf(fd,",flops_s,flops_v");
		fprintf(fd,",tot_instr,scalar_instr,vsetvl_instr,vec_instr");
		for(int s=0; s<SEWS; ++s){
		 	int sew=1<<(s+3);	
			fprintf(fd,	",v_e%d_instr,v_e%d_elems", sew,sew);
			fprintf(fd, ",v_e%d_arith,v_e%d_arith_elems,v_e%d_fp,v_e%d_int",sew,sew,sew,sew);
			fprintf(fd, ",v_e%d_reduction,v_e%d_reduction_elems,v_e%d_reduction_fp,v_e%d_reduction_int",sew,sew,sew,sew);
			fprintf(fd, ",v_e%d_mem,v_e%d_mem_elems,v_e%d_memunit,v_e%d_memidx,v_e%d_memstrided,v_e%d_avg_stride,v_e%d_memspill",sew,sew,sew,sew,sew,sew,sew);
			fprintf(fd, ",v_e%d_mask,v_e%d_mask_elems",sew,sew);
			fprintf(fd, ",v_e%d_other,v_e%d_other_elems",sew,sew);
		}
		fprintf(fd,"\n");
}

static void print_counters_csv(FILE * fd, rave_counters * counters){
	double totinstr = counters->scalar_instr + counters->vsetvl_instr;
	double totvec = 0;
	for(int s=0; s<SEWS; ++s) totvec += counters->vector_instr[s];
	totinstr += totvec;

	//Print general counters
	fprintf(fd,",%.0f,%.0f", counters->moved_bytes_s, counters->moved_bytes_v);
	fprintf(fd,",%.0f,%.0f", counters->scalarflops, counters->vectorflops); 
	fprintf(fd,",%.0f,%.0f,%.0f,%.0f", totinstr, counters->scalar_instr, counters->vsetvl_instr, totvec);

	//Print SEW-specific counters (vec)
	for(int s=0; s<SEWS; ++s){
		double  totvmem		= counters->vunit_instr[s] + counters->vstride_instr[s] + counters->vidx_instr[s] + counters->vspill_instr[s];
		double  totvarith	= counters->vfp_instr[s] + counters->vint_instr[s];
		double  totvred	= counters->vfp_reductions[s] + counters->vint_reductions[s];
		double  totvother	= counters->vector_instr[s] - totvmem - totvarith - totvred - counters->vmask_instr[s];
		double strides = (counters->vstride_instr[s] > 0)? counters->agg_strides[s] / counters->vstride_instr[s] : 0;
		double velem_other = counters->velem[s] - counters->velem_arith[s] - counters->velem_reductions[s] - counters->velem_mem[s] - counters->velem_mask[s];

		fprintf(fd,",%.0f,%.0f", counters->vector_instr[s], counters->velem[s]);
		fprintf(fd,",%.0f,%.0f,%.0f,%.0f", totvarith, counters->velem_arith[s], counters->vfp_instr[s], counters->vint_instr[s]);
		fprintf(fd,",%.0f,%.0f,%.0f,%.0f", totvred, counters->velem_reductions[s], counters->vfp_reductions[s], counters->vint_reductions[s]);
		fprintf(fd,",%.0f,%.0f,%.0f,%.0f,%.0f,%.2f,%.0f", totvmem, counters->velem_mem[s], counters->vunit_instr[s], counters->vidx_instr[s], counters->vstride_instr[s], strides, counters->vspill_instr[s]);
		fprintf(fd,",%.0f,%.0f", counters->vmask_instr[s], counters->velem_mask[s]); 
		fprintf(fd,",%.0f,%.0f", totvother, velem_other);
	}
	fprintf(fd,"\n");
}
