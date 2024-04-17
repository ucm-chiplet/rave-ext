#!/bin/bash

NUM_JOBS=8

if [ $# -ne 1 ]; then
	echo "Need Extension:"
	echo -e "\t./install_qemu.sh 0_7"
	echo -e "\t./install_qemu.sh 1_0"
	exit -1
fi

if [[ "$1" != "0_7" ]] && [[ "$1" != "1_0" ]]; then
	echo "Extension must be 0_7 or 1_0, you said: $1"
	exit -1
fi 

EXT=$1
#EXT=1_0

LOGFILE=`pwd`/logfile_qemu.log
echo -n "" > $LOGFILE

echo "Updating qemu sources..."

if [[ "$EXT" == "0_7" ]]; then
	sources_dir=qemu-0_7
elif [[ "$EXT" == "1_0" ]]; then 
	sources_dir=qemu-1_0
fi
install_dir=`pwd`/build/$sources_dir

git submodule update --init $sources_dir
cd $sources_dir


echo "Cleaning qemu sources..."
git restore . &>> $LOGFILE
git clean -f &>> $LOGFILE


echo "Configuring QEMU..."
rm -rf $install_dir
make clean &>> $LOGFILE
make distclean &>> $LOGFILE
#./configure --target-list=riscv64-softmmu,riscv32-softmmu,riscv64-linux-user,riscv32-linux-user --disable-docs --prefix="${install_dir}" --enable-plugins &>> $LOGFILE
./configure --target-list=riscv64-softmmu,riscv64-linux-user --disable-docs --prefix="${install_dir}" --enable-plugins &>> $LOGFILE
if [ $? -ne 0 ]; then
		echo "Configure FAILED! Check $LOGFILE"
		exit -1
fi

echo "Patching QEMU..."

##Set VLEN to 16k bits
sed -i 's/^\#define\ RV_VLEN_MAX\ .*/\#define\ RV_VLEN_MAX\ \(256\*64\)/g' ./target/riscv/cpu.h
#if [[ "$EXT" == "0_7" ]]; then
	cp ../patch_files/qemu-plugins.symbols plugins/.
#fi

echo "Building QEMU... (This might take a while)"
make -j $NUM_JOBS &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Building QEMU FAILED! Check $LOGFILE"
	exit -1
fi
make -j${NUM_JOBS} install &>> ${LOGFILE}

cd ..
