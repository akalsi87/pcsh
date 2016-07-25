/**
 * \file visitor.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_AST_VISITOR_HPP
#define PCSH_AST_VISITOR_HPP

#include "pcsh/ast.hpp"

#include "ast/nodes_fwd.hpp"

#include <functional>

namespace pcsh {
namespace ast {

    using stmt_visit_cbk = std::function<void(const node* a, bool islast)>;

    class node_visitor
    {
      public:
        inline virtual ~node_visitor()
        { }

        inline void visit(const variable* v)
        {
            visit_impl(v);
        }

        inline void visit(const int_constant* v)
        {
            visit_impl(v);
        }

        inline void visit(const float_constant* v)
        {
            visit_impl(v);
        }

        inline void visit(const string_constant* v)
        {
            visit_impl(v);
        }

        inline void visit(const unary_plus* v)
        {
            visit_impl(v);
        }

        inline void visit(const unary_minus* v)
        {
            visit_impl(v);
        }

        inline void visit(const unary_not* v)
        {
            visit_impl(v);
        }

        inline void visit(const binary_div* v)
        {
            visit_impl(v);
        }

        inline void visit(const binary_minus* v)
        {
            visit_impl(v);
        }

        inline void visit(const binary_mult* v)
        {
            visit_impl(v);
        }

        inline void visit(const binary_plus* v)
        {
            visit_impl(v);
        }

        inline void visit(const assign* v)
        {
            visit_impl(v);
        }

        inline void visit(const block* v)
        {
            visit_impl(v);
        }

        inline void visit(const if_stmt* v)
        {
            visit_impl(v);
        }

        inline void visit(const comp_equals* v)
        {
            visit_impl(v);
        }

        inline void visit(const comp_lt* v)
        {
            visit_impl(v);
        }

        inline void visit(const comp_gt* v)
        {
            visit_impl(v);
        }

        inline void visit(const comp_le* v)
        {
            visit_impl(v);
        }
        
        inline void visit(const comp_ge* v)
        {
            visit_impl(v);
        }

        inline void visit(const logical_and* v)
        {
            visit_impl(v);
        }

        inline void visit(const logical_or* v)
        {
            visit_impl(v);
        }

      private:
        void visit_impl_binary_op(const void* v);
        void visit_impl_unary_op(const void* v);

      protected:
        void visit_block(const block* v);
        void visit_block_precbk(const block* v, stmt_visit_cbk cbk);
        void visit_block_postcbk(const block* v, stmt_visit_cbk cbk);
        void visit_block_cbk(const block* v, stmt_visit_cbk precbk, stmt_visit_cbk postcbk);

      private:
        virtual void visit_impl(const variable* v)
        { }

        virtual void visit_impl(const int_constant* v)
        { }

        virtual void visit_impl(const float_constant* v)
        { }

        virtual void visit_impl(const string_constant* v)
        { }

        virtual void visit_impl(const unary_plus* v)
        {
            visit_impl_unary_op(v);
        }

        virtual void visit_impl(const unary_minus* v)
        {
            visit_impl_unary_op(v);
        }

        virtual void visit_impl(const unary_not* v)
        {
            visit_impl_unary_op(v);
        }

        virtual void visit_impl(const binary_div* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const binary_minus* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const binary_mult* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const binary_plus* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const comp_equals* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const comp_le* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const comp_ge* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const comp_lt* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const comp_gt* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const logical_and* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const logical_or* v)
        {
            visit_impl_binary_op(v);
        }

        virtual void visit_impl(const assign* v);
        virtual void visit_impl(const block* v);
        virtual void visit_impl(const if_stmt* v);
    };

}//namespace ast
}//namespace pcsh

#endif/*PCSH_AST_VISITOR_HPP*/