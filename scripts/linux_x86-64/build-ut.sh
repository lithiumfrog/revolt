#!/bin/bash

# root dir
cd "$(dirname "$0")/../../" || { echo "Failed to change directory to project root"; exit 1; }

# bin dir
mkdir -p bin/linux_x86-64/int-ut

# delete intermediates
rm -f bin/linux_x86-64/int-ut/*.o

# flags
COMPILER_FLAGS="\
                -std=c++23 \
                -Wall \
                -Wextra \
                -Werror \
                -g \
                -fno-exceptions \
                -DSTANDARD_PLATFORM_LINUX=1 \
                -Isrc/standard \
                -Isrc/ut \
                "
LINKER_FLAGS="\
              -lpthread \
              -lGL \
    -lxcb -lxcb-util -lxcb-icccm -lxcb-keysyms \
    -lX11 -L/usr/X11R6/lib \
              -l:standard.a -Lbin/linux_x86-64 \
              "
# -lX11-xcb -lxkbcommon

# compile
echo "-- ut --"
for file in src/ut/*.cpp src/ut/**/*.cpp; do
    if [ -f "$file" ]; then
        echo "$file"
        clang++-18 -c $COMPILER_FLAGS "$file" -o "bin/linux_x86-64/int-ut/$(basename "$file" .cpp).o"
    fi
done

# link
clang++-18 bin/linux_x86-64/int-ut/*.o $LINKER_FLAGS -o bin/linux_x86-64/ut
if [ $? -eq 0 ]; then
    echo "\"ut\""
fi

# clean up
exit 0
