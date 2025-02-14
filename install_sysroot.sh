#!/bin/bash

echo "Downloading sysroot...[1/2]"
file=sysroot.tar.gz
ftp=https://ssh.hca.bsc.es/epi/ftp/RAVE

if [ ! -f $file ]; then
	wget ${ftp}/${file}
else
	echo "Skipping download!"
fi

echo "Installing sysroot...[2/2]"
tar -xzf $file -C build 

