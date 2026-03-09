#!/bin/bash

red=$'\033[1;31m'
green=$'\033[1;32m'
nc=$'\033[0m'

pexit(){
	echo $red $@ $nc
	rm -f ${RAVE_CSV_NAME}
	exit -1
}

assert(){
	op1=$(($1))
	op=$2
	op2=$(($3))
	if ! [ $op1 $op $op2 ]; then
		pexit "$1 ($op1) not $op $3 ($op2)"
	fi
}

if [ $# -lt 2 ]; then
	pexit "Usage: $0 rave binary"
fi

RAVE=$1
BIN=$2

export RAVE_CSV_NAME=instr_class.csv
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

col_vec_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "vec_instr") print i}'`
col_v_e32_arith=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e32_arith") print i}'`
col_v_e32_int=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e32_int") print i}'`
col_v_e64_arith=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e64_arith") print i}'`
col_v_e64_int=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e64_int") print i}'`

col_v_e32_reduction=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_reduction") print i}'`
col_v_e32_reduction_int=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_reduction_int") print i}'`
col_v_e64_reduction=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_reduction") print i}'`
col_v_e64_reduction_int=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_reduction_int") print i}'`

col_v_e32_fp=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_fp") print i}'`
col_v_e64_fp=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_fp") print i}'`

col_v_e32_reduction_fp=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_reduction_fp") print i}'`
col_v_e64_reduction_fp=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_reduction_fp") print i}'`

col_v_e8_other=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_other") print i}'`
col_v_e16_other=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_other") print i}'`
col_v_e32_other=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_other") print i}'`
col_v_e64_other=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_other") print i}'`

col_v_e8_mask=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_mask") print i}'`
col_v_e16_mask=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_mask") print i}'`
col_v_e32_mask=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_mask") print i}'`
col_v_e64_mask=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_mask") print i}'`

col_v_e8_mem=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_mem") print i}'`
col_v_e16_mem=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_mem") print i}'`
col_v_e32_mem=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_mem") print i}'`
col_v_e64_mem=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_mem") print i}'`
col_v_e8_memspill=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_memspill") print i}'`
col_v_e16_memspill=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_memspill") print i}'`
col_v_e32_memspill=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_memspill") print i}'`
col_v_e64_memspill=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_memspill") print i}'`

col_v_e8_memunit=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_memunit") print i}'`
col_v_e16_memunit=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_memunit") print i}'`
col_v_e32_memunit=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_memunit") print i}'`
col_v_e64_memunit=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_memunit") print i}'`

col_v_e8_memstrided=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_memstrided") print i}'`
col_v_e16_memstrided=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_memstrided") print i}'`
col_v_e32_memstrided=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_memstrided") print i}'`
col_v_e64_memstrided=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_memstrided") print i}'`

col_v_e8_memidx=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e8_memidx") print i}'`
col_v_e16_memidx=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e16_memidx") print i}'`
col_v_e32_memidx=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e32_memidx") print i}'`
col_v_e64_memidx=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<NF;i++) if ($i == "v_e64_memidx") print i}'`

nline=0
while IFS= read -r line
do
	if [ $nline -eq 0 ]; then
		col_name=`echo $line | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "name") print i }'`
	else
		name=`echo $line | awk -F',' -v id=$col_name '{print $id}'`
		vec_instr=`echo $line | awk -F',' -v id=$col_vec_instr '{print $id}'`
		if [ "$name" = "Integer_arith" ]; then
			v_e32_arith=`echo $line | awk -F',' -v id=$col_v_e32_arith '{print $id}'`
			v_e32_int=`echo $line | awk -F',' -v id=$col_v_e32_int '{print $id}'`
			v_e64_arith=`echo $line | awk -F',' -v id=$col_v_e64_arith '{print $id}'`
			v_e64_int=`echo $line | awk -F',' -v id=$col_v_e64_int '{print $id}'`
			assert vec_instr -eq v_e32_arith+v_e64_arith
			assert v_e32_int -eq v_e32_arith
			assert v_e64_int -eq v_e64_arith
		elif [ "$name" = "Integer_reduction" ]; then
			v_e32_reduction=`echo $line | awk -F',' -v id=$col_v_e32_reduction '{print $id}'`
			v_e32_reduction_int=`echo $line | awk -F',' -v id=$col_v_e32_reduction_int '{print $id}'`
			v_e64_reduction=`echo $line | awk -F',' -v id=$col_v_e64_reduction '{print $id}'`
			v_e64_reduction_int=`echo $line | awk -F',' -v id=$col_v_e64_reduction_int '{print $id}'`
			assert vec_instr -eq v_e32_reduction+v_e64_reduction
			assert v_e32_reduction_int -eq v_e32_reduction
			assert v_e64_reduction_int -eq v_e64_reduction
		elif [ "$name" = "FP_arith" ]; then
			v_e32_arith=`echo $line | awk -F',' -v id=$col_v_e32_arith '{print $id}'`
			v_e32_fp=`echo $line | awk -F',' -v id=$col_v_e32_fp '{print $id}'`
			v_e64_arith=`echo $line | awk -F',' -v id=$col_v_e64_arith '{print $id}'`
			v_e64_fp=`echo $line | awk -F',' -v id=$col_v_e64_fp '{print $id}'`
			assert vec_instr -eq v_e32_arith+v_e64_arith
			assert v_e32_fp -eq v_e32_arith
			assert v_e64_fp -eq v_e64_arith
		elif [ "$name" = "FP_reductions" ]; then
			v_e32_reduction=`echo $line | awk -F',' -v id=$col_v_e32_reduction '{print $id}'`
			v_e32_reduction_fp=`echo $line | awk -F',' -v id=$col_v_e32_reduction_fp '{print $id}'`
			v_e64_reduction=`echo $line | awk -F',' -v id=$col_v_e64_reduction '{print $id}'`
			v_e64_reduction_fp=`echo $line | awk -F',' -v id=$col_v_e64_reduction_fp '{print $id}'`
			assert vec_instr -eq v_e32_reduction+v_e64_reduction
			assert v_e32_reduction_fp -eq v_e32_reduction
			assert v_e64_reduction_fp -eq v_e64_reduction
		elif [ "$name" = "Other" ]; then
			v_e8_other=`echo $line | awk -F',' -v id=$col_v_e8_other '{print $id}'`
			v_e16_other=`echo $line | awk -F',' -v id=$col_v_e16_other '{print $id}'`
			v_e32_other=`echo $line | awk -F',' -v id=$col_v_e32_other '{print $id}'`
			v_e64_other=`echo $line | awk -F',' -v id=$col_v_e64_other '{print $id}'`
			assert  vec_instr -eq v_e8_other+v_e16_other+v_e32_other+v_e64_other
		elif [ "$name" = "Mask creation" ]; then
			v_e8_mask=`echo $line | awk -F',' -v id=$col_v_e8_mask '{print $id}'`
			v_e16_mask=`echo $line | awk -F',' -v id=$col_v_e16_mask '{print $id}'`
			v_e32_mask=`echo $line | awk -F',' -v id=$col_v_e32_mask '{print $id}'`
			v_e64_mask=`echo $line | awk -F',' -v id=$col_v_e64_mask '{print $id}'`
			assert vec_instr -eq v_e8_mask+v_e16_mask+v_e32_mask+v_e64_mask
		elif [ "$name" = "MEM Spill" ]; then
			v_e8_mem=`echo $line | awk -F',' -v id=$col_v_e8_mem '{print $id}'`
			v_e16_mem=`echo $line | awk -F',' -v id=$col_v_e16_mem '{print $id}'`
			v_e32_mem=`echo $line | awk -F',' -v id=$col_v_e32_mem '{print $id}'`
			v_e64_mem=`echo $line | awk -F',' -v id=$col_v_e64_mem '{print $id}'`
			v_e8_memspill=`echo $line | awk -F',' -v id=$col_v_e8_memspill '{print $id}'`
			v_e16_memspill=`echo $line | awk -F',' -v id=$col_v_e16_memspill '{print $id}'`
			v_e32_memspill=`echo $line | awk -F',' -v id=$col_v_e32_memspill '{print $id}'`
			v_e64_memspill=`echo $line | awk -F',' -v id=$col_v_e64_memspill '{print $id}'`
			assert vec_instr -eq v_e8_mem+v_e16_mem+v_e32_mem+v_e64_mem
			assert v_e8_mem -eq v_e8_memspill
			assert v_e16_mem -eq v_e16_memspill
			assert v_e32_mem -eq v_e32_memspill
			assert v_e64_mem -eq v_e64_memspill
		elif [ "$name" = "MEM Unit-strided" ]; then
			v_e8_mem=`echo $line | awk -F',' -v id=$col_v_e8_mem '{print $id}'`
			v_e16_mem=`echo $line | awk -F',' -v id=$col_v_e16_mem '{print $id}'`
			v_e32_mem=`echo $line | awk -F',' -v id=$col_v_e32_mem '{print $id}'`
			v_e64_mem=`echo $line | awk -F',' -v id=$col_v_e64_mem '{print $id}'`
			v_e8_memunit=`echo $line | awk -F',' -v id=$col_v_e8_memunit '{print $id}'`
			v_e16_memunit=`echo $line | awk -F',' -v id=$col_v_e16_memunit '{print $id}'`
			v_e32_memunit=`echo $line | awk -F',' -v id=$col_v_e32_memunit '{print $id}'`
			v_e64_memunit=`echo $line | awk -F',' -v id=$col_v_e64_memunit '{print $id}'`
			assert vec_instr -eq v_e8_mem+v_e16_mem+v_e32_mem+v_e64_mem
			assert v_e8_mem -eq v_e8_memunit
			assert v_e16_mem -eq v_e16_memunit
			assert v_e32_mem -eq v_e32_memunit
			assert v_e64_mem -eq v_e64_memunit
		elif [ "$name" = "MEM strided" ]; then
			v_e8_mem=`echo $line | awk -F',' -v id=$col_v_e8_mem '{print $id}'`
			v_e16_mem=`echo $line | awk -F',' -v id=$col_v_e16_mem '{print $id}'`
			v_e32_mem=`echo $line | awk -F',' -v id=$col_v_e32_mem '{print $id}'`
			v_e64_mem=`echo $line | awk -F',' -v id=$col_v_e64_mem '{print $id}'`
			v_e8_memstrided=`echo $line | awk -F',' -v id=$col_v_e8_memstrided '{print $id}'`
			v_e16_memstrided=`echo $line | awk -F',' -v id=$col_v_e16_memstrided '{print $id}'`
			v_e32_memstrided=`echo $line | awk -F',' -v id=$col_v_e32_memstrided '{print $id}'`
			v_e64_memstrided=`echo $line | awk -F',' -v id=$col_v_e64_memstrided '{print $id}'`
			assert vec_instr -eq v_e8_mem+v_e16_mem+v_e32_mem+v_e64_mem
			assert v_e8_mem -eq v_e8_memstrided
			assert v_e16_mem -eq v_e16_memstrided
			assert v_e32_mem -eq v_e32_memstrided
			assert v_e64_mem -eq v_e64_memstrided
		elif [ "$name" = "MEM indexed" ]; then
			v_e8_mem=`echo $line | awk -F',' -v id=$col_v_e8_mem '{print $id}'`
			v_e16_mem=`echo $line | awk -F',' -v id=$col_v_e16_mem '{print $id}'`
			v_e32_mem=`echo $line | awk -F',' -v id=$col_v_e32_mem '{print $id}'`
			v_e64_mem=`echo $line | awk -F',' -v id=$col_v_e64_mem '{print $id}'`
			v_e8_memidx=`echo $line | awk -F',' -v id=$col_v_e8_memidx '{print $id}'`
			v_e16_memidx=`echo $line | awk -F',' -v id=$col_v_e16_memidx '{print $id}'`
			v_e32_memidx=`echo $line | awk -F',' -v id=$col_v_e32_memidx '{print $id}'`
			v_e64_memidx=`echo $line | awk -F',' -v id=$col_v_e64_memidx '{print $id}'`
			assert vec_instr -eq v_e8_mem+v_e16_mem+v_e32_mem+v_e64_mem
			assert v_e8_mem -eq v_e8_memidx
			assert v_e16_mem -eq v_e16_memidx
			assert v_e32_mem -eq v_e32_memidx
			assert v_e64_mem -eq v_e64_memidx
		fi
	fi
	nline=$((nline+1))
done < ${RAVE_CSV_NAME}

echo $green [TEST OK] Instruction class $nc
rm -f ${RAVE_CSV_NAME}
exit 0

