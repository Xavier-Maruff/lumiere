#ifndef LUMIERE_PARSER_DRIVER
#define LUMIERE_PARSER_DRIVER

#include <string>

#include "bison.hpp"

#define YY_DECL yy::parser::symbol_type yylex(parser_driver& lu_driver)
YY_DECL;

//drives the parser
class parser_driver{
    public:
    //parser location
    yy::location loc;
    //input filename
    std::string filename;
    //verbose mode
    bool is_verbose;
    //if repl, no file will be loaded - bison will read from stdin
    bool is_repl;

    parser_driver(bool is_repl_ = false, bool is_verbose_ = false);
    virtual ~parser_driver();

    //load a file to parse
    void load_file(std::string filename_);
    //parse loaded file
    void parse();
    //get the generated ir as a string - probably should improve this a bit
    std::string get_ir();

};

#endif