#include "driver.hpp"
#include "log.hpp"
#include "ast.hpp"

extern bool yy_flex_debug;
extern FILE* yyin;
extern bool call_parse_from_flex(parser_driver*);

parser_driver::parser_driver(){
    //
}

parser_driver::~parser_driver(){
    //
}

void parser_driver::create_ast(){
    //
}

void parser_driver::gen_bytecode(){
    //
}

void parser_driver::load_file(std::string filename_){
    try{
        filename = filename_;
        loc.initialize(&filename);
        DEBUG_LOG("Checked source file");
    }
    catch(std::exception& e) {
        ERR_LOG("Could not load file");
    }
}

void parser_driver::parse(){
    DEBUG_LOG("Parsing "+filename);
    yy_flex_debug = true;
    yyin = fopen(filename.c_str(), "r");
    call_parse_from_flex(this);
    fclose(yyin);
    DEBUG_LOG("Finished parsing "+filename);
}