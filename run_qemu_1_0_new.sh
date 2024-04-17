#!/bin/bash
args=;
if [ "$QEMU_PRINT_SCALAR" == "1" ]; then #OPT-in
	args=$args",arg=PRINT_SCALAR"
fi
if [ "$QEMU_PRINT_ADDR" == "1" ]; then
	args=$args",arg=PRINT_ADDR"
fi
if [ "$QEMU_PRINT_LOGFILE" == "1" ]; then
	args=$args",arg=PRINT_LOGFILE"
	if [ "$QEMU_LOGFILE" == "" ];then
		QEMU_LOGFILE=logfile.log
	fi
	#QEMU_LOGFILE="-D $QEMU_LOGFILE"
	QEMU_OPTION=-D ##TODO rename
fi

if [ "$QEMU_VLEN" == "" ];then
	QEMU_VLEN=16384
fi


if [ "$QEMU_PRINT_PRV" == "1" ] || [ "$QEMU_PRV_NAME" != "" ]; then #OPT-in
	if [ "$QEMU_PRV_NAME" == "" ]; then
		QEMU_PRV_NAME=qemutrace
	fi
	args=$args",arg=PRINT_PRV,arg=PRV_NAME,arg=$QEMU_PRV_NAME"
fi


if [ "$QEMU_PRINT_SUMMARY" == "1" ] || [ "$QEMU_PRINT_SUMMARY" == "" ]; then #OPT-out
	args=$args",arg=PRINT_SUMMARY"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
FOLDER=qemu-1_0
QEMU_SYSROOT=${SCRIPT_DIR}/build/riscv-glibc-toolchain/sysroot
QEMU_PLUGIN=${SCRIPT_DIR}/${FOLDER}/build/contrib/plugins
QEMU_PATH=${SCRIPT_DIR}/build/${FOLDER}/bin
QEMU_CPU=rv64,v=true,vext_spec=v1.0,vlen=$QEMU_VLEN


${QEMU_PATH}/qemu-riscv64 $QEMU_OPTION $QEMU_LOGFILE -d plugin -plugin ${QEMU_PLUGIN}/libexeclog.so$args -L ${QEMU_SYSROOT} -E LD_LIBRARY_PATH=${QEMU_SYSROOT}/lib -cpu $QEMU_CPU $@
#${QEMU_PATH}/qemu-riscv64 -D $QEMU_LOGFILE -L ${QEMU_SYSROOT} -E LD_LIBRARY_PATH=${QEMU_SYSROOT}/lib -cpu $QEMU_CPU $@
