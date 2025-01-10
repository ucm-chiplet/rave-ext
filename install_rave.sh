#!/bin/bash
NUM_JOBS=12

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

LOGFILE=`pwd`/logfile_plugins.log
echo -n "" > $LOGFILE

build_dir=`pwd`/build/qemu-rave
mkdir -p $build_dir
if [[ "$EXT" == "0_7" ]]; then
	sources_dir=qemu_sources/qemu-rvv-0_7_1
	install_dir=${build_dir}/RVV-0_7_1/lib/
elif [[ "$EXT" == "1_0" ]]; then 
	sources_dir=qemu_sources/qemu-rvv-1_0
	install_dir=${build_dir}/RVV-1_0/lib/
fi

cd $sources_dir

echo "Building plugin...[1/2]"

plugin_name=rave

rave_sources_dir=../../rave_sources
cp ${rave_sources_dir}/$plugin_name.c contrib/plugins/.
if [[ "$EXT" == "1_0" ]]; then
 sed -i 's/#define\ EPI_07/\/\/#define EPI_07/g' contrib/plugins/${plugin_name}.c
	cp ${rave_sources_dir}/instr2prv_1_0.h contrib/plugins/.
	cp ${rave_sources_dir}/example_trace_1_0.h contrib/plugins/.
else
 sed -i 's/\/\/#define\ EPI_07/#define EPI_07/g' contrib/plugins/${plugin_name}.c
	cp ${rave_sources_dir}/instr2prv_0_7.h contrib/plugins/.
	cp ${rave_sources_dir}/example_trace_0_7.h contrib/plugins/.
fi
cp ${rave_sources_dir}/my_decode.h contrib/plugins/.
cp ${rave_sources_dir}/instr_data.h contrib/plugins/.
cp ${rave_sources_dir}/qemu_counters.h contrib/plugins/.
cp ${rave_sources_dir}/qemu2prv.h contrib/plugins/.

if ! grep -q $plugin_name contrib/plugins/Makefile; then
	sed	 -i "/^NAMES :=/a NAMES += ${plugin_name}" contrib/plugins/Makefile
fi

make -j${NUM_JOBS} plugins &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Building plugins FAILED! Check $LOGFILE"
	exit -1
fi


mkdir -p $install_dir
cp build/contrib/plugins/librave.so $install_dir/.

mkdir -p $install_dir/../bin
cp ${rave_sources_dir}/rave-$EXT.sh $install_dir/../bin/rave

echo "Building the API...[2/2]"

cd -
LLVM_DIR=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross
PATH=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/:$PATH
cd interfaces
make
cd -
