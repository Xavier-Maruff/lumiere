#include "driver.hpp"
#include "log.hpp"
#include "ast.hpp"
#include "llvm_inst.hpp"

#include <cstdio>
#include <llvm/Support/raw_ostream.h>

extern bool yy_flex_debug;
extern FILE* yyin;
extern bool call_parse_from_flex(parser_driver*);

parser_driver::parser_driver(bool is_repl_, bool is_verbose_):
is_repl(is_repl_), is_verbose(is_verbose_){
    //
}

parser_driver::~parser_driver(){
    //
}

//initialize the location
void parser_driver::load_file(std::string filename_){
    try{
        filename = filename_;
        loc.initialize(&filename);
        DEBUG_LOG("Checked source file");
    }
    catch(std::exception& e) {
        stdlog.err() << "Could not load file" << std::endl;
    }
}

//parse the file
void parser_driver::parse(){
    DEBUG_LOG("Parsing "+filename);
    yy_flex_debug = is_verbose;
    if(!is_repl) yyin = fopen(filename.c_str(), "r");
    else yyin = stdin;
    call_parse_from_flex(this);
    fclose(yyin);
    DEBUG_LOG("Finished parsing "+filename);
}

//get the IR as a string
std::string parser_driver::get_ir(){
    std::string ret_string;
    llvm::raw_string_ostream ret_stream(ret_string);
    ret_stream << *llvm_module;
    ret_stream.flush();
    return ret_string;
}