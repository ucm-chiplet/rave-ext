#!/bin/bash


remove_from_path(){
	export $1=`echo ${!1} | sed "s?\([^:]*\)$2[^:]*\(:\|$\)??g"`
}

append_to_path(){
	export $1=$2:${!1}
}

if [ $# -lt 1 ]; then
	echo RVV not defined, defaulting to 1_0.
	echo Remember you can specify which RVV to use with: 
	echo source environment.sh 1_0
	echo source environment.sh 0_7_1
	export RVV=1_0
else
	export RVV=$1
fi

remove_from_path PATH qemu-rave
remove_from_path PATH llvm-cross
remove_from_path PYTHONPATH interfaces

SCRIPT_DIR="$(cd -- "$(dirname -- "$(readlink -f -- "${BASH_SOURCE[0]}")")" && pwd)"

if [ "$RVV" = "0_7_1" ]; then
	append_to_path PATH ${SCRIPT_DIR}/build/qemu-rave/RVV-0_7_1/bin
	append_to_path PATH ${SCRIPT_DIR}/build/llvm-cross/llvm-EPI-0.7-development-toolchain-cross/bin/
elif [ "$RVV" = "1_0" ]; then
	append_to_path PATH ${SCRIPT_DIR}/build/qemu-rave/RVV-1_0/bin
	append_to_path PATH ${SCRIPT_DIR}/build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/
else
	echo RVV needs to be either 0_7_1 or 1_0
fi

export RAVE_INCLUDE=${SCRIPT_DIR}/build/interfaces
append_to_path PYTHONPATH $RAVE_INCLUDE 

