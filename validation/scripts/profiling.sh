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
export RAVE_PROFILE_NAME=profile.csv
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

source_file="profiling.c"

col_elems=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Elems") print i }'`
col_instr=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "avg_instr") print i }'`
col_instances=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Instances") print i }'`
col_PC=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "Funct") print i }'`
col_fileline=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "file:line") print i }'`
col_reg=`grep Elems ${RAVE_PROFILE_NAME} | awk  '{ for (i=1;i<=NF;i++) if ($i == "avg_vreg_use") print i }'`

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
		reg=`echo $line | awk  -v id=$col_reg '{print $id}'`
		instances=`echo $line | awk  -v id=$col_instances '{print $id}'`
		PC=`echo $line | awk  -v id=$col_PC '{print $id}'`
		fileline=`echo $line | awk  -v id=$col_fileline '{print $id}'`
		
		file=`echo $fileline | cut -d ':' -f1`
		line=`echo $fileline | cut -d ':' -f2`

		if [[ "$PC" == "main" ]]; then
			assert file = source_file 
		fi

		if [ $line -eq 56 ]; then
			expected_elems=12900
			assert elems = expected_elems
			expected_instr="6.0"
			assert instr = expected_instr
			expected_instances=2
			assert instances = expected_instances
			expected_reg="0.094"
			assert reg = expected_reg
		elif [ $line -eq 34 ]; then
			expected_elems=5400
			assert elems = expected_elems
			expected_instr="6.0"
			assert instr = expected_instr
			expected_instances=3
			assert instances = expected_instances
			expected_reg="0.000"
			assert reg = expected_reg
		elif [ $line -eq 16 ]; then
			expected_elems=800
			assert elems = expected_elems
			expected_instr="2.0"
			assert instr = expected_instr
			expected_instances=4
			assert instances = expected_instances
			expected_reg="0.000"
			assert reg = expected_reg
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
