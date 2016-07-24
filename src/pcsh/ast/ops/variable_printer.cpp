/**
 * \file variable_printer.cpp
 * \date Feb 01, 2016
 */

#include "ast/nodes.hpp"
#include "ast/ops/printer.hpp"
#include "ast/ops/variable_printer.hpp"

namespace pcsh {
namespace ast {

    void var_value_printer::visit_impl(const block* v)
    {
        if (!prn_) {
            prn_ = new printer(strm_, true);
        } else {
            strm_ << "\n";
        }

        print_spacing();
        strm_ << "(block) at " << v;

        auto oldtbl = tbl_;
        ++nesting_;
        print_spacing();
        tbl_ = &v->table();
        {// visit this block
            {// print this block
                auto nv = symbol_table::all_entries(*tbl_);
                variable tmp(nullptr);
                for (const auto& el : nv) {
                    tmp.set_name(el.name);
                    strm_ << "\n";
                    print_spacing();
                    tmp.accept(prn_);
                    strm_ << " -> ";
                    if (el.evaluated) {
                        symbol_table::lookup((*tbl_), &tmp).ptr->accept(prn_);
                    } else {
                        strm_ << "<unassigned>";
                    }
                }
            }
            visit_block(v);
        }

        --nesting_;
        tbl_ = oldtbl;

        if (tbl_ == nullptr) {
            // print ending new line
            strm_ << "\n";
        }
    }

    void var_value_printer::print_spacing() const
    {
        for (int i = 0; i != nesting_; ++i) {
            strm_ << spacing;
        }
    }

}//namespace ast
}//namespace pcsh
