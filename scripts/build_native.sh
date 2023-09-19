mkdir -p ./build_x86_64 && cd build_x86_64
../autogen.sh
CFLAGS="-O2 -Wall -march=native -fcommon" ../configure --enable-bm1397
make

cd ..

mkdir -p ./build_debug_x86_64 && cd ./build_debug_x86_64
../autogen.sh
CFLAGS=" -ggdb -Wall -Wextra -march=native -fcommon" ../configure --enable-bm1397
make 

# cd ..

# mkdir -p ./build_e_x86_64 && cd ./build_e_x86_64
# ../autogen.sh
# CFLAGS="-Wall -Wextra -march=native -fcommon" ../configure --enable-bm1397
# make