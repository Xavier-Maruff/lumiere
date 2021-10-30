%{ /* -*- C++ -*- */

#include "driver.hpp"
#include "bison.hpp"
#include "cmp_time_extensible.hpp"
#include <iostream>

extern parser_driver drv;

%}

%option noyywrap nounput noinput batch debug

int_lit [1-9]+[0-9]*
flt_lit [1-9]+[0-9]*\.[0-9]+
string_lit (\"(?:[^"\\]|\\.)*\")|(\'(?:[^'\\]|\\.)*\')
identifier [_a-zA-Z][_a-zA-Z0-9]*

%{
    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%
%{
    yy::location& loc = drv.loc;
    loc.step();
%}
[ \t\r]+            loc.step();
\n+                 loc.lines(yyleng); loc.step();
\/\/.*              {;}
{flt_lit}           {
                        return yy::parser::make_FLT_LIT(std::stof(yytext), loc);
                    }
{int_lit}           {
                        return yy::parser::make_INT_LIT(std::stoi(yytext), loc);
                    }
{string_lit}        {
                        std::string yytext_str(yytext);
                        if(yytext_str.size() > 2) yytext_str = yytext_str.substr(1, yytext_str.size()-2);
                        else yytext_str = "";
                        return yy::parser::make_STR_LIT(yytext_str, loc);
                    }
"true"              {
                        return yy::parser::make_BOOL_LIT(true, loc);
                    }
"false"             {
                        return yy::parser::make_BOOL_LIT(false, loc);
                    }
"lambda"            {
                        //store_symbol_type_map_to_buffer();
                        return yy::parser::make_LAMBDA_KW(loc);
                    }
"return"            return yy::parser::make_RETURN_KW(loc);
{identifier}        return yy::parser::make_IDENT(std::string(yytext), loc);
"("                 return yy::parser::make_OPEN_PAREN(loc);
")"                 return yy::parser::make_CLOSE_PAREN(loc);
"{"                 return yy::parser::make_OPEN_BRACE(loc);
"}"                 return yy::parser::make_CLOSE_BRACE(loc);
"+"                 return yy::parser::make_ADD(loc);
"||"                return yy::parser::make_LOG_OR(loc); //from here
"&&"                return yy::parser::make_LOG_AND(loc);
"!="                return yy::parser::make_NOT_EQUIV(loc); // to here
"=="                return yy::parser::make_EQUIV(loc);
"="                 return yy::parser::make_ASSIGN(loc);
"!"                 return yy::parser::make_NOT(loc);
","                 return yy::parser::make_COMMA(loc);
"->"                return yy::parser::make_ARROW(loc);
"-"                 return yy::parser::make_NEG(loc);
"*"                 return yy::parser::make_STAR(loc);
"/"                 return yy::parser::make_SLASH(loc);
"..."               return yy::parser::make_ELLIPS(loc);
<<EOF>>             return yy::parser::make_YYEOF(loc);
.                   throw yy::parser::syntax_error(loc, "Syntax error: unrecognised token "+std::string(yytext));

%%

bool call_parse_from_flex(parser_driver* source){
    yy::parser parse(*source);
    return parse () == 0;
}