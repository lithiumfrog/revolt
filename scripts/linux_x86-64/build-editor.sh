#!/bin/bash

# root dir
cd "$(dirname "$0")/../../" || { echo "Failed to change directory to project root"; exit 1; }

# bin dir
mkdir -p bin/linux_x86-64/int-editor

# delete intermediates
rm -f bin/linux_x86-64/int-editor/*.o

# flags
COMPILER_FLAGS="\
                -std=c++23 \
                -Wall \
                -Wextra \
                -Werror \
                -g \
                -fexceptions \
                -DSTANDARD_PLATFORM_LINUX=1 \
                -Isrc/standard \
                -Isrc/editor \
                "
LINKER_FLAGS="\
              -lpthread \
              -ldl \
              -lX11 \
              -l:standard.a -Lbin/linux_x86-64 \
              "

# compile
echo "-- editor --"
for file in src/editor/*.cpp src/editor/**/*.cpp; do
    if [ -f "$file" ]; then
        echo "$file"
        clang++-18 -c $COMPILER_FLAGS "$file" -o "bin/linux_x86-64/int-editor/$(basename "$file" .cpp).o"
    fi
done

# link
clang++-18 bin/linux_x86-64/int-editor/*.o $LINKER_FLAGS -o bin/linux_x86-64/editor
if [ $? -eq 0 ]; then
    echo "\"editor\""
fi

# clean up
exit 0
