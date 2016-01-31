/**
 * \file ir_printer.cpp
 * \date Jan 31, 2016
 */

#include "ir_nodes.hpp"
#include "ir_printer.hpp"

namespace pcsh {
namespace ir {

    static const char* spacing = "\n  ";

    void printer::visit_impl(const variable* v)
    {
        strm_ << "<var:" << v->varname() << ">";
    }

    void printer::visit_impl(const int_constant* v)
    {
        strm_ << "<int:" << v->value() << ">";
    }

    void printer::visit_impl(const float_constant* v)
    {
        strm_ << "<double:" << v->value() << ">";
    }

    void printer::visit_impl(const string_constant* v)
    {
        strm_ << "<string:" << v->value() << ">";
    }

    void printer::visit_impl(const unary_minus* v)
    {
        strm_ << "(negate ";
        v->operand()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const binary_div* v)
    {
        strm_ << "(divide ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const binary_minus* v)
    {
        strm_ << "(minus ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const binary_mult* v)
    {
        strm_ << "(mult ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const binary_plus* v)
    {
        strm_ << "(plus ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const assign* v)
    {
        strm_ << "(assign ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")" << spacing;
    }

    void printer::visit_impl(const block* v)
    {
        strm_ << "(block) at " << v << spacing;
        visit_block(v);
        strm_ << "\n";
    }

}//namespace ir
}//namespace pcsh

