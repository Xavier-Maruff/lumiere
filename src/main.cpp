#include <iostream>
#include "lu_parser/driver.hpp"

int main(int argc, char* argv[]){
    parser_driver p;
    p.create_ast();

    return EXIT_SUCCESS;
}