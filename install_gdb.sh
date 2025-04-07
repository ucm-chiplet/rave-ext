#!/bin/bash
if [ $# -lt 1 ]; then
	echo "Usage: ./$0 [0_7 / 1_0]"
	echo "Arguments:"
	echo -e "\t[0_7 / 1_0]:\tMandatory. Select RVV version"
	exit -1
fi

EXT=$1

if [[ "$EXT" != "0_7" ]] && [[ "$EXT" != "1_0" ]]; then
	echo "Extension must be 0_7 or 1_0, you said: $1"
	exit -1
fi 


if [[ "$EXT" == "0_7" ]]; then
	file=riscv-binutils-gdb
	if [ ! -f $file ]; then
		git clone https://github.com/riscvarchive/$file
	fi
	cd $file
	git checkout rvv-0.7.1
else
	ftp=https://ftp.gnu.org/gnu/gdb
	file=gdb-15.1.tar.gz
	if [ ! -f $file ]; then
		wget ${ftp}/${file}
	fi
	file=gdb-15.1
	if [ ! -f $file ]; then
		tar -xzvf ${file}.tar.gz
	fi
	cd $file
fi

build_dir=`pwd`/../build/gdb/gdb-rvv-$EXT
mkdir -p ${build_dir}
./configure --prefix=$build_dir --target=riscv64-linux-gnu
make -j 
make install

