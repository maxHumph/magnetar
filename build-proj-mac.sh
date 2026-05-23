#!/bin/zsh

# Exit on error
set -e
set -x   # optional: trace commands

echo "Building project.."

pushd engine >/dev/null
source build-mac.sh
popd >/dev/null

pushd test >/dev/null
source build-mac.sh
popd >/dev/null

echo "Build successful."
