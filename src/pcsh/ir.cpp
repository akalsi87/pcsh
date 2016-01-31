/**
 * \file ir.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/ir.hpp"

#include "ir_printer.hpp"

namespace pcsh {
namespace ir {

    void tree::print(ostream& os)
    {
        printer p(os);
        accept(&p);
    }

}// namespace ir
}// namespace pcsh
