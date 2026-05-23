#!/bin/zsh
# Engine build script

set -x

mkdir -p ../bin

# Glob c files
cFiles=$(find . -type f -name "*.c")

# echo "Files:" $cFiles

assembly="test"
compilerFlags="-g -fPIC"
# -fms-extensions
# -Wall -Werror
includeFlags="-Isrc -I../engine/src/"
linkerFlags="-L../bin/ -lengine -Wl,-rpath,@loader_path"
defines="-D_DEBUG -DKEXPORT"

echo "Building $assembly..."
echo clang $cFiles $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
clang $cFiles $compilerFlags -o ../bin/$assembly $defines $includeFlags $linkerFlags
