#/usr/bin/env bash

tests=`cat tests | tr '\n' ' '`

make

for t in $tests; do
	./run_on_all_CPUs.sh ./$t || exit $?
done

echo -e "\n +-+-+ Good news everyone! All the tests succeeded.\n"
