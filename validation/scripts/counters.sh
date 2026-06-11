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
col_moved_bytes_s=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "moved_bytes_s") print i}'`
col_flops_s=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "flops_s") print i}'`
col_scalar_instr=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "scalar_instr") print i}'`

nline=0
while IFS= read -r line
do
	if [ $nline -gt 1 ]; then
		name=`echo $line | awk -F',' -v id=$col_name '{print $id}'`
		instr=`echo $name | cut -d '_' -f1`
		flops=`echo $name | cut -d '_' -f2`
		bytes=`echo $name | cut -d '_' -f3`

		moved_bytes_s=`echo $line | awk -F',' -v id=$col_moved_bytes_s '{print $id}'`
		flops_s=`echo $line | awk -F',' -v id=$col_flops_s '{print $id}'`
		scalar_instr=`echo $line | awk -F',' -v id=$col_scalar_instr '{print $id}'`

		assert moved_bytes_s -eq bytes
		assert flops_s -eq flops
		max_instr=$((instr+2))
		min_instr=$((instr-2))
		assert scalar_instr -le max_instr
		assert scalar_instr -ge min_instr
	fi
	nline=$((nline+1))
done < ${RAVE_CSV_NAME} 

read_lines=$nline
min_lines=3
assert read_lines -ge min_lines

echo $green [TEST OK] Counters $nc
rm -f ${RAVE_CSV_NAME}
exit 0
