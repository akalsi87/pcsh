/**
 * \file ir.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/ir.hpp"

#include "ir_printer.hpp"

namespace pcsh {
namespace ir {

    void tree::print(ostream& os, bool types)
    {
        printer p(os, types);
        accept(&p);
    }

}// namespace ir
}// namespace pcsh
