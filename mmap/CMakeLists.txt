
project(mmap)

set(CMAKE_C_FLAGS_RELEASE "-Wall -Wextra -Wconversion -O3 -fomit-frame-pointer -march=native")

add_executable(mmaptest main.c)
