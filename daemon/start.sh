#!/bin/bash

if [ $# -ne 2 ] ; then
	echo "usage: $0 pid_contextd pid_contxt-nofify";
	exit 1;
fi

LD_LIBRARY_PATH="../piga-systrans/build/src/" ./daemon $1 $2
