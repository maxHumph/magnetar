#!/bin/bash
# Build project

set echo on
echo "Building project.."

pushd engine
source build-linux.sh
popd

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error:"$ERROR_LEVEL && exit
fi

pushd test
source build-linux.sh
popd

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
    echo "Error:"$ERROR_LEVEL && exit
fi

echo "Build successful."
