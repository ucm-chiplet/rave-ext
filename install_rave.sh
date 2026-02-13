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
	cp ${rave_sources_dir}/instr2prv_1_0.c contrib/plugins/.
	cp ${rave_sources_dir}/example_trace_1_0.c contrib/plugins/.
else
 sed -i 's/\/\/#define\ EPI_07/#define EPI_07/g' contrib/plugins/${plugin_name}.c
	cp ${rave_sources_dir}/instr2prv_0_7.c contrib/plugins/.
	cp ${rave_sources_dir}/example_trace_0_7.c contrib/plugins/.
fi

for file in profiling.c formatting.c 07_decode.c instr_data.c rave_counters.c rave_events.c rave_regions.c rave2prv.c rave_threading.c rave_utils.c rave_init_exit.c rave_callbacks.c
do
	cp ${rave_sources_dir}/$file contrib/plugins/.
done

if ! grep -q $plugin_name contrib/plugins/Makefile; then
	sed	 -i "/^NAMES :=/a NAMES += ${plugin_name}" contrib/plugins/Makefile
fi
 
elfutils=${build_dir}/../elfutils/
sed -i "s;\$(CFLAGS);\0 -I${elfutils}/include;g" contrib/plugins/Makefile
#sed -i "s;\$(CFLAGS);\0 -I${elfutils}/include -Wall -Werror;g" contrib/plugins/Makefile
#make LDLIBS="-L${elfutils}/lib -Wl,-rpath=${elfutils}/lib -lelf -ldw" V=1 -j${NUM_JOBS} plugins &>> ${LOGFILE}
make LDLIBS="-L${elfutils}/lib -lelf -ldw" V=1 -j${NUM_JOBS} plugins &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Building plugins FAILED! Check $LOGFILE"
	exit -1
fi


mkdir -p $install_dir
cp build/contrib/plugins/librave.so $install_dir/.
#cp build/contrib/plugins/libcache.so $install_dir/.

mkdir -p $install_dir/../bin
cp ${rave_sources_dir}/rave-$EXT.sh $install_dir/../bin/rave

cp ../../utils/rave_gdb $install_dir/../bin/rave_gdb
sed -i "s/EXT/${EXT}/g" $install_dir/../bin/rave_gdb

echo "Building the API...[2/2]"

cd -
if [[ "$EXT" == "1_0" ]]; then
	LLVM_DIR=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross
	export PATH=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/:$PATH
	cd interfaces
	make
	cd -
fi
