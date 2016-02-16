/**
 * \file tree_cloner.hpp
 * \date Feb 01, 2016
 */

#ifndef PCSH_TREE_CLONER_HPP
#define PCSH_TREE_CLONER_HPP

#include "ir/visitor.hpp"

namespace pcsh {
namespace ir {

    class tree_cloner : public node_visitor
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

#endif/*PCSH_TREE_CLONER_HPP*/
