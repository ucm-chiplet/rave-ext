#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Usage: $0 path/to/your/binary [arguments]"
	echo "Additionally, these environment variables control the tracing plugin:"
	echo -e "\tRAVE_PRINT_SCALAR: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)"
	echo -e "\tRAVE_PRINT_LOGFILE: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0)"
	echo -e "\tRAVE_LOGFILE_NAME: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets RAVE_PRINT_LOGFILE to 1" 
	echo -e "\tRAVE_VLEN: Sets the maximum available vector-length in bits (default: 16384)"
	echo -e "\tRAVE_PRINT_PRV: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0)" 
	echo -e "\tRAVE_PRV_NAME: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets RAVE_PRINT_PRV to 1"
	echo -e "\tRAVE_PRINT_CSV: If set to \"1\", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0)."
	echo -e "\tRAVE_CSV_NAME: Sets the name of the generated csv trace (default: qemu_summary.csv). Additionally, automatically sets RAVE_PRINT_CSV to 1."
	echo -e "\tRAVE_SYSROOT: Sets the path to a user-specified RISC-V sysroot."
	exit -1
fi

args=;
if [ "$RAVE_PRINT_SCALAR" == "1" ]; then #OPT-in
	args=$args",PRINT_SCALAR=on"
fi
if [ "$RAVE_PRINT_ADDR" == "1" ]; then
	args=$args",arg=PRINT_ADDR"
fi
if [ "$RAVE_PRINT_LOGFILE" == "1" ] || [ "$RAVE_LOGFILE_NAME" != "" ]; then
	args=$args",PRINT_LOGFILE=on"
	if [ "$RAVE_LOGFILE_NAME" == "" ];then
		RAVE_LOGFILE_NAME=qemulog.log
	fi
	QEMU_OPTION=-D 
fi

if [ "$RAVE_VLEN" == "" ];then
	RAVE_VLEN=16384
fi

if [ "$RAVE_PRINT_PRV" == "1" ] || [ "$RAVE_PRV_NAME" != "" ]; then #OPT-in
	if [ "$RAVE_PRV_NAME" == "" ]; then
		RAVE_PRV_NAME=qemutrace
	fi
	args=$args",PRINT_PRV=on,PRV_NAME=$RAVE_PRV_NAME"
fi

if [ "$RAVE_DETECT_SYMBOLS" == "1" ]; then #OPT-in
	args=$args",DETECT_SYMBOLS=on"
fi
if [ "$RAVE_PRINT_REPORT" == "1" ]; then #OPT-in
	args=$args",PRINT_REPORT=on"
fi
if [ "$RAVE_REPORT_NAME" != "" ]; then #OPT-in
	args=$args",PRINT_REPORT=on,REPORT_NAME=$RAVE_REPORT_NAME"
fi
if [ "$RAVE_PRINT_CSV" == "1" ] || [ "$RAVE_CSV_NAME" != "" ]; then #OPT-in
	if [ "$RAVE_CSV_NAME" == "" ]; then
		RAVE_CSV_NAME=qemu_summary.csv
	fi
	args=$args",PRINT_CSV=on,CSV_NAME=$RAVE_CSV_NAME"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
#SYSROOT:
if [ "$RAVE_SYSROOT" == "" ]; then
	RAVE_SYSROOT=${SCRIPT_DIR}/../../../sysroot 
fi

RAVE_PLUGIN=${SCRIPT_DIR}/../lib 
QEMU_PATH=${SCRIPT_DIR}/../qemu/bin
QEMU_CPU=rv64,v=true,vext_spec=v1.0,vlen=$RAVE_VLEN
if [ "$RAVE_CUSTOM_EXTENSIONS" != "" ]; then 
	QEMU_CPU=${QEMU_CPU},${RAVE_CUSTOM_EXTENSIONS}
fi

${QEMU_PATH}/qemu-riscv64 $QEMU_OPTION $RAVE_LOGFILE_NAME -d plugin -plugin ${RAVE_PLUGIN}/librave.so$args -L ${RAVE_SYSROOT} -E LD_LIBRARY_PATH=${QEMU_SYSROOT}/lib:${SCRIPT_DIR}/../../../parallel:$LD_LIBRARY_PATH -E PATH=$PATH -E LD_RUN_PATH=$LD_RUN_PATH -E LD_PRELOAD=${SCRIPT_DIR}/../../../parallel/ompt.so:$LD_PRELOAD -cpu $QEMU_CPU $@

