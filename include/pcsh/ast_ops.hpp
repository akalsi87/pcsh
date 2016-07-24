/**
 * \file ast_ops.hpp
 * \date Feb 15, 2016
 */

#ifndef PCSH_AST_OPS_HPP
#define PCSH_AST_OPS_HPP

#include "pcsh/exportsym.h"
#include "pcsh/ast.hpp"
#include "pcsh/ostream.hpp"
#include "pcsh/result_type.hpp"

namespace pcsh {
namespace ast {

    PCSH_API void print(const tree* ptree, ostream& os = DEFAULT_OUTPUT_STREAM, bool printvarty = true);

    PCSH_API void print_variables(const tree* ptree, ostream& os = DEFAULT_OUTPUT_STREAM);

    PCSH_API tree::ptr clone(const tree* ptree);

    PCSH_API void evaluate(const tree* ptree);

    struct var_value
    {
        result_type type;
        union {
            int int_val;
            double dbl_val;
            cstring str_val;
        };
    };

    PCSH_API var_value query(const tree* ptree, cstring name);

}//namespace ast
}//namespace pcsh

#endif/*PCSH_AST_OPS_HPP*/
