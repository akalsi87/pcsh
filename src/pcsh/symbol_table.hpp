/**
 * \file symbol_table.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_SYMBOL_TABLE_HPP
#define PCSH_SYMBOL_TABLE_HPP

#include "pcsh/arena.hpp"
#include "pcsh/ir.hpp"

#include "ir_nodes_fwd.hpp"
#include "result_type.hpp"

#include <memory>
#include <vector>

namespace pcsh {
namespace symbol_table {

    class table_impl;

namespace detail {

    void destroy_table_impl(table_impl* p);

    struct table_impl_destroyer
    {
        void operator()(table_impl* p) const
        {
            destroy_table_impl(p);
        }
    };

}//namespace detail

    typedef std::unique_ptr<table_impl, detail::table_impl_destroyer> ptr;

    //////////////////////////////////////////////////////////////////////////
    /// symbol table
    //////////////////////////////////////////////////////////////////////////

    ptr make_new();

    struct entry
    {
        ir::node* ptr;
        result_type type;
    };

    struct name_and_type
    {
        cstring name;
        result_type type;
    };

    void insert(const ptr& tbl, ir::variable* v, ir::node* value);

    entry lookup(const ptr& tbl, ir::variable* v);

    void set_var_type(const ptr& tbl, ir::variable* v, result_type ty);

    std::vector<name_and_type> all_entries(const ptr& tbl);

}//namespace symbol_table
}//namespace pcsh

#endif/*PCSH_SYMBOL_TABLE_HPP*/
