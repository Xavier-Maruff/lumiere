set -x
set -e
./build_Debug/bin/lumiere ./test/2.lu -o ./test/2.ll
clang++ ./test/2.ll ./test/2.cpp -o ./test/2 -g --std=c++2a
set +x