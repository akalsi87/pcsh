/**
 * \file printer.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_AST_PRINTER_HPP
#define PCSH_AST_PRINTER_HPP

#include "pcsh/ostream.hpp"

#include "ast/visitor.hpp"

namespace pcsh {
namespace ast {

    static const char* const spacing = "  ";

    ostream& print(ostream& os, const int_constant* v);

    ostream& print(ostream& os, const float_constant* v);

    ostream& print(ostream& os, const string_constant* v);

    class printer final : public node_visitor
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
        void visit_impl(const unary_not* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const comp_equals* v) override;
        void visit_impl(const comp_le* v) override;
        void visit_impl(const comp_ge* v) override;
        void visit_impl(const comp_lt* v) override;
        void visit_impl(const comp_gt* v) override;
        void visit_impl(const block* v) override;
        void visit_impl(const if_stmt* v) override;

        void print_types(const block* v);
        void print_spacing_newline();
        void print_spacing();
    };

}//namespace ast
}//namespace pcsh

#endif/*PCSH_AST_PRINTER_HPP*/
