#!/bin/bash

file=elfutils 
if [ ! -d $file ]; then
	git clone git://sourceware.org/git/${file}.git
fi
cd $file

build_dir=`pwd`/../build/elfutils

if [ ! -f $build_dir ]; then
	autoreconf -i -f
	./configure --prefix=${build_dir} --enable-maintainer-mode
fi
make install -j 4 dwarf_cu_dwp_section_info_no_Werror=1 link_map_no_Werror=1



