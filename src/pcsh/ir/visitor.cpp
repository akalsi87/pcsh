/**
 * \file visitor.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/ir.hpp"

#include "ir/nodes.hpp"
#include "ir/visitor.hpp"

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

    void node_visitor::visit_impl(const if_stmt* v)
    {
        v->condition()->accept(this);
        v->body()->accept(this);
    }

    void node_visitor::visit_block(const block* v)
    {
        auto h = v->head();
        while (h != nullptr) {
            h->entry->accept(this);
            h = h->next;
        }
    }

    void node_visitor::visit_block_postcbk(const block* v, stmt_visit_cbk cbk)
    {
        auto h = v->head();
        while (h != nullptr) {
            h->entry->accept(this);
            cbk(h->entry, h->next == nullptr);
            h = h->next;
        }
    }

    void node_visitor::visit_block_precbk(const block* v, stmt_visit_cbk cbk)
    {
        auto h = v->head();
        while (h != nullptr) {
            cbk(h->entry, h->next == nullptr);
            h->entry->accept(this);
            h = h->next;
        }
    }

    void node_visitor::visit_block_cbk(const block* v, stmt_visit_cbk precbk, stmt_visit_cbk postcbk)
    {
        auto h = v->head();
        while (h != nullptr) {
            precbk(h->entry, h->next == nullptr);
            h->entry->accept(this);
            postcbk(h->entry, h->next == nullptr);
            h = h->next;
        }
    }

}//namespace ir
}//namespace pcsh
