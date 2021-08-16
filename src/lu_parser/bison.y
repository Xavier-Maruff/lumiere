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
    using ast_func_call_expr_uptr = std::unique_ptr<ast_func_call_expr>;
    using ast_func_proto_uptr = std::unique_ptr<ast_func_proto>;
    using ast_block_uptr = std::unique_ptr<ast_block>;
    using ast_node_uptr = std::unique_ptr<ast_node>;
    using ast_func_block_uptr = std::unique_ptr<ast_func_block>;
    using ast_func_def_uptr = std::unique_ptr<ast_func_def>;
    
    using str_pair = std::pair<std::string, std::string>;
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

    ast_var_expr_uptr make_ast_var_expr_uptr(std::string var_type, std::string name){
        return ast_var_expr_uptr(new ast_var_expr(var_type, name));
    }

    ast_expr_uptr make_ast_func_call_expr_uptr(std::string callee, std::vector<ast_expr_uptr>& args){
        return ast_func_call_expr_uptr(new ast_func_call_expr(callee, args));
    }

    ast_func_proto_uptr make_ast_func_proto_uptr(std::string name, std::vector<ast_node_uptr>& args, std::string return_type){
        return ast_func_proto_uptr(new ast_func_proto(name, args, return_type));
    }

    ast_func_proto_uptr make_ast_func_proto_uptr(std::string name, std::string return_type){
        return ast_func_proto_uptr(new ast_func_proto(name, return_type));
    }

    ast_block_uptr make_ast_block_uptr(std::vector<ast_node_uptr>& children){
        return ast_block_uptr(new ast_block(children));
    }

    ast_func_block_uptr make_ast_func_block_uptr(std::vector<ast_node_uptr>& children, ast_expr_uptr& return_expr){
        return ast_func_block_uptr(new ast_func_block(children, return_expr));
    }

    ast_func_block_uptr make_ast_func_block_uptr(ast_expr_uptr& return_expr){
        return ast_func_block_uptr(new ast_func_block(return_expr));
    }

    ast_func_def_uptr make_ast_func_def_uptr(ast_func_proto_uptr& func_proto, ast_node_uptr& func_body){
        return ast_func_def_uptr(new ast_func_def(func_proto, func_body));
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
%type <ast_func_proto_uptr> lambda_dec;
%type <ast_func_def_uptr> lambda_def;
%type <ast_node_uptr> dec
%type <ast_var_expr_uptr> var_dec;
%type <std::vector<ast_node_uptr>> decs;
%type <std::vector<ast_expr_uptr>> lambda_args;
%type <ast_node_uptr> lambda_body;
%type <ast_block_uptr> block;
%type <std::vector<ast_node_uptr>> statements;
%type <ast_node_uptr> statement;
%type <ast_func_block_uptr> lambda_block;


//TODO: write actual printer
%printer { yyo << $$; } <*>;

%left "+"

%start program
%%

program: %empty
        | statements {
                for(ast_node_uptr& ast_statement: $1){
                    ast_statement->gen_code();
                }
            };

statements: statement {
                $$ = std::vector<ast_node_uptr>();
                $$.push_back(std::move($1));
            }
         |  statements statement {
                $1.push_back(std::move($2));
                $$ = std::move($1);
            }

statement: dec {
                $$ = std::move($1);
            }
        | var_dec ASSIGN expr {
                //TODO: mutable variables
                DEBUG_LOG("New assignment to new variable "+$1->name+" of type "+$1->cmp_node_type);
                //add to values map
                value_map[$1->name] = $3->gen_code();
                $$ = std::move($1);
            }
        | IDENT ASSIGN expr {
                //TODO: this can't do anything until mutable variables are added
                DEBUG_LOG("(FUTURE) New assignment to variable "+$1);
            }
        | expr {
                $$ = std::move($1);
            }

block: OPEN_BRACE statements CLOSE_BRACE {
                $$ = make_ast_block_uptr($2);
            }

decs: dec {
        $$ = std::vector<ast_node_uptr>();
        $$.push_back(std::move($1));
    }
    | decs COMMA dec {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }

dec: var_dec {
            $$ = std::move($1);
        }
    | lambda_dec {
            $$ = std::move($1);
            //$$->gen_code();
        }
    | lambda_def {
            $$ = std::move($1);
            //$$->gen_code();
        }

var_dec: IDENT IDENT {
                        $$ = make_ast_var_expr_uptr($1, $2);
                        symbol_type_map[$$->name] = $$->cmp_node_type;
                        DEBUG_LOG("New "+$1+" variable declaration "+$2+" loaded as "+symbol_type_map[$$->name]+", "+$$->name);
                    }

lambda_dec: LAMBDA_KW IDENT IDENT OPEN_PAREN decs CLOSE_PAREN {
                            DEBUG_LOG("New function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $5, $2);
                        }
            | LAMBDA_KW IDENT IDENT OPEN_PAREN CLOSE_PAREN {
                            DEBUG_LOG("New noarg function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $2);
                        }

lambda_def:  lambda_dec ARROW lambda_body {
                    //TODO:
                    $$ = make_ast_func_def_uptr($1, $3);
                }
            //| IDENT ASSIGN OPEN_PAREN decs CLOSE_PAREN ARROW lambda_body TODO:

lambda_body: expr {
                DEBUG_LOG("New lambda expression");
                $$ = std::move($1);
            }
            | lambda_block  {
                    DEBUG_LOG("New lambda body");
                    $$ = std::move($1);
                }
            

lambda_block: OPEN_BRACE statements RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOG("New lambda block");
                    $$ = make_ast_func_block_uptr($2, $4);
                }
            | OPEN_BRACE RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOG("New single ret lambda block");
                    $$ = make_ast_func_block_uptr($3);
            }

expr: IDENT {
                $$ = make_ast_var_expr_uptr(symbol_type_map[$1], $1);
            }
    | INT_LIT {
                $$ = make_ast_int_expr_uptr($1);
                $$->cmp_node_type = "int";
            }
    | FLT_LIT {
                $$ = make_ast_flt_expr_uptr($1);
                $$->cmp_node_type = "float";
            }
    | STR_LIT {
                $$ = make_ast_string_expr_uptr($1);
                $$->cmp_node_type = "string";
            }
    //TODO: other binops
    | expr ADD expr {
                $$ = make_ast_bin_expr_uptr(OPER_ADD, $1, $3);
                DEBUG_LOG("New binary expression");
            }
    | IDENT OPEN_PAREN lambda_args CLOSE_PAREN {
        DEBUG_LOG("New lambda "+$1+" call, with "+std::to_string($3.size())+" args");
        $$ = make_ast_func_call_expr_uptr($1, $3);
    }

lambda_args: expr {
                $$ = std::vector<ast_expr_uptr>();
                $$.push_back(std::move($1));
            }
        | lambda_args COMMA expr {
                $1.push_back(std::move($3));
                $$ = std::move($1);
            }
        | %empty {
                $$ = std::vector<ast_expr_uptr>();
            }

%%

void yy::parser::error (const location_type& l, const std::string& m){
    std::stringstream msg("At location: ");
    msg << l << ": " << m;
    stdlog.err() << msg.rdbuf() << std::endl;
}

