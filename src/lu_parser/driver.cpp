#include "driver.hpp"
#include "log.hpp"
#include "ast.hpp"
#include "llvm_inst.hpp"

#include <cstdio>
#include <llvm/Support/raw_ostream.h>
#include <fstream>
#include <system_error>

extern bool yy_flex_debug;
extern FILE* yyin;
extern bool call_parse_from_flex(parser_driver*);

parser_driver::parser_driver(bool is_repl_, bool is_verbose_):
is_repl(is_repl_), is_verbose(is_verbose_){
    yy_flex_debug = false;
}

parser_driver::~parser_driver(){
    //
}

//initialize the location
void parser_driver::load_file(std::string filename_){
    filename = filename_;
    loc.initialize(&filename);
    std::ifstream file_check(filename);
    if(!file_check.is_open()){
        stdlog.err() << "Could not open source file " << filename << std::endl;
        throw FILE_OPEN_ERR;
    }
}

//parse the file
void parser_driver::parse(){
    DEBUG_LOG("Parsing "+filename);
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

void parser_driver::write_ir(std::string output_filename){
    std::error_code llvm_err_code;
    llvm::raw_fd_ostream file_outstream(llvm::StringRef(output_filename.c_str()), llvm_err_code);
    file_outstream << *llvm_module;
    file_outstream.flush();
    if(llvm_err_code != std::error_code()) {
        stdlog.err() << "Could not write to output file "  << output_filename << std::endl;
    }
    return;
}