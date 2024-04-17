NUM_JOBS=8
LOGFILE=`pwd`/logfile_toolchain.log
echo -n "" > $LOGFILE
TC_INSTALL=`pwd`/build/riscv-glibc-toolchain

echo "Checking out toolchain (glibc)..."
git submodule update --init riscv-gnu-toolchain
cd riscv-gnu-toolchain
make clean &>> ${LOGFILE}
make distclean &>> ${LOGFILE}
git submodule update --init glibc
#
echo "Configuring toolchain (glibc)..."
autoreconf -ivf &>> ${LOGFILE}
#TC_OPTS="--with-cmodel=medany --enable-multilib"
TC_OPTS="--with-cmodel=medany --disable-multilib --disable-gdb --disable-libsanitizer --disable-qemu-system"

./configure --prefix="${TC_INSTALL}" ${TC_OPTS} &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "Configure toolchain FAILED! Check $LOGFILE"
	exit -1
fi


echo "Building toolchain (glibc) ... (This might take a while)"
time make -j${NUM_JOBS} linux &>> ${LOGFILE}
if [ $? -ne 0 ]; then
	echo "BUILD toolchain FAILED! Check $LOGFILE"
	exit -1
fi
