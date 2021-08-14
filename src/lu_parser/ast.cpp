#include "ast.hpp"

//abstract ast node base
ast_node::ast_node(){
    //
}

ast_node::~ast_node(){
    //
}

//ast expression base
ast_expr::ast_expr():
ast_node(){
    //
}

ast_expr::~ast_expr(){
    //
}


//ast variable expression
ast_var_expr::ast_var_expr():
ast_expr(), name(){
    //
}

ast_var_expr::ast_var_expr(std::string name_):
ast_expr(), name(name_){
    //
}

ast_var_expr::~ast_var_expr(){
    //
}


//ast float expression (but its a double)
ast_flt_expr::ast_flt_expr():
ast_expr(), value(){
    //
}

ast_flt_expr::ast_flt_expr(double value_):
ast_expr(), value(value_){
    //
}

ast_flt_expr::~ast_flt_expr(){
    //
}