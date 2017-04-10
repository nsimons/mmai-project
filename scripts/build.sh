#!/bin/bash

pushd .

# folder where script is
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR/..

if [ -d build ]; then
    rm -rf build
fi

mkdir build
cd build
cmake .. && make
popd
