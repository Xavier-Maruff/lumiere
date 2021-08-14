#ifndef LUMIERE_PARSER_DRIVER
#define LUMIERE_PARSER_DRIVER

#include <string>

#include "bison.hpp"

#define YY_DECL yy::parser::symbol_type yylex(parser_driver& lu_driver)
YY_DECL;

class parser_driver{
    public:
    yy::location loc;
    std::string filename;

    parser_driver();
    virtual ~parser_driver();

    void create_ast();
    void gen_bytecode();
    void load_file(std::string filename_);
    void parse();

};

#endif