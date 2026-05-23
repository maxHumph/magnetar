#!/bin/bash
# Engine build script

set echo on

mkdir -p ../bin

# Glob c files
cFiles=$(find . -type f -name "*.c")

# echo "Files:" $cFiles

assembly="engine"
compilerFlags="-g -shared -fdeclspec -fPIC"
# -fms-extensions
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -lxcb -lX11 -lX11-xcb -lxkbcommon -L$VULKAN_SDK/lib -L/usr/X11R6/lib"
defines="-D_DEBUG -DKEXPORT"

echo "Building $assembly..."
clang $cFiles $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags
