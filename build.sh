#!/bin/bash
BASEDIR=$(dirname $0)


if [ -z ${1+x} ]; then
    mkdir build
    cd build
    set -x
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..;
    set +x
else
    mkdir build_$1
    cd build_$1
    set -x
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$1 ..;
    set +x
fi

make
cd $BASEDIR
