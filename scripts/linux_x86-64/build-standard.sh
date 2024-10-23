#!/bin/bash

# root dir
cd "$(dirname "$0")/../../" || { echo "Failed to change directory to project root"; exit 1; }

# bin dir
mkdir -p bin/linux_x86-64/int-standard

# delete intermediates
rm -f bin/linux_x86-64/int-standard/*.o

# flags
COMPILER_FLAGS="\
                -std=c++23 \
                -Wall \
                -Wextra \
                -Werror \
                -g \
                -fno-exceptions \
                -DSTANDARD_PLATFORM_LINUX=1 \
                -DSTANDARD_DEBUG_ASSERTS=1 \
                -DSTANDARD_DEBUG_BASE_ADDRESS_FOR_ALLOCATIONS=1 \
                -DSTANDARD_DEBUG_PROFILING=1 \
                -DSTANDARD_DEBUG_FILE_LINE_TRACKING=1 \
                -Isrc/standard \
                -Isrc/vendor/khronos \
                "
LINKER_FLAGS="\
              -lpthread \
              -ldl \
              -lX11 \
              "

# compile
echo "-- standard --"
for file in src/standard/*.cpp src/standard/**/*.cpp; do
    if [ -f "$file" ]; then
        echo "$file"
        clang++-18 -c $COMPILER_FLAGS "$file" -o "bin/linux_x86-64/int-standard/$(basename "$file" .cpp).o"
    fi
done

# archive
ar rcs bin/linux_x86-64/standard.a bin/linux_x86-64/int-standard/*.o
if [ $? -eq 0 ]; then
    echo "\"standard.a\""
fi

# clean up
exit 0
