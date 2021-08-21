set -x
set -e
./build_Debug/bin/lumiere ./test/1/1.lu -o ./test/1/1.ll -v
clang++ ./test/1/1.ll ./test/1/1.cpp -o ./test/1/1 -g --std=c++2a -g
./test/1/1
set +x