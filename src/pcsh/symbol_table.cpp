/**
 * \file symbol_table.cpp
 * \date Jan 31, 2016
 */

#include "symbol_table.hpp"

#include <unordered_map>

namespace pcsh {
namespace symbol_table {

    class table_impl : public std::unordered_map<ir::variable*, ir::node*>
    { };

namespace detail {
    void destroy_table_impl(table_impl* p)
    {
        delete p;
    }
}//namespace detail

    ptr make_new()
    {
        ptr tableptr(new table_impl());
        return std::move(tableptr);
    }

    void insert(const ptr& tbl, ir::variable* v, ir::node* value)
    {
        (*tbl)[v] = value;
    }

    ir::node* lookup(const ptr& tbl, ir::variable* v)
    {
        auto it = tbl->find(v);
        if (it == tbl->end()) {
            return nullptr;
        } else {
            return it->second;
        }
    }

}//namespace symbol_table
}//namespace pcsh
