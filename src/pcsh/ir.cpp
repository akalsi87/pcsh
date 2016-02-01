/**
 * \file ir.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/ir.hpp"

#include "ir_printer.hpp"
#include "ir_evaluator.hpp"

namespace pcsh {
namespace ir {

    void tree::print(ostream& os, bool types)
    {
        printer p(os, types);
        accept(&p);
    }

    tree::ptr tree::evaluate() const
    {
        evaluator e;
        accept(&e);
        return std::move(e.evaluated_tree());
    }

}// namespace ir
}// namespace pcsh
