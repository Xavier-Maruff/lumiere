set -x
set -e
./build/bin/lumiere ./test/1.lu -o ./test/1.ll
clang++ ./test/1.ll ./test/1.cpp -o ./test/1 -g --std=c++2a
./test/1
set +x