#!/bin/bash
# get version from Cargo.toml
# if flag -d is passed then debug build is done
if [ "$1" == "-d" ]; then
    echo "Debug build"
    ./scripts/build_yocto.sh debug
    BIN_PATH=./build_yocto
else
    echo "Release build"
    ./scripts/build_yocto.sh release
    BIN_PATH=./build_yocto
fi
REMOTE_WORKSPACE=/data/workspace/cgminer
HOST=myeko-5111

scp $BIN_PATH/cgminer myeko@$HOST:$REMOTE_WORKSPACE/


if [ "$2" == "-r" ]; then
    TARGET_NAME=myeko-5111
    TARGET_USER=myeko
    GDB_PORT=11777
    ssh -f "${TARGET_USER}@${TARGET_NAME}" "sh -i -c 'cd ${REMOTE_WORKSPACE}; killall gdbserver; nohup gdbserver *:${GDB_PORT} cgminer'"
fi