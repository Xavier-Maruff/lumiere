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
    #include "cmp_time_extensible.hpp"
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
    //using ast_lhs_ptr_bin_expr_uptr = std::unique_ptr<ast_lhs_ptr_bin_expr>;
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

    //ast_expr_uptr make_ast_lhs_ptr_bin_expr_uptr(bin_oper opcode, std::string lhs, ast_expr_uptr& rhs){
    //    return ast_lhs_ptr_bin_expr_uptr(new ast_lhs_ptr_bin_expr(opcode, lhs, rhs));
    //}

    ast_expr_uptr make_ast_unary_expr_uptr(unary_oper opcode, ast_expr_uptr& target){
        return ast_unary_expr_uptr(new ast_unary_expr(opcode, target));
    }

    ast_expr_uptr make_ast_var_expr_uptr(std::string var_type, std::string name){
        return ast_var_expr_uptr(new ast_var_expr(var_type, name));
    }

    ast_expr_uptr make_ast_var_expr_uptr(std::string name){
        return ast_var_expr_uptr(new ast_var_expr(name));
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

    ast_func_block_uptr make_ast_func_block_uptr(std::vector<ast_node_uptr>& children){
        return ast_func_block_uptr(new ast_func_block(children));
    }

    ast_func_block_uptr make_ast_func_block_uptr(ast_expr_uptr& return_expr){
        return ast_func_block_uptr(new ast_func_block(return_expr));
    }

    ast_func_block_uptr make_ast_func_block_uptr(){
        return ast_func_block_uptr(new ast_func_block());
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
%token CLOSE_BRACE "}" RETURN_KW "return" ELLIPS "..."

%type <ast_expr_uptr> expr;
%type <ast_func_proto_uptr> lambda_dec;
%type <ast_func_def_uptr> lambda_def;
%type <ast_node_uptr> dec
%type <ast_expr_uptr> var_dec;
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
%nonassoc DECL_TOK

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

var_dec: IDENT IDENT %prec DECL_TOK{
                        //DEBUG_LOGL(@$, "Assignment to new variable "+$2+" of type "+$1);
                        $$ = make_ast_var_expr_uptr($1, $2);
                        node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                        DEBUG_LOGL(@$, $1+" variable declaration "+$2+" loaded as "+symbol_type_map[$$->name]+", "+$$->name);
                }
        | IDENT IDENT ASSIGN expr{
                //DEBUG_LOGL(@$, "Assignment to new variable "+$2+" of type "+$1);
                $$ = make_ast_var_expr_uptr($1, $2);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                $$->set_init_val($4);
                DEBUG_LOGL(@$, $1+" variable initial assignment "+$2+" loaded as "+symbol_type_map[$$->name]+", "+$$->name);
            }

lambda_dec: LAMBDA_KW IDENT IDENT OPEN_PAREN decs CLOSE_PAREN {
                            DEBUG_LOGL(@$, "Function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $5, $2);
                            node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                        }
            | LAMBDA_KW IDENT IDENT OPEN_PAREN CLOSE_PAREN {
                            DEBUG_LOGL(@$, "Noarg function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $2);
                            node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                        }
            | LAMBDA_KW IDENT IDENT OPEN_PAREN decs ELLIPS CLOSE_PAREN {
                            DEBUG_LOGL(@$, "Variadic function "+$3+" returning a "+$2);
                            $$ = make_ast_func_proto_uptr($3, $5, $2);
                            node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                            $$->variadic = true;
                        }

lambda_def:  lambda_dec ARROW lambda_body {
                    $$ = make_ast_func_def_uptr($1, $3);
                    //load_symbol_type_map_from_buffer();
                }
            //| IDENT ASSIGN OPEN_PAREN decs CLOSE_PAREN ARROW lambda_body TODO:

lambda_body: expr {
                    DEBUG_LOGL(@$, "Lambda expression");
                    $$ = std::move($1);
                }
            | lambda_block  {
                    DEBUG_LOGL(@$, "Lambda body");
                    $$ = std::move($1);
                }
            

lambda_block: OPEN_BRACE statements RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOGL(@$, "Lambda block");
                    $$ = make_ast_func_block_uptr($2, $4);
                    node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                }
            | OPEN_BRACE CLOSE_BRACE {
                    DEBUG_LOGL(@$, "Void empty lambda block");
                    $$ = make_ast_func_block_uptr();
                    node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                }
            | OPEN_BRACE statements CLOSE_BRACE {
                    DEBUG_LOGL(@$, "Void noret lambda block");
                    $$ = make_ast_func_block_uptr($2);
                    node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                }
            | OPEN_BRACE RETURN_KW expr CLOSE_BRACE {
                    DEBUG_LOGL(@$, "Single ret lambda block");
                    $$ = make_ast_func_block_uptr($3);
                    node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
            }

expr: IDENT {
                $$ = make_ast_var_expr_uptr($1);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
            }
    | INT_LIT {
                $$ = make_ast_int_expr_uptr($1);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                $$->cmp_node_type = "int";
            }
    | FLT_LIT {
                $$ = make_ast_flt_expr_uptr($1);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                $$->cmp_node_type = "float";
            }
    | STR_LIT {
                $$ = make_ast_string_expr_uptr($1);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                $$->cmp_node_type = "string";
            }
    | NEG expr %prec UNEG {
        DEBUG_LOGL(@$, "Unary expression with target type "+$2->cmp_node_type);
        $$ = make_ast_unary_expr_uptr(U_OPER_NEG, $2);
        node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
    }
    //TODO: mod
    | expr ADD expr {
                $$ = make_ast_bin_expr_uptr(OPER_ADD, $1, $3);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                DEBUG_LOGL(@$, "Binary expression");
            }
    | expr NEG expr {
                $$ = make_ast_bin_expr_uptr(OPER_SUB, $1, $3);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                DEBUG_LOGL(@$, "Binary expression");
            }
    | expr STAR expr {
                $$ = make_ast_bin_expr_uptr(OPER_MULT, $1, $3);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                DEBUG_LOGL(@$, "Binary expression");
            }
    | expr SLASH expr {
                $$ = make_ast_bin_expr_uptr(OPER_DIV, $1, $3);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                DEBUG_LOGL(@$, "Binary expression");
            }
    /*| var_dec ASSIGN expr {
                DEBUG_LOGL(@$, "Assignment to new variable "+$1->name+" of type "+$1->cmp_node_type);
                $1->set_init_val($3);
                $$ = std::move($1);
            }*/
    | IDENT ASSIGN expr {
            DEBUG_LOGL(@$, "Assignment to mutable variable "+$1);
            //auto symbol_iter = declared_symbols.find($1);
            //auto symbol_type_iter = symbol_type_map.find($1);
            //if(symbol_type_iter != symbol_type_map.end()){
                ast_expr_uptr var_ref = make_ast_var_expr_uptr($1);
                $$ = make_ast_bin_expr_uptr(OPER_ASSIGN, var_ref, $3);
                node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
                DEBUG_LOGL(@$, "Binary expression");
            //}
            //else {
            //    stdlog.err() << ANSI_CYAN << @$ << ANSI_RESET << "\t Assignment to undeclared variable " << $1 << std::endl;
            //    throw PARSE_ERR;
            //}
        }
	//function call
    | IDENT OPEN_PAREN lambda_args CLOSE_PAREN {
        DEBUG_LOGL(@$, "Lambda "+$1+" call, with "+std::to_string($3.size())+" args");
        $$ = make_ast_func_call_expr_uptr($1, $3);
        node_id_source_info_map[$$->node_id] = error_loc_info(*@$.begin.filename, @$.begin.line, @$.begin.column, @$.end.line, @$.end.column);
    }
	//grouping expressions - currently fucking everything up
    | OPEN_PAREN expr %prec EXPR_GROUP CLOSE_PAREN {
			DEBUG_LOGL(@$, "Grouped expression");
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
    stdlog.err() << ANSI_CYAN << *l.begin.filename << ", line " << l.begin.line << ":" << l.begin.column
    << " - " << l.end.line << ":" << l.end.column  << ANSI_RESET << " -> " << m << std::endl;
    throw PARSE_ERR;
}

