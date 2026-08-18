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

assert_null(){
	op1=${!1}
	op=$2
	if ! [ "$op1" $op "" ]; then
		pexit "$1 ($op1) not $op \"\""
	fi
}

if [ $# -lt 2 ]; then
	pexit "Usage: $0 rave binary"
fi

RAVE=$1
BIN=$2

export RAVE_CSV_NAME=api.csv
export RAVE_PRV_NAME=api
$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Binary ended with an error"
fi

# Check CSV
has_erased=`grep region_erased ${RAVE_CSV_NAME}`
has_region1=`grep region1 ${RAVE_CSV_NAME}`
has_notrace=`grep region_notrace ${RAVE_CSV_NAME}`
has_region2=`grep region2 ${RAVE_CSV_NAME}`
has_disabled=`grep region_disabled ${RAVE_CSV_NAME}`
has_region3=`grep region3 ${RAVE_CSV_NAME}`

assert_null has_erased !=
assert_null has_region1 != 
assert_null has_notrace !=  
assert_null has_region2 != 
assert_null has_disabled ==  
assert_null has_region3 != 

#Check PRV:

val_erased=`grep region_erased ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`
val_region1=`grep region1 ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`
val_notrace=`grep region_notrace ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`
val_region2=`grep region2 ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`
val_disabled=`grep region_disabled ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`
val_region3=`grep region3 ${RAVE_PRV_NAME}.pcf | awk '{print $1}'`

assert_null val_erased != 
assert_null val_region1 != 
assert_null val_region2 != 
assert_null val_region3 != 
assert_null val_notrace != 
assert_null val_disabled ==

has_erased=`grep 1000:$val_erased ${RAVE_PRV_NAME}.prv`
has_region1=`grep 1000:$val_region1 ${RAVE_PRV_NAME}.prv`
has_notrace=`grep 1000:$val_notrace ${RAVE_PRV_NAME}.prv`
has_region2=`grep 1000:$val_region2 ${RAVE_PRV_NAME}.prv`
has_disabled=`grep 1000:$val_disabled ${RAVE_PRV_NAME}.prv`
has_region3=`grep 1000:$val_region3 ${RAVE_PRV_NAME}.prv`

assert_null has_earsed == 
assert_null has_region1 != 
assert_null has_region2 != 
assert_null has_region3 != 
#Should add something to notrace to check that no instructions are recorder
assert_null has_notrace !=  

has_13=`grep 13:13 ${RAVE_PRV_NAME}.prv`
has_42=`grep 42:11 ${RAVE_PRV_NAME}.prv`
assert_null has_13 == 
assert_null has_42 != 

pcf_42=`grep forty-two ${RAVE_PRV_NAME}.pcf`
pcf_11=`grep eleven ${RAVE_PRV_NAME}.pcf`

assert_null pcf_42 != 
assert_null pcf_11 != 


echo $green [TEST OK] API $nc
rm -f ${RAVE_CSV_NAME}
rm -f ${RAVE_PRV_NAME}.{prv,pcf,row}
exit 0
