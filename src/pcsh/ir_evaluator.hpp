/**
 * \file ir_evaluator.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_EVALUATOR_HPP
#define PCSH_IR_EVALUATOR_HPP

#include "ir_visitor.hpp"
#include "symbol_table.hpp"

#include <list>

namespace pcsh {
namespace ir {

    class evaluator : public node_visitor
    {
      public:
        inline evaluator() : curr_(nullptr), curr_visitor_(nullptr), ar_(nullptr), nested_tables_(), last_assign_(nullptr)
        { }
      private:
        const block* curr_;
        node_visitor* curr_visitor_;
        arena* ar_;
        sym_table_list nested_tables_;
        node* last_assign_;

        template <class T>
        class typed_evaluate;

        void visit_impl(const variable* v) override;
        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_plus* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const comp_equals* v) override;
        void visit_impl(const block* v) override;
        void visit_impl(const if_stmt* v) override;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_EVALUATOR_HPP*/
