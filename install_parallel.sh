#!/bin/bash
build_dir=`pwd`/build/parallel
mkdir -p ${build_dir}

LLVM_DIR=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross
PATH=`pwd`/build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/:$PATH


cd parallel
echo "Building OMPT...[1/3]"
make ompt.so
mv lib/ompt.so ${build_dir}/.


echo "Copying LIBOMP...[2/3]"
cp lib/libomp.so ${build_dir}/.


exit
echo "Installing RISC-V MPI cross-compiler and libraries...[3/3]"

echo "Downloading..."
file=openmpi-4.1.6
if [ ! -f ${file}.tar.bz2 ]; then
	wget https://download.open-mpi.org/release/open-mpi/v4.1/${file}.tar.bz2
fi

echo "Extracting..."
if [ ! -d $file ]; then
	tar -xf openmpi-4.1.6.tar.bz2
fi

echo "Configuring..."
cd openmpi-4.1.6
#mkdir -p ${build_dir}/openmpi/native/openmpi-4.1.6

#Don't use GCC builtins!
sed -i 's/pmix_cv_asm_builtin=\"BUILTIN_GCC\"/pmix_cv_asm_builtin=\"BUILTIN_C11\"/g' ./opal/mca/pmix/pmix3x/pmix/configure

PATH=`pwd`/../../build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/:$PATH
./configure --host=riscv64-linux-gnu --prefix=$build_dir/openmpi-4.1.6 CC=clang CXX=clang++ FC=flang --enable-script-wrapper-compilers --with-cross=`pwd`/../../utils/mpi_cross.sizes --enable-binaries=no --with-wrapper-ldflags=-Wl,-rpath-link=\${libdir} 
#./configure --host=riscv64-linux-gnu --prefix=$build_dir/openmpi-4.1.6-native CC=clang CXX=clang++ FC=flang --with-cross=`pwd`/../../utils/mpi_cross.sizes --enable-binaries=yes --with-wrapper-ldflags=-Wl,-rpath-link=\${libdir} 

echo "Installing..."
make -j
make install
