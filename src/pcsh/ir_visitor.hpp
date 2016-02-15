/**
 * \file ir_visitor.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_VISITOR_HPP
#define PCSH_IR_VISITOR_HPP

#include "pcsh/ir.hpp"

#include "ir_nodes_fwd.hpp"

#include <functional>

namespace pcsh {
namespace ir {

    using stmt_visit_cbk = std::function<void(const node* a, bool islast)>;

    class node_visitor
    {
      public:
        inline virtual ~node_visitor()
        { }

        void visit(const variable* v)
        {
            visit_impl(v);
        }

        void visit(const int_constant* v)
        {
            visit_impl(v);
        }

        void visit(const float_constant* v)
        {
            visit_impl(v);
        }

        void visit(const string_constant* v)
        {
            visit_impl(v);
        }

        void visit(const unary_plus* v)
        {
            visit_impl(v);
        }

        void visit(const unary_minus* v)
        {
            visit_impl(v);
        }

        void visit(const binary_div* v)
        {
            visit_impl(v);
        }

        void visit(const binary_minus* v)
        {
            visit_impl(v);
        }

        void visit(const binary_mult* v)
        {
            visit_impl(v);
        }

        void visit(const binary_plus* v)
        {
            visit_impl(v);
        }

        void visit(const assign* v)
        {
            visit_impl(v);
        }

        void visit(const block* v)
        {
            visit_impl(v);
        }

        void visit(const if_stmt* v)
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

        virtual void visit_impl(const assign* v);
        virtual void visit_impl(const block* v);
        virtual void visit_impl(const if_stmt* v);
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_VISITOR_HPP*/
