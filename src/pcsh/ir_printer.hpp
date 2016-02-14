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

    static const char* const spacing = "  ";

    ostream& print(ostream& os, const int_constant* v);

    ostream& print(ostream& os, const float_constant* v);

    ostream& print(ostream& os, const string_constant* v);

    class printer : public node_visitor
    {
      public:
        printer(ostream& os, bool types) : strm_(os), nesting_(0), types_(types)
        { }
      private:
        ostream& strm_;
        int nesting_;
        bool types_;

        void visit_impl(const variable* v) override;
        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_plus* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;

        void print_types(const block* v);
        void print_spacing_newline();
        void print_spacing();
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_PRINTER_HPP*/
