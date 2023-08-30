#!/bin/bash
if [[ -z $1 ]] ; then
    echo "Usage: $0 <debug>"
    exit 1
else
    DEBUG=$1
fi

mkdir -p ./build_arm && cd ./build_arm
../autogen.sh
if [ $DEBUG == "debug" ]; then
    CFLAGS="-O0 -g -DDEBUG -ggdb -Wall -marm -fcommon" ../configure --build=x86_64-linux-gnu --host=arm-linux-gnueabihf --enable-bm1397 --enable-debug
else
    CFLAGS="-O2 -Wall -marm -fcommon" ../configure --build=x86_64-linux-gnu --host=arm-linux-gnueabihf --enable-bm1397
fi
make