%{ /* -*- C++ -*- */

#include "driver.hpp"
#include "bison.hpp"
#include <iostream>

extern parser_driver drv;

%}

%option noyywrap nounput noinput batch debug

int_lit [1-9]+[0-9]*
flt_lit [0-9]?[1-9]*\.[0-9]*
string_lit (\"(?:[^"\\]|\\.)*\")|(\'(?:[^'\\]|\\.)*\')

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
{int_lit}           {
                        return yy::parser::make_INT_LIT(std::stoi(yytext), loc);
                    }
{flt_lit}           {
                        return yy::parser::make_FLT_LIT(std::stof(yytext), loc);
                    }
{string_lit}        {
                        std::string yytext_str(yytext);
                        if(yytext_str.size() > 2) yytext_str = yytext_str.substr(1, yytext_str.size()-2);
                        else yytext_str = "";
                        return yy::parser::make_STR_LIT(yytext_str, loc);
                    }
"+"                 return yy::parser::make_ADD(loc);
<<EOF>>             return yy::parser::make_YYEOF(loc);
.                   throw yy::parser::syntax_error(loc, "Syntax error: unrecognised token "+std::string(yytext));

%%

bool call_parse_from_flex(parser_driver* source){
    yy::parser parse(*source);
    return parse () == 0;
}