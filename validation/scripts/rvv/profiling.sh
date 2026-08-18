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

#export RAVE_CSV_NAME=lmul_sew_vl.csv
export RAVE_PLAIN_TEXT=1
export RAVE_PROFILE_NAME=profile.csv
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

source_file="profiling.c"

col_elems=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Elems") print i }'`
col_instr=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "it_Instr(avg)") print i }'`
col_its=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Iters(avg)") print i }'`
col_instances=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Instances") print i }'`
col_func=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Function") print i }'`
col_fileline=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "file:line") print i }'`
col_reg=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "vreg_use(avg)") print i }'`
col_vmix=`sed -n '2p' ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "vmix") print i }'`



nline=0
while IFS= read -r line
do
#	------------------- PROFILED LOOPS --------------------
#Elems	avg_instr	Instances	PC	Funct	file:line
#4500	4.0	1		0xf04	main	profiling.c:43
#1800	6.0	1		0xe50	main	profiling.c:24
#200	2.0	1		0xddc	main	profiling.c:9
#-------------------------------------------------------
	if [[ $line != *"Elems"* ]] && [[ $line != *"----"* ]] ; then
		elems=`echo $line | awk  -v id=$col_elems '{print $id}'`
		instr=`echo $line | awk  -v id=$col_instr '{print $id}'`
		its=`echo $line | awk  -v id=$col_its '{print $id}'`
		reg=`echo $line | awk  -v id=$col_reg '{print $id}'`
		instances=`echo $line | awk  -v id=$col_instances '{print $id}'`
		func=`echo $line | awk  -v id=$col_func '{print $id}'`
		fileline=`echo $line | awk  -v id=$col_fileline '{print $id}'`
		vmix=`echo $line | awk  -v id=$col_vmix '{print $id}'`
		
		file=`echo $fileline | cut -d ':' -f1`
		line=`echo $fileline | cut -d ':' -f2`

		if [[ "$func" == "main" ]]; then
			assert file = source_file 
		fi

		if [ $line -eq 56 ]; then
			expected_its=50.0
			assert its = expected_its
			expected_instr="6.0"
			assert instr = expected_instr
			expected_instances=2
			assert instances = expected_instances
			expected_elems=$((129 * 50 * expected_instances))
			assert elems = expected_elems
			expected_reg="0.09"
			assert reg = expected_reg
			expected_vmix="0.50"
			assert vmix = expected_vmix
		elif [ $line -eq 34 ]; then
			expected_its=300.0
			assert its = expected_its
			expected_instr="6.0"
			assert instr = expected_instr
			expected_instances=3
			assert instances = expected_instances
			expected_elems=$((6 * 300 * expected_instances))
			assert elems = expected_elems
			expected_reg="0.00"
			assert reg = expected_reg
			expected_vmix="0.00"
			assert vmix = expected_vmix
		elif [ $line -eq 16 ]; then
			expected_its=200.0
			assert its = expected_its
			expected_instr="2.0"
			assert instr = expected_instr
			expected_instances=4
			assert instances = expected_instances
			expected_elems=$((2 * 200 * expected_instances))
			assert elems = expected_elems
			expected_reg="0.00"
			assert reg = expected_reg
			expected_vmix="0.00"
			assert vmix = expected_vmix
		elif [ $line -ne -1 ]; then
			pexit "Incorrect line number on profiled loops"
		fi	

	fi
	nline=$((nline+1))

done < ${RAVE_PROFILE_NAME}

read_lines=$nline
min_lines=6
assert read_lines -ge min_lines

echo $green [TEST OK] Profiling $nc
rm -f ${RAVE_PROFILE_NAME}
exit 0
