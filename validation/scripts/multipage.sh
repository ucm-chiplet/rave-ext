#!/bin/bash

red=$'\033[1;31m'
green=$'\033[1;32m'
nc=$'\033[0m'

pexit(){
	echo $red $@ $nc
	exit -1
}

if [ $# -lt 2 ]; then
	pexit "Usage: $0 rave binary"
fi

RAVE=$1
BIN=$2

$RAVE $BIN
ret=$?
if [ $ret -ne 0 ]; then
	pexit "Failed test"
fi	

echo $green [TEST OK] Multipage access $nc
