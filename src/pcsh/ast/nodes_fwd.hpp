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

    class int_constant;

    class float_constant;

    class string_constant;

    // unary ops
    class unary_plus;

    class unary_minus;

    // binary ops
    class binary_div;

    class binary_minus;

    class binary_mult;

    class binary_plus;

    class assign;

    class comp_equals;

    // container
    class block;

    // keyword statements
    class if_stmt;

}//namespace ast
}//namespace pcsh

#endif/*PCSH_AST_NODES_FWD_HPP*/
