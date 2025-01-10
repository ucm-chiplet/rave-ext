#!/bin/bash

export TIME=%e
for freq in `seq 0 10 300`
do
	oldIFS=$IFS
	IFS=;
	out=`/usr/bin/time rave ./vecmix.x $freq $((1*(10**8))) 2>&1`
	IFS=$oldIFS
	echo $freq $out 
done
