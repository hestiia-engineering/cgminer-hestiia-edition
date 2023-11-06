SDK_PATH=/home/pixma/yocto/

if [ -f "$SDK_PATH/environment-setup-cortexa8hf-neon-poky-linux-gnueabi" ]; then
    . $SDK_PATH/environment-setup-cortexa8hf-neon-poky-linux-gnueabi 
else
    echo "SDK not found"
    exit 1
fi

mkdir -p ./build_yocto && cd build_yocto
../autogen.sh
CFLAGS="-O2 -Wall -fcommon" ../configure ${CONFIGURE_FLAGS} --enable-bm1397
make