/**
 * \file printer.cpp
 * \date Jan 31, 2016
 */

#include "ast/nodes.hpp"
#include "ast/ops/printer.hpp"

namespace pcsh {
namespace ast {

    void printer::print_spacing()
    {
        for (int i = 0; i < nesting_; ++i) {
            strm_ << spacing;
        }
    }

    void printer::print_spacing_newline()
    {
        strm_ << "\n";
        print_spacing();
    }

    void printer::visit_impl(const variable* v)
    {
        strm_ << "<var:" << v->name() << ">";
    }

    void printer::visit_impl(const int_constant* v)
    {
        strm_ << "<int:";
        print(strm_, v) << ">";
    }

    void printer::visit_impl(const float_constant* v)
    {
        strm_ << "<double:";
        print(strm_, v) << ">";
    }

    void printer::visit_impl(const string_constant* v)
    {
        strm_ << "<string:\"";
        print(strm_, v) << "\">";
    }

    void printer::visit_impl(const unary_plus* v)
    {
        strm_ << "(un-plus ";
        v->operand()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const unary_minus* v)
    {
        strm_ << "(un-minus ";
        v->operand()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const unary_not* v)
    {
        strm_ << "(un-not ";
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
    }

    void printer::visit_impl(const block* v)
    {
        {
            strm_ << "(block) at " << v;
            print_types(v);
            ++nesting_;
            print_spacing_newline();
            visit_block_postcbk(v,
                [this](const node* a, bool islast) -> void {
                    if (!islast) {
                        strm_ << "\n";
                        print_spacing();
                    }
                });
            --nesting_;
        }

        if (nesting_ == 0) {
            strm_ << "\n";
        }
    }

    void printer::visit_impl(const if_stmt* v)
    {
        strm_ << "(if-else ";
        v->condition()->accept(this);
        strm_ << " ";
        auto b = v->then_body();

        ++nesting_;

        print_spacing_newline();
        b->accept(this);

        b = v->else_body();
        if (b) {
            strm_ << " ";
            print_spacing_newline();
            b->accept(this);
        }

        --nesting_;
        strm_ << ")";
    }

    void printer::visit_impl(const comp_equals* v)
    {
        strm_ << "(== ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const comp_lt* v)
    {
        strm_ << "(< ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const comp_gt* v)
    {
        strm_ << "(> ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const comp_le* v)
    {
        strm_ << "(<= ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const comp_ge* v)
    {
        strm_ << "(>= ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const logical_and* v)
    {
        strm_ << "(and ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::visit_impl(const logical_or* v)
    {
        strm_ << "(or ";
        v->left()->accept(this);
        strm_ << " ";
        v->right()->accept(this);
        strm_ << ")";
    }

    void printer::print_types(const block* v)
    {
        if (!types_) { return; }
        const auto& tbl = v->table();
        strm_ << " | typemap = { ";
        auto ntvec = symbol_table::all_entries(tbl);
        for (const auto& el : ntvec) {
            strm_ << "<" << el.name << ":" << to_string(el.type) << "> ";
        }
        strm_ << "}";
    }

    ostream& print(ostream& os, const int_constant* v)
    {
        os << v->value();
        return os;
    }

    ostream& print(ostream& os, const float_constant* v)
    {
        os << v->value();
        return os;
    }

    ostream& print(ostream& os, const string_constant* v)
    {
        cstring in = v->value();
        for (; *in; ++in) {
            char c = *in;
            switch (c) {
                case '\n':
                    os << "\\n";
                    break;
                case '\r':
                    os << "\\r";
                    break;
                case '\t':
                    os << "\\t";
                    break;
                case '\\':
                    os << "\\\\";
                    break;
                case '\a':
                    os << "\\a";
                    break;
                case '\b':
                    os << "\\b";
                    break;
                case '\v':
                    os << "\\v";
                    break;
                default:
                    os << c;
                    break;
            }
        }
        return os;
    }

}//namespace ast
}//namespace pcsh

