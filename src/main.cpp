#include <iostream>
#include "driver.hpp"

parser_driver drv;

int main(int argc, char* argv[]){
    drv.load_file(argv[1]);
    drv.parse();

    return EXIT_SUCCESS;
}