/**
 * \file ir_visitor.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/ir.hpp"

#include "ir_nodes.hpp"
#include "ir_visitor.hpp"

namespace pcsh {
namespace ir {

    void node_visitor::visit_impl_binary_op(const void* v)
    {
        auto n = reinterpret_cast<const untyped_binary_op_base*>(v);
        n->left()->accept(this);
        n->right()->accept(this);
    }

    void node_visitor::visit_impl_unary_op(const void* v)
    {
        auto n = reinterpret_cast<const untyped_unary_op_base*>(v);
        n->operand()->accept(this);
    }

    void node_visitor::visit_impl(const assign* v)
    {
        v->left()->accept(this);
        v->right()->accept(this);
    }

    void node_visitor::visit_impl(const block* v)
    {
        visit_block(v);
    }

    void node_visitor::visit_block(const block* v)
    {
        auto h = v->head();
        while (h != nullptr) {
            h->entry->accept(this);
            h = h->next;
        }
    }

}//namespace ir
}//namespace pcsh
