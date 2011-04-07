#!/bin/bash

if [[ $# < 1 ]]
then
	echo "Usage $0 app_path"
	exit 1
fi

basePath=/tmp/selinux-audit
app=$1
shift 1

mkdir $basePath 2> /dev/null

while true
do
	rm $basePath/* -rf 
	strace -e "trace=open,execve,mkdir" -o $basePath/strace $app $@ 

	echo "The application stopped. Wait for the audit to finish before reloading."
	touch $basePath/lock
	if [[ $? != 0 ]]
	then
		exit 1
	fi
	while [[ $? == 0 ]]
	do
		sleep 0.2		 
		ls -l $basePath/lock > /dev/null 2> /dev/null
	done
done
