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
LOGFILE=`pwd`/logfile_plugins.log
echo -n "" > $LOGFILE

if [[ "$EXT" == "0_7" ]]; then
	sources_dir=qemu-0_7
	install_dir=`pwd`/build/EPI-0.7/lib/
elif [[ "$EXT" == "1_0" ]]; then 
	sources_dir=qemu-1_0
	install_dir=`pwd`/build/EPI/lib/
fi

cd $sources_dir

echo "Building plugins..."

#Only for 0.7:
#if [[ "$EXT" == "0_7" ]]; then
#	cp ../patch_files/qemu-plugins.symbols plugins/.
#fi

#For both
plugin_name=rave
#plugin_name=rave_dbg

cp ../patch_files/$plugin_name.c contrib/plugins/.
if [[ "$EXT" == "1_0" ]]; then
 sed -i 's/#define\ EPI_07/\/\/#define EPI_07/g' contrib/plugins/${plugin_name}.c
	cp ../patch_files/instr2prv_1_0.h contrib/plugins/.
	cp ../patch_files/example_trace_1_0.h contrib/plugins/.
else
 sed -i 's/\/\/#define\ EPI_07/#define EPI_07/g' contrib/plugins/${plugin_name}.c
	cp ../patch_files/instr2prv_0_7.h contrib/plugins/.
	cp ../patch_files/example_trace_0_7.h contrib/plugins/.
fi
cp ../patch_files/my_decode.h contrib/plugins/.
cp ../patch_files/instr_data.h contrib/plugins/.
cp ../patch_files/qemu_counters.h contrib/plugins/.
cp ../patch_files/qemu2prv.h contrib/plugins/.

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
