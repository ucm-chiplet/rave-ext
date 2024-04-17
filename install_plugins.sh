NUM_JOBS=8

EXT=0_7
LOGFILE=`pwd`/logfile_plugins.log
echo -n "" > $LOGFILE

if [[ "$EXT" == "0_7" ]]; then
	sources_dir=qemu-0_7
elif [[ "$EXT" == "1_0" ]]; then 
	sources_dir=qemu-1_0
fi

cd $sources_dir

echo "Building plugins..."

#Only for 0.7:
if [[ "$EXT" == "0_7" ]]; then
	cp ../patch_files/qemu-plugins.symbols plugins/.
fi

#For both
cp ../patch_files/execlog.c contrib/plugins/.
cp ../patch_files/my_decode.h contrib/plugins/.
cp ../patch_files/events_and_values.h contrib/plugins/.
cp ../patch_files/instr_data.h contrib/plugins/.
cp ../patch_files/qemu2prv.h contrib/plugins/.
cp ../patch_files/qemu_counters.h contrib/plugins/.
cp ../patch_files/example_trace.h contrib/plugins/.

make -j${NUM_JOBS} plugins &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Building plugins FAILED! Check $LOGFILE"
	exit -1
fi
