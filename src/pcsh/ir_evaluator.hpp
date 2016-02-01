/**
 * \file ir_evaluator.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_EVALUATOR_HPP
#define PCSH_IR_EVALUATOR_HPP

#include "ir_visitor.hpp"

#include <vector>

namespace pcsh {
namespace ir {

    class evaluator : public node_visitor
    {
      public:
        evaluator::evaluator() : curr_(nullptr), curr_visitor_(nullptr), tree_(tree::create()), out_stmts_(), root_(nullptr)
        { }

        tree::ptr evaluated_tree();
      private:
        const block* curr_;
        node_visitor* curr_visitor_;

        tree::ptr tree_;
        std::vector<node*> out_stmts_;
        block* root_;

        void visit_impl(const variable* v) override;
        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;

        void print_types(const block* v);
        void print_spacing_newline();
        void print_spacing();
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_EVALUATOR_HPP*/
