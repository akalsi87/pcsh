/**
 * \file ir_operations.hpp
 * \date Feb 15, 2016
 */

#ifndef PCSH_IR_OPERATIONS_HPP
#define PCSH_IR_OPERATIONS_HPP

#include "pcsh/exportsym.h"
#include "pcsh/ir.hpp"
#include "pcsh/ostream.hpp"
#include "pcsh/result_type.hpp"

namespace pcsh {
namespace ir {

    PCSH_API void print(const tree* ptree, ostream& os, bool printvarty = true);

    PCSH_API void print_variables(const tree* ptree, ostream& os);

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

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_OPERATIONS_HPP*/
