/**
 * \file tree_cloner.hpp
 * \date Feb 01, 2016
 */

#ifndef PCSH_TREE_CLONER_HPP
#define PCSH_TREE_CLONER_HPP

#include "ast/visitor.hpp"

namespace pcsh {
namespace ast {

    class tree_cloner final : public node_visitor
    {
      public:
        tree_cloner() : curr_(nullptr), tree_(tree::create()), root_(nullptr), out_stmts_(), cloned_(nullptr)
        { }

        tree::ptr cloned_tree();
      private:
        const block* curr_;

        tree::ptr tree_;
        block* root_;
        std::vector<node*> out_stmts_;

        node* cloned_;

        void visit_impl(const variable* v) override;
        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_plus* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const unary_not* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const comp_equals* v) override;
        void visit_impl(const comp_le* v) override;
        void visit_impl(const comp_ge* v) override;
        void visit_impl(const comp_lt* v) override;
        void visit_impl(const comp_gt* v) override;
        void visit_impl(const logical_and* v) override;
        void visit_impl(const logical_or* v) override;
        void visit_impl(const block* v) override;
        void visit_impl(const if_stmt* v) override;
    };

}//namespace ast
}//namespace pcsh

#endif/*PCSH_TREE_CLONER_HPP*/
