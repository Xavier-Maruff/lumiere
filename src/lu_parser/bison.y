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
    #include <memory>
    class parser_driver;

    using ast_expr_uptr = std::unique_ptr<ast_expr>;
    using ast_int_expr_uptr = std::unique_ptr<ast_int_expr>;
    using ast_flt_expr_uptr = std::unique_ptr<ast_flt_expr>;
    using ast_string_expr_uptr = std::unique_ptr<ast_string_expr>;
    using ast_bin_expr_uptr = std::unique_ptr<ast_bin_expr>;
    using ast_var_expr_uptr = std::unique_ptr<ast_var_expr>;
}

%code {
    //
    #include "driver.hpp"


    ast_expr_uptr make_ast_int_expr_uptr(int64_t value){
        return ast_int_expr_uptr(new ast_int_expr(value));
    }

    ast_expr_uptr make_ast_flt_expr_uptr(double value){
        return ast_flt_expr_uptr(new ast_flt_expr(value));
    }

    ast_expr_uptr make_ast_string_expr_uptr(std::string value){
        return ast_string_expr_uptr(new ast_string_expr(value));
    }

    ast_expr_uptr make_ast_bin_expr_uptr(bin_oper opcode, ast_expr_uptr& lhs, ast_expr_uptr& rhs){
        return ast_bin_expr_uptr(new ast_bin_expr(opcode, lhs, rhs));
    }

    ast_expr_uptr make_ast_var_expr_uptr(std::string name){
        return ast_var_expr_uptr(new ast_var_expr(name));
    }
}

%param {parser_driver& drv}

%locations
//%define parse.trace
%define parse.error verbose
%define parse.lac full

%define api.token.prefix {TOKEN__}

%token <int64_t> INT_LIT;
%token <double> FLT_LIT;
%token <std::string> STR_LIT;
%token <std::string> IDENT
%token ADD "+" MULT "*" NEG "-" DIV "/" MOD "%" OPEN_PAREN "(" CLOSE_PAREN ")"
%token LAMBDA_KW "lambda" ASSIGN "=" EQUIV "==" COMMA "," ARROW "->" OPEN_BRACE "{"
%token CLOSE_BRACE "}" RETURN_KW "return"
%type <ast_expr_uptr> expr;

//TODO: write actual printer
%printer { yyo << $$; } <*>;

%left "+"

%start program
%%

program: %empty
        | statements;

statements: statement
         |  statements statement

statement: dec 
        | dec ASSIGN expr {DEBUG_LOG("New assignment to new variable");}
        | IDENT ASSIGN expr {DEBUG_LOG("New assignment to variable "+$1);}
        | expr

block: OPEN_BRACE statements CLOSE_BRACE {/*TODO: block node class*/;}

decs: dec
    | decs COMMA dec

dec: var_dec 
    | lambda_dec

var_dec: IDENT IDENT {
                        DEBUG_LOG("New "+$1+" variable declaration "+$2);
                    }

lambda_dec: LAMBDA_KW IDENT {
                            DEBUG_LOG("New function "+$2);
                        }

//exprs: expr;
//     | exprs expr;

expr: IDENT {
                $$ = make_ast_var_expr_uptr($1);
                //DEBUG_LOG("Identifier "+$1);
            }
    | INT_LIT {
                $$ = make_ast_int_expr_uptr($1);
                //DEBUG_LOG("New integer literal");
                //$$->gen_code();
            }
    | FLT_LIT {
                $$ = make_ast_flt_expr_uptr($1);
                //DEBUG_LOG("New float literal");
                //$$->gen_code();
            }
    | STR_LIT {
                $$ = make_ast_string_expr_uptr($1);
                //DEBUG_LOG("New string literal");
            }
    //TODO: other binops
    | expr ADD expr {
                $$ = make_ast_bin_expr_uptr(OPER_ADD, $1, $3);
                DEBUG_LOG("New binary expression");
                $$->gen_code();
            }
    | lambda_body

lambda_body: OPEN_PAREN decs CLOSE_PAREN ARROW IDENT block {
                DEBUG_LOG("New complex lambda mapping to "+$5);
            }
            | OPEN_PAREN decs CLOSE_PAREN ARROW IDENT OPEN_PAREN expr CLOSE_PAREN {
                DEBUG_LOG("New simple lambda mapping to "+$5);
            }

%%

void yy::parser::error (const location_type& l, const std::string& m){
    std::stringstream msg("At location: ");
    msg << l << ": " << m;
    stdlog.err() << msg.rdbuf() << std::endl;
}

