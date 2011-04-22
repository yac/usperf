#/usr/bin/env bash

if [ $# -ne 1 ]; then 
	echo Need a program to run on all CPUs as a single argumnet.
	exit 1
fi
program=$1

ncpu=`grep processor /proc/cpuinfo | wc -l`
(( ncpu-- ))

for cpu in `seq 0 $ncpu`; do
	echo -e "\n +-+-+ Running \`$program\` on CPU $cpu:\n"
	taskset -c $cpu $program || exit $?
done
