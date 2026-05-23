#!/bin/zsh
# Engine build script

set -x

mkdir -p ../bin

# Glob c files
cFiles=$(find . -type f -name "*.c")

# echo "Files:" $cFiles

assembly="engine"
compilerFlags="-g -dynamiclib -fdeclspec -fPIC"
# -fms-extensions
# -Wall -Werror
includeFlags="-Isrc -I$VULKAN_SDK/include"
linkerFlags="-lvulkan -L$VULKAN_SDK/lib -framework Cocoa -framework Metal -framework QuartzCore"
defines="-D_DEBUG -DKEXPORT"

echo "Building $assembly..."
clang $cFiles $compilerFlags -o ../bin/lib$assembly.dylib $defines $includeFlags $linkerFlags
