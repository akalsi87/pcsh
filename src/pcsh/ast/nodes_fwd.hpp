/**
 * \file nodes_fwd.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_AST_NODES_FWD_HPP
#define PCSH_AST_NODES_FWD_HPP

namespace pcsh {
namespace ast {

    // atoms
    class variable;

    class type_constant;

    class int_constant;

    class float_constant;

    class string_constant;

    // unary ops
    class unary_plus;

    class unary_minus;

    class unary_not;

    // binary ops
    class binary_div;

    class binary_minus;

    class binary_mult;

    class binary_plus;

    class assign;

    class comp_equals;

    class comp_lt;

    class comp_gt;

    class comp_le;

    class comp_ge;

    class logical_and;

    class logical_or;

    class var_decl;

    // container
    class block;

    // keyword statements
    class if_stmt;

}//namespace ast
}//namespace pcsh

#endif/*PCSH_AST_NODES_FWD_HPP*/
