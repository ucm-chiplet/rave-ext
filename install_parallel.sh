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


echo "Installing RISC-V MPI cross-compiler and libraries...[3/3]"

#Download MPI tar
echo "Downloading..."
folder=openmpi-4.1.6
if [ ! -f ${folder}.tar.bz2 ]; then
	wget https://download.open-mpi.org/release/open-mpi/v4.1/${folder}.tar.bz2
fi

#Extract MPI tar
echo "Extracting..."
if [ ! -d $folder ]; then
	tar -xf openmpi-4.1.6.tar.bz2
fi

#Get into MPI folder
cd $folder

#Don't use GCC builtins!
sed -i 's/pmix_cv_asm_builtin=\"BUILTIN_GCC\"/pmix_cv_asm_builtin=\"BUILTIN_C11\"/g' ./opal/mca/pmix/pmix3x/pmix/configure

#Put LLVM 1.0 cross-compiler in path
PATH=`pwd`/../../build/llvm-cross/llvm-EPI-development-toolchain-cross/bin/:$PATH

##Cross-Compile Native MPI (binaries & lib)
echo "Configuring...[1/4]"
mpi_dir=${build_dir}/openmpi-4.1.6
make clean
./configure --host=riscv64-linux-gnu \
 						--prefix=${mpi_dir} \
					 	CC=clang CXX=clang++ FC=flang \
					 	--with-cross=`pwd`/../../utils/mpi_cross.sizes \
						--enable-binaries=yes \
						--bindir=${mpi_dir}/bin-native --includedir=${mpi_dir}/include --libdir=${mpi_dir}/lib
						--enable-mpirun-prefix-by-default \
					 	--enable-mpi-cxx \
						--enable-shared \
						--with-hwloc=internal \


echo "Installing...[2/4]"
make -j 10
make install

#Cross-Compile Cross-Compiler wrappers MPICC, MPIFORT, ...
echo "Configuring...[3/4]"
./configure --host=riscv64-linux-gnu \
	--prefix=${mpi_dir} \
	CC=clang CXX=clang++ FC=flang \
	--enable-script-wrapper-compilers \
	--with-cross=`pwd`/../../utils/mpi_cross.sizes \
	--enable-binaries=no \
	--with-wrapper-ldflags=-Wl,-rpath-link=\${libdir} \
	--bindir=${mpi_dir}/bin-cross --includedir=${mpi_dir}/include --libdir=${mpi_dir}/lib \
	--enable-mpi-cxx

echo "Installing...[4/4]"
cd ompi/tools/wrappers
make -j 10
make install

