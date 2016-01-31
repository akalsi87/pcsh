/**
 * \file ir_printer.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_PRINTER_HPP
#define PCSH_IR_PRINTER_HPP

#include "pcsh/ostream.hpp"

#include "ir_visitor.hpp"

namespace pcsh {
namespace ir {

    class printer : public node_visitor
    {
      public:
        printer(ostream& os) : strm_(os)
        { }
      private:
        ostream& strm_;

        void visit_impl(const variable* v) override;
        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_PRINTER_HPP*/
