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

echo "Downloading EPI LLVM-based cross compiler for RVV $EXT ... [1/2]"
ftp=https://ssh.hca.bsc.es/epi/ftp/
if [[ "$EXT" == "0_7" ]]; then
	file=llvm-EPI-0.7-development-toolchain-cross-latest.tar.bz2
else
	file=llvm-EPI-development-toolchain-cross-latest.tar.bz2
fi

if [ ! -f $file ]; then
	wget ${ftp}/${file}
else
	echo "Skipping download!"
fi


echo "Installing compiler ... [2/2]"
build_dir=`pwd`/build
mkdir -p ${build_dir}/llvm-cross
tar -xf $file -C ${build_dir}/llvm-cross
