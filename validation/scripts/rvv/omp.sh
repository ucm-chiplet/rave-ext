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
export OMP_NUM_THREADS=4
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

echo $green [TEST OK] OMP $nc
rm -f ${RAVE_CSV_NAME}
exit 0
