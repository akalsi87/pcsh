/**
 * \file ir_printer.cpp
 * \date Jan 31, 2016
 */

#include "ir_nodes.hpp"
#include "ir_printer.hpp"

namespace pcsh {
namespace ir {

    static const char* spacing = "  ";

    void printer::print_spacing()
    {
        strm_ << "\n";
        for (int i = 0; i != nesting_; ++i) {
            strm_ << spacing;
        }
    }

    void printer::visit_impl(const variable* v)
    {
        strm_ << "<var:" << v->name() << ">";
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
        this->visit(v->var()); //optimize
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
        print_spacing();
    }

    void printer::visit_impl(const block* v)
    {
        for (int i = 0; i != nesting_; ++i) {
            strm_ << spacing;
        }
        ++nesting_;
        strm_ << "(block) at " << v;
        print_spacing();
        visit_block(v);
        --nesting_;
        if (nesting_ == 0) {
            strm_ << "\n";
        }
    }

}//namespace ir
}//namespace pcsh

