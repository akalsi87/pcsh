/**
 * \file populate_symbol_table.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_POPULATE_SYMBOL_TABLE_HPP
#define PCSH_POPULATE_SYMBOL_TABLE_HPP

#include "ir_visitor.hpp"
#include "symbol_table.hpp"

namespace pcsh {
namespace ir {

    class populate_symbol_table : public node_visitor
    {
      public:
        populate_symbol_table() : curr_(nullptr), nested_list_()
        { }
      private:
        const block* curr_;
        sym_table_list nested_list_;

        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;
        void visit_impl(const if_stmt* v) override;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_POPULATE_SYMBOL_TABLE_HPP*/
