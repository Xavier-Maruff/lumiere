#ifndef LUMIERE_PARSER_DRIVER
#define LUMIERE_PARSER_DRIVER

#include <string>

class parser_driver{
    public:

    parser_driver();
    ~parser_driver();

    void create_ast();
    void gen_bytecode();

};

#endif