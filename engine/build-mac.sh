#!/bin/zsh
# Engine build script

set -x

mkdir -p ../bin

# Glob c files
# cFiles=("${(@f)$(find . -type f -name '*.c')}")
cFiles=("${(@f)$(find . -type f \( -name '*.c' -o -name '*.m' \))}")

# echo "Files:" $cFiles

assembly="engine"
compilerFlags=(-g -dynamiclib -fPIC)
# -fms-extensions
# -Wall -Werror
includeFlags=(-Isrc -I$VULKAN_SDK/include)
linkerFlags=(-lvulkan -L$VULKAN_SDK/lib -framework Cocoa -framework Metal -framework QuartzCore)
defines=(-D_DEBUG -DMEXPORT)

echo "Building $assembly..."
clang $cFiles $compilerFlags -o ../bin/lib$assembly.dylib $defines $includeFlags $linkerFlags
