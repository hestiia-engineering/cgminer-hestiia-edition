SDK_PATH=/home/pixma/yocto/

if [[ -z $1 ]] ; then
    echo "Usage: $0 <debug>"
    exit 1
else
    DEBUG=$1
fi

if [ -f "$SDK_PATH/environment-setup-cortexa8hf-neon-poky-linux-gnueabi" ]; then
    . $SDK_PATH/environment-setup-cortexa8hf-neon-poky-linux-gnueabi 
else
    echo "SDK not found"
    exit 1
fi

mkdir -p ./build_yocto && cd build_yocto
../autogen.sh
if [ $DEBUG == "debug" ]; then
    CFLAGS="-O1 -g -DDEBUG -ggdb -Wall -marm -fcommon" ../configure --build=x86_64-linux-gnu --host=arm-poky-linux-gnueabi --enable-bm1397 --enable-debug
else
    CFLAGS="-O2 -Wall -marm -fcommon" ../configure --build=x86_64-linux-gnu --host=arm-poky-linux-gnueabi --enable-bm1397
fi
make