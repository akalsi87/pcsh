/**
 * \file ir_variable_printer.cpp
 * \date Feb 01, 2016
 */

#include "ir_nodes.hpp"
#include "ir_printer.hpp"
#include "ir_variable_printer.hpp"

namespace pcsh {
namespace ir {

    static const char* const spacing = "  ";

    void var_value_printer::visit_impl(const block* v)
    {
        if (!prn_) {
            prn_ = new printer(strm_, true);
        }

        strm_ << "\n";
        print_spacing();
        strm_ << "(block) at " << v;

        auto oldtbl = tbl_;
        ++nesting_;

        tbl_ = &v->table();
        {// visit this block
            {// print this block
                auto nv = symbol_table::all_entries(*tbl_);
                variable tmp(nullptr);
                for (const auto& el : nv) {
                    tmp.set_name(el.name);
                    auto val = symbol_table::lookup((*tbl_), &tmp).ptr;
                    strm_ << "\n";
                    print_spacing();
                    tmp.accept(prn_);
                    strm_ << " -> ";
                    val->accept(prn_);
                }
            }
            visit_block(v);
        }

        --nesting_;
        tbl_ = oldtbl;
    }

    void var_value_printer::print_spacing() const
    {
        for (int i = 0; i != nesting_; ++i) {
            strm_ << spacing;
        }
    }

}//namespace ir
}//namespace pcsh
