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
    #include "llvm_inst.hpp"
    #include "err_codes.h"
    #include <string>
    #include <sstream>
    #include <iostream>
    #include <memory>
    class parser_driver;
    
    using str_pair = std::pair<std::string, std::string>;
    
    using ast_expr_uptr = std::unique_ptr<ast_expr>;
    using ast_int_expr_uptr = std::unique_ptr<ast_int_expr>;
    using ast_flt_expr_uptr = std::unique_ptr<ast_flt_expr>;
    using ast_string_expr_uptr = std::unique_ptr<ast_string_expr>;
    using ast_bin_expr_uptr = std::unique_ptr<ast_bin_expr>;
    using ast_lhs_ptr_bin_expr_uptr = std::unique_ptr<ast_lhs_ptr_bin_expr>;
    using ast_unary_expr_uptr = std::unique_ptr<ast_unary_expr>;
    using ast_var_expr_uptr = std::unique_ptr<ast_var_expr>;
    using ast_func_call_expr_uptr = std::unique_ptr<ast_func_call_expr>;
    using ast_func_proto_uptr = std::unique_ptr<ast_func_proto>;
    using ast_block_uptr = std::unique_ptr<ast_block>;
    using ast_node_uptr = std::unique_ptr<ast_node>;
    using ast_func_block_uptr = std::unique_ptr<ast_func_block>;
    using ast_func_def_uptr = std::unique_ptr<ast_func_def>;
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

    ast_expr_uptr make_ast_lhs_ptr_bin_expr_uptr(bin_oper opcode, std::string lhs, ast_expr_uptr& rhs){
        return ast_lhs_ptr_bin_expr_uptr(new ast_lhs_ptr_bin_expr(opcode, lhs, rhs));
    }

    ast_expr_uptr make_ast_unary_expr_uptr(unary_oper opcode, ast_expr_uptr& target){
        return ast_unary_expr_uptr(new ast_unary_expr(opcode, target));
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

    ast_func_def_uptr make_ast_func_def_uptr(ast_func_proto_uptr& func_proto, ast_func_block_uptr& func_body){
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
%token ADD "+" STAR "*" NEG "-" SLASH "/" MOD "%" OPEN_PAREN "(" CLOSE_PAREN ")"
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
//%type <ast_block_uptr> block;
%type <std::vector<ast_node_uptr>> statements;
%type <ast_node_uptr> statement;
%type <ast_func_block_uptr> lambda_block;


//TODO: write actual printer
%printer { yyo << $$; } <*>;

%right IDENT
%right ASSIGN
%left ADD NEG
%left STAR SLASH
%nonassoc UNEG
%nonassoc EXPR_GROUP

%start program
%%

program: %empty {
                stdlog.err() << "Empty program" << std::endl;
            }
        | statements{
                for(auto& a_node: $1){
                    if(a_node != nullptr) a_node->gen_code();
                    else stdlog.warn() << "Null program child" << std::endl;
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
        | expr {
                    $$ = std::move($1);
                }
            | lambda_def {
                    $$ = std::move($1);
                    //$$->gen_code();
                }

/*
block: OPEN_BRACE statements CLOSE_BRACE {
                $$ = make_ast_block_uptr($2);
            }
*/

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

var_dec: IDENT IDENT /*%prec DECL_TOK*/{
                        //DEBUG_LOGL(@1, "Assignment to new variable "+$2+" of type "+$1);
                        $$ = make_ast_var_expr_uptr($1, $2);
                        symbol_type_map[$$->name] = $$->cmp_node_type;
                        DEBUG_LOGL(@1, $1+" variable declaration "+$2+" loaded as "+symbol_type_map[$$->name]+", "+$$->name);
                }
        | IDENT IDENT ASSIGN expr{
                //DEBUG_LOGL(@1, "Assignment to new variable "+$2+" of type "+$1);
                $$ = make_ast_var_expr_uptr($1, $2);
                symbol_type_map[$$->name] = $$->cmp_node_type;
                $$->set_init_val($4);
                DEBUG_LOGL(@1, $1+" variable declaration "+$2+" loaded as "+symbol_type_map[$$->name]+", "+$$->name);
            }

lambda_dec: LAMBDA_KW IDENT IDENT OPEN_PAREN decs CLOSE_PAREN {
                            DEBUG_LOGL(@1, "Function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $5, $2);
                        }
            | LAMBDA_KW IDENT IDENT OPEN_PAREN CLOSE_PAREN {
                            DEBUG_LOGL(@1, "Noarg function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $2);
                        }

lambda_def:  lambda_dec ARROW lambda_body {
                    //TODO:
                    $$ = make_ast_func_def_uptr($1, $3);
                    std::cout << std::endl << std::endl;
                }
            //| IDENT ASSIGN OPEN_PAREN decs CLOSE_PAREN ARROW lambda_body TODO:

lambda_body: expr {
                    DEBUG_LOGL(@1, "Lambda expression");
                    $$ = std::move($1);
                }
            | lambda_block  {
                    DEBUG_LOGL(@1, "Lambda body");
                    $$ = std::move($1);
                }
            

lambda_block: OPEN_BRACE statements RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOGL(@1, "Lambda block");
                    $$ = make_ast_func_block_uptr($2, $4);
                }
            | OPEN_BRACE RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOGL(@1, "Single ret lambda block");
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
    | NEG expr %prec UNEG {
        DEBUG_LOGL(@1, "Unary expression with target type "+$2->cmp_node_type);
        $$ = make_ast_unary_expr_uptr(U_OPER_NEG, $2);
    }
    //TODO: mod
    | expr ADD expr {
                $$ = make_ast_bin_expr_uptr(OPER_ADD, $1, $3);
                DEBUG_LOGL(@1, "Binary expression");
            }
    | expr NEG expr {
                $$ = make_ast_bin_expr_uptr(OPER_SUB, $1, $3);
                DEBUG_LOGL(@1, "Binary expression");
            }
    | expr STAR expr {
                $$ = make_ast_bin_expr_uptr(OPER_MULT, $1, $3);
                DEBUG_LOGL(@1, "Binary expression");
            }
    | expr SLASH expr {
                $$ = make_ast_bin_expr_uptr(OPER_DIV, $1, $3);
                DEBUG_LOGL(@1, "Binary expression");
            }
    /*| var_dec ASSIGN expr {
                DEBUG_LOGL(@1, "Assignment to new variable "+$1->name+" of type "+$1->cmp_node_type);
                $1->set_init_val($3);
                $$ = std::move($1);
            }*/
    | IDENT ASSIGN expr {
            DEBUG_LOGL(@1, "Assignment to mutable variable "+$1);
            /*auto var_iter = symbol_type_map.find($1);
            if(var_iter == symbol_type_map.end()) {
                //TODO: error logging with better 
                stdlog.err() << "Assignment to uninitialised variable " << $1 << std::endl;
                throw PARSE_ERR;
            }
            ast_expr_uptr temp_var = make_ast_var_expr_uptr(var_iter->second, $1);*/
            //llvm::Value* var_ptr = value_map[$1];
            //if(var_ptr == nullptr){
            //    stdlog.err() << "Variable " << $1 << " has not associated alloca instance" << std::endl;
            //    throw PARSE_ERR;
            //}
            $$ = make_ast_lhs_ptr_bin_expr_uptr(OPER_ASSIGN, $1, $3);
        }
	//function call
    | IDENT OPEN_PAREN lambda_args CLOSE_PAREN {
        DEBUG_LOGL(@1, "Lambda "+$1+" call, with "+std::to_string($3.size())+" args");
        $$ = make_ast_func_call_expr_uptr($1, $3);
    }
	//grouping expressions - currently fucking everything up
    | OPEN_PAREN expr %prec EXPR_GROUP CLOSE_PAREN {
			DEBUG_LOGL(@1, "Grouped expression");
            $$ = std::move($2);
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
    std::stringstream msg;
    msg << ANSI_CYAN << l << ANSI_RESET << "\t" << m;
    stdlog.err() << msg.rdbuf() << std::endl;
    throw PARSE_ERR;
}

