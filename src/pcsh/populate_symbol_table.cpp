/**
 * \file populate_symbol_table.cpp
 * \date Jan 31, 2016
 */

#include "populate_symbol_table.hpp"
#include "ir_nodes.hpp"

namespace pcsh {
namespace ir {

    void populate_symbol_table::visit_impl(const assign* v)
    {
        curr_->insert(v->var(), v->right());
    }

    void populate_symbol_table::visit_impl(const block* v)
    {
        auto old = curr_;
        curr_ = v;
        visit_block(v);
        curr_ = old;
    }

}//namespace ir
}//namespace pcsh
