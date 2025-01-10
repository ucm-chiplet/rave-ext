#!/bin/bash
if [[ "$RVV" != "0_7" ]] && [[ "$RVV" != "1_0" ]]; then
	echo "Environment variable RVV must be set to either 0_7 or 1_0. Now its set at: $RVV"
else
	SCRIPT_DIR=$(dirname -- "$(readlink -f -- "$BASH_SOURCE")")
	if [[ "$RVV" == "0_7" ]]; then
		export RAVE_DIR=${SCRIPT_DIR}/build/qemu-rave/RVV-0_7_1
		export LLVM_DIR=${SCRIPT_DIR}/build/llvm-cross/llvm-EPI-0.7-development-toolchain-cross
	else
		export RAVE_DIR=${SCRIPT_DIR}/build/qemu-rave/RVV-1_0
		export LLVM_DIR=${SCRIPT_DIR}/build/llvm-cross/llvm-EPI-development-toolchain-cross
	fi


	if [ ! -f ${RAVE_DIR}/bin/rave ]; then
		echo "WARNING: RAVE is not installed!"
	else
		echo "RAVE path: $RAVE_DIR"
		export PATH=${RAVE_DIR}/bin:${PATH}
		export RAVE_INCLUDE=${SCRIPT_DIR}/interfaces
		export PYTHONPATH=${RAVE_INCLUDE}
	fi

	if [ ! -d ${LLVM_DIR} ]; then
		echo "WARNING: LLVM cross-compiler is not installed!"
	else
		echo "LLVM path: $LLVM_DIR"
		export PATH=${LLVM_DIR}/bin:${PATH}
	fi

	#MPICC_DIR=${SCRIPT_DIR}/build/parallel/openmpi/cross/4.1.6_gcc11.4.0
	MPICC_DIR=${SCRIPT_DIR}/build/parallel/openmpi-4.1.6/
	if [ ! -d ${SYSROOT_DIR} ]; then
		echo "WARNING: MPI cross-compiler not installed!"
	else
		echo "MPICC path: $MPICC_DIR"
		export PATH=${MPICC_DIR}/bin:${PATH}
	fi

	SYSROOT_DIR=${SCRIPT_DIR}/build/sysroot
	if [ ! -d ${SYSROOT_DIR} ]; then
		echo "WARNING: RISC-V Sysroot not installed!"
	else
		echo "SYSROOT path: $SYSROOT_DIR"
	fi

fi 

