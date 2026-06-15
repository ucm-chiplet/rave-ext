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
	op1=${!1}
	op=$2
	op2=${!3}
	if ! [ $op1 $op $op2 ]; then
		pexit "$1 ($op1) not $op $3 ($op2)"
	fi
}


if [ $# -lt 2 ]; then
	pexit "Usage: $0 rave binary"
fi

RAVE=$1
BIN=$2

export RAVE_CSV_NAME=lmul_sew_vl.csv
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

col_name=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "name") print i }'`
col_vec_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "vec_instr") print i}'`
col_v_e8_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e8_instr") print i}'`
col_v_e16_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e16_instr") print i}'`
col_v_e32_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e32_instr") print i}'`
col_v_e64_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e64_instr") print i}'`
col_v_e8_elems=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e8_elems") print i}'`
col_v_e16_elems=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e16_elems") print i}'`
col_v_e32_elems=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e32_elems") print i}'`
col_v_e64_elems=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "v_e64_elems") print i}'`

nline=0
while IFS= read -r line
do
	if [ $nline -gt 0 ]; then
		name=`echo $line | awk -F',' -v id=$col_name '{print $id}'`
		lmul=`echo $name | cut -d '_' -f1`
		sew=`echo $name | cut -d '_' -f2`
		vl=`echo $name | cut -d '_' -f3`
		vec_instr=`echo $line | awk -F',' -v id=$col_vec_instr '{print $id}'`
		if [ "$sew" = "e8" ]; then
			v_e8_instr=`echo $line | awk -F',' -v id=$col_v_e8_instr '{print $id}'`
			assert vec_instr -eq v_e8_instr
			v_e8_elems=`echo $line | awk -F',' -v id=$col_v_e8_elems '{print $id}'`
			avgvl=`echo "scale=2; $v_e8_elems / $v_e8_instr" | bc`
			assert vl = avgvl
		fi
		if [ "$sew" = "e16" ]; then
			v_e16_instr=`echo $line | awk -F',' -v id=$col_v_e16_instr '{print $id}'`
			assert vec_instr -eq v_e16_instr
			v_e16_elems=`echo $line | awk -F',' -v id=$col_v_e16_elems '{print $id}'`
			avgvl=`echo "scale=2; $v_e16_elems / $v_e16_instr" | bc`
			assert vl = avgvl
		fi
		if [ "$sew" = "e32" ]; then
			v_e32_instr=`echo $line | awk -F',' -v id=$col_v_e32_instr '{print $id}'`
			assert vec_instr -eq v_e32_instr
			v_e32_elems=`echo $line | awk -F',' -v id=$col_v_e32_elems '{print $id}'`
			avgvl=`echo "scale=2; $v_e32_elems / $v_e32_instr" | bc`
			assert vl = avgvl
		fi
		if [ "$sew" = "e64" ]; then
			v_e64_instr=`echo $line | awk -F',' -v id=$col_v_e64_instr '{print $id}'`
			assert vec_instr -eq v_e64_instr
			v_e64_elems=`echo $line | awk -F',' -v id=$col_v_e64_elems '{print $id}'`
			avgvl=`echo "scale=2; $v_e64_elems / $v_e64_instr" | bc`
			assert vl = avgvl
		fi
	fi
	nline=$((nline+1))
done < ${RAVE_CSV_NAME} 

read_lines=$nline
min_lines=3
assert read_lines -ge min_lines

echo $green [TEST OK] LMUL SEW VL $nc
rm -f ${RAVE_CSV_NAME}
exit 0
