#!/bin/bash

if [ $# -lt 1 ] || [[ "$1" == "--help" ]]; then
	echo "Usage: $0 path/to/your/binary [arguments]"
	echo "Additionally, these environment variables control the tracing plugin:"
echo "
 - **RAVE_TRACE_SCALAR**: If set to \"1\", adds tracing information for each scalar instruction (trace gets a lot bigger). Otherwise, scalar instructions are treated as bursts. (default: 0)
 - **RAVE_TRACE_ADDR**: If set to \"1\", adds tracing information of the base address of vector loads/stores. (default: 0)

Control RAVE internals:
 - RAVE_VLEN: Sets the maximum available vector-length in bits (default: 16384).
 - RAVE_SYSROOT: Sets the path to a user-specified RISC-V sysroot.
 - RAVE_CUSTOM_EXTENSIONS: Appends RISC-V extensions to the QEMU cpu (e.g. \"zicbom=true,zicboz=true,zicbop=true,zicond=true\" to emulate the bananapif3 boards). 

Control logfile generation:
 - RAVE_LOGFILE: If set to \"1\", a logfile is generated with all the executed instructions. (default: 0).
 - RAVE_LOGFILE_NAME: Sets the name of the generated logfile (default: qemulog.log). Additionally, automatically sets RAVE_LOGFILE to 1.

Control PRV generation:
 - RAVE_PRV: If set to \"1\", a paraver trace is generated with all the executed instructions. (default: 0).
 - RAVE_PRV_NAME: Sets the name of the generated paraver trace (default: qemutrace). Additionally, automatically sets RAVE_PRV to 1.
 - RAVE_REGION_EVENT: Set paraver event where the first nesting level of regions will be mapped. (default: 1000) 

Control report / profile / csv generation:
 - RAVE_PLAIN_TEXT: Print the report/profile without colours or highlighted text. (default: 0).
 - RAVE_ACCUM_REGIONS: If set to "1", a region that appears twice will be aggregated/accumulated. (default: 0).

 - RAVE_REPORT: If set to "1", the tracer will print to stdout a hardware counter summary for each executed code region. (default: 0).
 - RAVE_REPORT_NAME: Redirects the report to the provided file name. Additionally, automatically sets RAVE_REPORT to 1. 

 - RAVE_PROFILE: If set to "1", the tracer will print to stdout a profiling of the executed loops. (default: 0).
 - RAVE_PROFILE_NAME: Redirects the profile to the provided file name. Additionally, automatically sets RAVE_PROFILE to 1. 

 - RAVE_CSV: If set to "1", the tracer will print a CSV with the hardware counter summary for each executed code region. (default: 0).
 - RAVE_CSV_NAME: Sets the name of the generated csv trace (default: qemu_summary.csv). Additionally, automatically sets RAVE_CSV to 1.
"
	exit -1
fi


#Legacy support... To be drop at some point.
if [ "$RAVE_PRINT_SCALAR" == "1" ]; then
	export RAVE_TRACE_SCALAR=1
	echo RAVE WARNING! RAVE_PRINT_SCALAR is deprecated, changed it to RAVE_TRACE_SCALAR
fi
if [ "$RAVE_PRINT_ADDR" == "1" ]; then
	export RAVE_TRACE_ADDR=1
	echo RAVE WARNING! RAVE_PRINT_ADDR is deprecated, changed it to RAVE_TRACE_ADDR
fi


args=;
if [ "$RAVE_TRACE_SCALAR" == "1" ]; then
	args=$args",arg=TRACE_SCALAR"
fi
if [ "$RAVE_TRACE_ADDR" == "1" ]; then
	args=$args",arg=TRACE_ADDR"
fi
if [ "$RAVE_PRINT_LOGFILE" == "1" ] || [ "$RAVE_LOGFILE_NAME" != "" ]; then
	args=$args",arg=PRINT_LOGFILE"
	if [ "$RAVE_LOGFILE_NAME" == "" ];then
		RAVE_LOGFILE_NAME=qemulog.log
	fi
	QEMU_OPTION=-D 
fi

if [ "$RAVE_VLEN" == "" ];then
	RAVE_VLEN=16384
fi

if [ "$RAVE_PRINT_PRV" == "1" ] ||  [ "$RAVE_PRV_NAME" != "" ]; then #OPT-in
	if [ "$RAVE_PRV_NAME" == "" ]; then
		RAVE_PRV_NAME=qemutrace
	fi
	args=$args",arg=PRINT_PRV,arg=PRV_NAME=$RAVE_PRV_NAME"
fi

if [ "$RAVE_PRINT_REPORT" == "1" ]; then #OPT-in
	args=$args",arg=PRINT_REPORT"
fi
if [ "$RAVE_ACCUM_REGIONS" == "1" ]; then #OPT-in
	args=$args",arg=ACCUM_REGIONS"
fi
if [ "$RAVE_REGION_EVENT" != "" ]; then #OPT-in
	args=$args",arg=REGION_EVENT=$RAVE_REGION_EVENT"
fi
if [ "$RAVE_REPORT_NAME" != "" ]; then #OPT-in
	args=$args",arg=REPORT,arg=REPORT_NAME=$RAVE_REPORT_NAME"
fi
if [ "$RAVE_PRINT_PROFILE" == "1" ]; then #OPT-in
	i=1
	while [[ "${!i}" == "-E"* ]] || [[ "${!i}" == *"="* ]] ; do
		((i++))
	done
	args=$args",arg=PRINT_PROFILE,arg=BINARY_NAME=${!i}"
fi
if [ "$RAVE_PROFILE_NAME" != "" ]; then #OPT-in
	i=1
	while [[ "${!i}" == "-E"* ]] || [[ "${!i}" == *"="* ]] ; do
		((i++))
	done
	args=$args",arg=PRINT_PROFILE,arg=BINARY_NAME=${!i},arg=PROFILE_NAME=$RAVE_PROFILE_NAME"
fi
if [ "$RAVE_PRINT_CSV" == "1" ] || [ "$RAVE_CSV_NAME" != "" ]; then #OPT-in
	if [ "$RAVE_CSV_NAME" == "" ]; then
		RAVE_CSV_NAME=qemu_summary.csv
	fi
	args=$args",arg=PRINT_CSV,arg=CSV_NAME=$RAVE_CSV_NAME"
fi
if [ "$RAVE_PLAIN_TEXT" == "1" ]; then
	args=$args",arg=PLAIN_TEXT"
fi
if [ "$RAVE_MUSA" == "1" ]; then
	args=$args",arg=MUSA"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
#SYSROOT:
if [ "$RAVE_SYSROOT" == "" ]; then
	RAVE_SYSROOT=${SCRIPT_DIR}/../../../sysroot 
fi
#QEMU_SYSROOT=/apps/riscv/fpga-sdv/jammy-1/ #USER-SPECIFIED SYSROOT

RAVE_PLUGIN=${SCRIPT_DIR}/../lib
QEMU_PATH=${SCRIPT_DIR}/../qemu/bin
QEMU_CPU=rv64,x-v=true,vext_spec=v0.7.1,vlen=$RAVE_VLEN
if [ "$RAVE_CUSTOM_EXTENSIONS" != "" ]; then 
	QEMU_CPU=${QEMU_CPU},${RAVE_CUSTOM_EXTENSIONS}
fi

export LD_LIBRARY_PATH=${SCRIPT_DIR}/../../../elfutils/lib:$LD_LIBRARY_PATH
${QEMU_PATH}/qemu-riscv64 $QEMU_OPTION $RAVE_LOGFILE_NAME -d plugin -plugin ${RAVE_PLUGIN}/librave.so$args -L ${RAVE_SYSROOT} -E LD_LIBRARY_PATH=${QEMU_SYSROOT}/lib:${SCRIPT_DIR}/../../../parallel:$LD_LIBRARY_PATH -E PATH=$PATH -E LD_RUN_PATH=$LD_RUN_PATH -E LD_PRELOAD=${SCRIPT_DIR}/../../../parallel/ompt.so:$LD_PRELOAD -cpu $QEMU_CPU $@
