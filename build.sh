#!/bin/bash
BASEDIR=$(dirname $0)

mkdir build
cd build

if [ -z ${1+x} ]; then
    set -x
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..;
    set +x
else
    set -x
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$1 ..;
    set +x
fi

make
cd $BASEDIR
