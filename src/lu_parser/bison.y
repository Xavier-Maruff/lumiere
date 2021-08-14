%skeleton "lalr1.cc"
%require "3.7.6"
%defines
%define api.token.raw
%define api.token.constructor
%define api.value.type variant
%define parse.assert

%code requires{
    #include "colours.h"
    #include "ast.hpp"
    #include "log.hpp"
    #include <string>
    #include <sstream>
    #include <iostream>
    class parser_driver;
}

%param {parser_driver& drv}

%locations
//%define parse.trace
%define parse.error verbose
%define parse.lac full

%code {
    #include "driver.hpp"
}


%define api.token.prefix {TOKEN__}

%token <ast_int_expr> INT_LIT;
%token <ast_flt_expr> FLT_LIT;
%token <ast_string_expr> STR_LIT;
%token ADD "+" MULT "*" NEG "-" DIV "/" MOD "%"
%type <ast_expr> expr;

%printer { yyo << $$; } <*>;

%left "+"

%start program
%%

program: exprs;

exprs: expr;
     | exprs expr;

expr: INT_LIT {$$ = ast_int_expr($1); DEBUG_LOG("New integer literal"); INFO_LOG($$.gen_code());}
    | FLT_LIT {$$ = ast_flt_expr($1); DEBUG_LOG("New float literal"); INFO_LOG($$.gen_code());}
    | STR_LIT {$$ = ast_string_expr($1); DEBUG_LOG("New string literal"); INFO_LOG($$.gen_code());}

%%

void yy::parser::error (const location_type& l, const std::string& m){
    std::stringstream msg("At location: ");
    msg << l << ": " << m;
    ERR_LOG(msg.str());
}

