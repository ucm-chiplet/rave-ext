#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Usage: ./run_qemu_0_7_1.sh path/to/your/binary [arguments]"
	echo "Additionally, these environment variables control the tracing plugin:"
	echo -e "\tQEMU_PRINT_SCALAR: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)"
	echo -e "\tQEMU_PRINT_LOGFILE: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0)"
	echo -e "\tQEMU_LOGFILE_NAME: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets QEMU_PRINT_LOGFILE to 1" 
	echo -e "\tQEMU_VLEN: Sets the maximum available vector-length in bits (default: 16384)"
	echo -e "\tQEMU_PRINT_PRV: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0)" 
	echo -e "\tQEMU_PRV_NAME: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets QEMU_PRINT_PRV to 1"
	echo -e "\tQEMU_PRINT_REGIONS: If set to \"1\", the tracer will print a hardware counter summary for each executed code region." 
	echo -e "\tQEMU_PRINT_AVERAGE: If set to \"1\", the tracer will print an average of the hardware counters for each defined code region." 
fi

args=;
if [ "$QEMU_PRINT_SCALAR" == "1" ]; then #OPT-in
	args=$args",PRINT_SCALAR=on"
fi
#if [ "$QEMU_PRINT_ADDR" == "1" ]; then
#	args=$args",arg=PRINT_ADDR"
#fi
if [ "$QEMU_PRINT_LOGFILE" == "1" ] || [ "$QEMU_LOGFILE_NAME" != "" ]; then
	args=$args",PRINT_LOGFILE=on"
	if [ "$QEMU_LOGFILE_NAME" == "" ];then
		QEMU_LOGFILE_NAME=qemulog.log
	fi
	QEMU_OPTION=-D 
fi

if [ "$QEMU_VLEN" == "" ];then
	QEMU_VLEN=16384
fi

if [ "$QEMU_PRINT_PRV" == "1" ] || [ "$QEMU_PRV_NAME" != "" ]; then #OPT-in
	if [ "$QEMU_PRV_NAME" == "" ]; then
		QEMU_PRV_NAME=qemutrace
	fi
	args=$args",PRINT_PRV=on,arg=PRV_NAME,arg=$QEMU_PRV_NAME"
fi

if [ "$QEMU_PRINT_REPORT" == "1" ]; then #OPT-in
	args=$args",PRINT_REPORT=on"
fi

if [ "$QEMU_PRINT_CSV" == "1" ] || [ "$QEMU_CSV_NAME" != "" ]; then #OPT-in
	if [ "$QEMU_CSV_NAME" == "" ]; then
		QEMU_CSV_NAME=qemu_summary.csv
	fi
	args=$args",PRINT_CSV=on,arg=CSV_NAME,arg=$QEMU_CSV_NAME"
fi

#SYSROOT:
#QEMU_SYSROOT=${SCRIPT_DIR}/build/riscv-glibc-toolchain/sysroot #DEFAULT SYSROOT
QEMU_SYSROOT=/apps/riscv/fpga-sdv/jammy-1/ #USER-SPECIFIED SYSROOT

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
FOLDER=qemu-1_0
QEMU_PLUGIN=${SCRIPT_DIR}/${FOLDER}/build/contrib/plugins
QEMU_PATH=${SCRIPT_DIR}/build/${FOLDER}/bin
QEMU_CPU=rv64,v=true,vext_spec=v1.0,vlen=$QEMU_VLEN

${QEMU_PATH}/qemu-riscv64 $QEMU_OPTION $QEMU_LOGFILE_NAME -d plugin -plugin ${QEMU_PLUGIN}/libexeclog.so$args -L ${QEMU_SYSROOT} -E LD_LIBRARY_PATH=${QEMU_SYSROOT}/lib -cpu $QEMU_CPU $@

