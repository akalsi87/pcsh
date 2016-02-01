/**
 * \file populate_symbol_table.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_POPULATE_SYMBOL_TABLE_HPP
#define PCSH_POPULATE_SYMBOL_TABLE_HPP

#include "ir_visitor.hpp"

namespace pcsh {
namespace ir {

    class populate_symbol_table : public node_visitor
    {
      public:
        populate_symbol_table() : curr_(nullptr)
        { }
      private:
        const block* curr_;

        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_POPULATE_SYMBOL_TABLE_HPP*/
