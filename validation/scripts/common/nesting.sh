#!/bin/bash
red=$'\033[1;31m'
green=$'\033[1;32m'
nc=$'\033[0m'

pexit(){
	echo $red $@ $nc
	rm -f ${RAVE_CSV_NAME}
	rm -f ${RAVE_PRV_NAME}.{prv,pcf,row}
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


export RAVE_ACCUM_REGIONS=1
export RAVE_CSV_NAME=nesting.csv
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

col_name=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "name") print i }'`
col_nesting=`head -n 1 ${RAVE_CSV_NAME} | awk -F',' '{ for (i=1;i<=NF;i++) if ($i == "nesting(parent)") print i }'`

nline=0
while IFS= read -r line
do
	if [ $nline -gt 1 ]; then
		name=`echo $line | awk -F',' -v id=$col_name '{print $id}'`
		nesting=`echo $line | awk -F',' -v id=$col_nesting '{print $id}'`
		nest=`echo $nesting | sed 's/(.*)//g'`
		parent=`echo $nesting | sed 's/.*(\(.*\))/\1/g'`

		if [[ "$name" == "GLOBAL_REGION" ]]; then 
			expected_parent="N/A"
			expected_nest=0
		elif [[ "$name" == "r1" ]]; then
			expected_parent="GLOBAL_REGION"
			expected_nest=1
		elif [[ "$name" == "r2" ]]; then
			expected_parent="r1"
			expected_nest=2
		elif [[ "$name" == "r3" ]]; then
			expected_parent="r1"
			expected_nest=2
		elif [[ "$name" == "r4" ]]; then
			expected_parent="Multiple"
			expected_nest=3
		elif [[ "$name" == "r5" ]]; then
			expected_parent="r1"
			expected_nest=2
		elif [[ "$name" == "r6" ]]; then
			expected_parent="r5"
			expected_nest=3
		elif [[ "$name" == "r7" ]]; then
			expected_parent="Multiple"
			expected_nest="-1"
		else
			pexit "Unexpected region name $name"
		fi
		assert parent == expected_parent
	 	assert nest == expected_nest	
	fi
	nline=$((nline+1))
done < ${RAVE_CSV_NAME} 

echo $green [TEST OK] API $nc
rm -f ${RAVE_CSV_NAME}
exit 0
