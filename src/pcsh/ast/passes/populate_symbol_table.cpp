/**
 * \file populate_symbol_table.cpp
 * \date Jan 31, 2016
 */

#include "ast/nodes.hpp"
#include "ast/passes/populate_symbol_table.hpp"

namespace pcsh {
namespace ast {

    void populate_symbol_table::visit_impl(const assign* v)
    {
        variable_accessor acc(nested_list_);
        auto res = acc.lookup(v->var());
        if (!(res.ptr)) {
            symbol_table::set(*(nested_list_.back()), v->var(), v->right());
        }
        v->right()->accept(this);
    }

    void populate_symbol_table::visit_impl(const block* v)
    {
        auto old = curr_;
        nested_list_.push_back(&(v->table()));
        curr_ = v;
        visit_block(v);
        nested_list_.pop_back();
        curr_ = old;
    }

    void populate_symbol_table::visit_impl(const if_stmt* v)
    {
        v->then_body()->accept(this);
        auto elsebody = v->else_body();
        if (elsebody) {
            elsebody->accept(this);
        }
    }

}//namespace ast
}//namespace pcsh
