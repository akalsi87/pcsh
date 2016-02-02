/**
 * \file ir.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/ir.hpp"

#include "ir_evaluator.hpp"
#include "ir_nodes.hpp"
#include "ir_printer.hpp"
#include "ir_variable_printer.hpp"
#include "tree_cloner.hpp"

namespace pcsh {
namespace ir {

    void tree::print(ostream& os, bool types) const
    {
        printer p(os, types);
        accept(&p);
    }

    const tree& tree::evaluate() const
    {
        evaluator e;
        accept(&e);
        return *this;
    }

    tree::ptr tree::clone() const
    {
        tree_cloner c;
        accept(&c);
        return std::move(c.cloned_tree());
    }

    void tree::print_variables(ostream& os) const
    {
        var_value_printer p(os);
        accept(&p);
    }

}// namespace ir
}// namespace pcsh
