/**
 * \file symbol_table.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"

#include "ir_nodes.hpp"
#include "symbol_table.hpp"

#include <unordered_map>

namespace pcsh {
namespace symbol_table {

    struct variable_name_hash
    {
        inline size_t operator()(const ir::variable* v) const
        {
            auto s = v->name();
            size_t hash = 0;
            for (; *s; ++s) {
                hash = (hash * 31) + (*s);
            }
            return hash;
        }
    };

    struct variable_name_comp
    {
        inline bool operator()(const ir::variable* v1, const ir::variable* v2) const
        {
            return ::strcmp(v1->name(), v2->name()) == 0;
        }
    };

    class table_impl
      : public std::unordered_map<const ir::variable*, entry, variable_name_hash, variable_name_comp>
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

    void set(const ptr& tbl, const ir::variable* v, ir::node* value, result_type ty, bool eval)
    {
        (*tbl)[v] = { value, ty, eval };
    }

    entry lookup(const ptr& tbl, const ir::variable* v)
    {
        auto it = tbl->find(v);
        if (it == tbl->end()) {
            return { nullptr, result_type::UNDETERMINED, false };
        } else {
            return it->second;
        }
    }

    void set_var_type(const ptr& tbl, const ir::variable* v, result_type ty)
    {
        auto it = tbl->find(v);
        PCSH_ASSERT_MSG(it != tbl->end(), "Attempt to set var type of untracked variable.");
        it->second.type = ty;
    }

    std::vector<name_and_type> all_entries(const ptr& tbl)
    {
        std::vector<name_and_type> v;
        v.reserve(tbl->size());
        for (const auto& el : (*tbl)) {
            name_and_type nt;
            nt.name = el.first->name();
            nt.type = el.second.type;
            v.push_back(nt);
        }
        return v;
    }

    void copy_into(const ptr& psrc, const ptr& pdst)
    {
        *pdst = *psrc;
    }

}//namespace symbol_table
}//namespace pcsh
