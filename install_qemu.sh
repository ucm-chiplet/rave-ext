#!/bin/bash

NUM_JOBS=12

if [ $# -lt 1 ]; then
	echo "Usage: ./$0 [0_7 / 1_0] (system)"
	echo "Arguments:"
	echo -e "\t[0_7 / 1_0]:\tMandatory. Select RVV version"
	echo -e "\t(system):\tOptional. Enables system-emulation (not needed for RAVE)"
	exit -1
fi

EXT=$1

if [[ "$EXT" != "0_7" ]] && [[ "$EXT" != "1_0" ]]; then
	echo "Extension must be 0_7 or 1_0, you said: $1"
	exit -1
fi 

targets="--target-list=riscv64-linux-user"
if [ $# -eq 2 ]; then
	if [[ "$2" == "system" ]]; then
		targets="$targets,riscv64-softmmu"
	else
		echo "If present, second argument needs to be \"system\""
		exit -1
	fi
fi


LOGFILE=`pwd`/logfile_install_qemu.log
echo -n "" > $LOGFILE

echo "Updating qemu sources...[1/5]"

build_dir=`pwd`/build/qemu-rave
if [[ "$EXT" == "0_7" ]]; then
	sources_dir=qemu_sources/qemu-rvv-0_7_1
	install_dir=${build_dir}/RVV-0_7_1/qemu 
elif [[ "$EXT" == "1_0" ]]; then 
	sources_dir=qemu_sources/qemu-rvv-1_0
	install_dir=${build_dir}/RVV-1_0/qemu
fi

git submodule update --init $sources_dir
cd $sources_dir


echo "Cleaning qemu sources...[2/5]"
git restore . &>> $LOGFILE
git clean -f &>> $LOGFILE


echo "Configuring QEMU...[3/5]"
rm -rf $install_dir
make clean &>> $LOGFILE
make distclean &>> $LOGFILE


options="$targets --disable-docs --prefix="${install_dir}" --enable-plugins"
#if [[ "$EXT" == "1_0" ]]; then
#	options="$options --python=python3.10"
#fi

./configure $options &>> $LOGFILE
if [ $? -ne 0 ]; then
		echo "Configure FAILED! Check $LOGFILE"
		exit -1
fi

echo "Patching QEMU... [4/5]"

##Set VLEN to 16k bits
sed -i 's/^\#define\ RV_VLEN_MAX\ .*/\#define\ RV_VLEN_MAX\ \(256\*64\)/g' ./target/riscv/cpu.h
#cp ../../utils/qemu-plugins.symbols plugins/.

#if [[ "$EXT" == "0_7" ]]; then
	git checkout plugins/qemu-plugins.symbols
	sed -i '/};/i qemu_get_cpu;' plugins/qemu-plugins.symbols
	sed -i '/};/i qemu_plugin_hwaddr_device_name;' plugins/qemu-plugins.symbols
	sed -i '/};/i qemu_plugin_hwaddr_is_io;' plugins/qemu-plugins.symbols
	sed -i '/};/i qemu_plugin_hwaddr_phys_addr;' plugins/qemu-plugins.symbols
	sed -i '/};/i qemu_plugin_insn_symbol;' plugins/qemu-plugins.symbols
	sed -i '/};/i cpu_memory_rw_debug;' plugins/qemu-plugins.symbols
#fi


#Hybrid translator loop 
patch -p 1 < ../../utils/translate_${EXT}.patch

#Expose vector registers to GDB in 0_7
if [[ "$EXT" == "0_7" ]]; then
	patch -p 0 < ../../utils/expose_0_7.patch
fi

echo "Building QEMU... [5/5] (This might take a while)"
make -j $NUM_JOBS &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Building QEMU FAILED! Check $LOGFILE"
	exit -1
fi
make -j${NUM_JOBS} install &>> ${LOGFILE}

cd ..
