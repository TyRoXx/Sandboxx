mv user-config.jam ~
./b2 toolset=gcc-arm abi=aapcs address-model=32 architecture=arm binary-format=elf -j12 -s ZLIB_SOURCE=/home/virtual/dev/libs/zlib-1.2.8
