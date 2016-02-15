/**
 * \file ir_printer.cpp
 * \date Jan 31, 2016
 */

#include "ir_nodes.hpp"
#include "ir_printer.hpp"

namespace pcsh {
namespace ir {

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
            strm_ << "\n\n";
        }
    }

    void printer::visit_impl(const if_stmt* v)
    {
        strm_ << "(if-cond-body ";
        v->condition()->accept(this);
        strm_ << " ";
        auto b = v->body();
        ++nesting_;
        if (dynamic_cast<block*>(b)) {
            print_spacing_newline();
            b->accept(this);
        } else {
            b->accept(this);
        }
        --nesting_;
        strm_ << ")";
    }

    void printer::visit_impl(const comp_equals* v)
    {
        strm_ << "(eq ";
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
                default:
                    os << c;
                    break;
            }
        }
        return os;
    }

}//namespace ir
}//namespace pcsh

