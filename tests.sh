#!/bin/bash

# compilation
gcc -o tests tests.c -I src/ -O2 -W -Wall -Wextra -pedantic -Wpedantic -std=c99 -Wbad-function-cast -Wcast-align -Wcast-qual -Wconversion -Wdate-time -Wfloat-equal -Wformat=2 -Winit-self -Wnested-externs -Wnull-dereference -Wold-style-definition -Wpointer-arith -Wshadow -Wstack-protector -Wstrict-prototypes -Wswitch-default -Wwrite-strings -Wmissing-prototypes -Wformat-security -fstack-protector-strong -fPIE -D_FORTIFY_SOURCE=2 -D_XOPEN_SOURCE=700 -Wduplicated-cond -Wformat-signedness -Wjump-misses-init -Wlogical-op -Wnormalized -Wsuggest-attribute=format -Wtrampolines -pie

./tests

if (( $? != 0 ))
then
    exit -1
fi

diff ./test_rd_file ./COPYING 2>> /dev/null
if (( $? != 0 ))
then
    echo "Test ead file fail, file contains diff."
    exit -1
fi

rm ./test_rd_file
rm tests

echo "Tests success."
