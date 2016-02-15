/**
 * \file symbol_table.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_SYMBOL_TABLE_HPP
#define PCSH_SYMBOL_TABLE_HPP

#include "pcsh/arena.hpp"
#include "pcsh/ir.hpp"
#include "pcsh/result_type.hpp"

#include "ir_nodes_fwd.hpp"

#include <list>
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

    void copy_into(const ptr& psrc, const ptr& pdst);

    struct entry
    {
        ir::node* ptr;
        result_type type;
        bool evaluated;
    };

    struct name_and_type
    {
        cstring name;
        result_type type;
        bool evaluated;
    };

    void set(const ptr& tbl, const ir::variable* v, ir::node* value, result_type ty = result_type::UNDETERMINED, bool eval = false);

    entry lookup(const ptr& tbl, const ir::variable* v);

    void set_var_type(const ptr& tbl, const ir::variable* v, result_type ty);

    std::vector<name_and_type> all_entries(const ptr& tbl);

}//namespace symbol_table

namespace ir {

    using sym_table_list = std::list<const symbol_table::ptr*>;

    class variable_accessor
    {
      public:
        inline variable_accessor(const sym_table_list& l) : list_(l)
        { }

        symbol_table::entry lookup(const ir::variable* v, bool findevaluated = false) const;

        void set(const ir::variable* v, ir::node* value, result_type ty = result_type::UNDETERMINED, bool eval = false) const;

        inline const sym_table_list& symtab_list() const
        {
            return list_;
        }
      private:
        const sym_table_list& list_;
    };

}//namespace ir

}//namespace pcsh

#endif/*PCSH_SYMBOL_TABLE_HPP*/
