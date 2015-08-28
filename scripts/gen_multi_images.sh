#!/bin/sh

if test -e include/config/auto.conf; then
	. include/config/auto.conf
else
	echo "Error: include/config/auto.conf not found - run 'make prepare' to update it"
	exit 1
fi

if [ "CONFIG_OF_MULTI_IMAGES" != y ]; then
	exit 0
fi

if [ ! -e .tmp_dtbs ]; then
	echo "Error: .tmp_dtbs not found.  Please send bug report"
	exit 1
fi

for dtb in $(.tmp_dtbs)
do
	echo 
	echo $dtb

done
