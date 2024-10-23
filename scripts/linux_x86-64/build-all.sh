#!/bin/bash

# clang version check
if ! (command -v clang++-18 || command -v clang++-19) &> /dev/null
then
    echo "clang v18+ required"
    exit 1
fi

build_failed() {
    # ASCII art for failed build
    echo
    echo " *******************************************************************************"
    echo " *     ______     ______     __         ______     __         ______           *"
    echo " *    /\  ___\   /\  ___\   /\ \       /\  __ \   /\ \       /\  ___\          *"
    echo " *    \ \ \____  \ \  __\   \ \ \____  \ \ \/\ \  \ \ \____  \ \___  \         *"
    echo " *     \ \_____\  \ \_____\  \ \_____\  \ \_____\  \ \_____\  \/\_____\        *"
    echo " *      \/_____/   \/_____/   \/_____/   \/_____/   \/_____/   \/_____/        *"
    echo " *                                                                           *"
    echo " *                     BUILD FAILED - CHECK ERRORS ABOVE                     *"
    echo " *******************************************************************************"
    echo
    exit 1
}

echo ""
echo "------------------------------- STARTING BUILD --------------------------------"
echo ""

./build-standard.sh || {
    echo "Build failed during standard library build."
    build_failed
}

echo ""
./build-editor.sh || {
    echo "Build failed during editor build."
    build_failed
}

echo ""
echo "------------------------------- BUILD COMPLETE --------------------------------"
echo ""
exit 0
