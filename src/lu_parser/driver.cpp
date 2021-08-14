#include "driver.hpp"
#include "debug.hpp"
#include "ast.hpp"

parser_driver::parser_driver(){
    DEBUG_LOG("driver created");
}

parser_driver::~parser_driver(){
    //
}

void parser_driver::create_ast(){
    ast_expr expr();
    DEBUG_LOG("ast created");
}

void parser_driver::gen_bytecode(){
    //
}