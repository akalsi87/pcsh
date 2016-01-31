/**
 * \file ir_nodes.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_NODES_HPP
#define PCSH_IR_NODES_HPP

#include "pcsh/arena.hpp"
#include "pcsh/ir.hpp"

#include "ir_visitor.hpp"

#include <sstream>

// Disable MSVC warnings for dominant method inheritance in
// diamond classes
#if defined(_MSC_VER)
#pragma warning (disable:4250)
#endif // defined(_MSC_VER)

namespace pcsh {
namespace ir {

    template <class T>
    class typed_node : public virtual node
    {
      private:
        void accept_impl(node_visitor* v) const
        {
            v->visit(static_cast<const T*>(this));
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// node types
    //////////////////////////////////////////////////////////////////////////

    class untyped_atom_base : public virtual node
    {
      private:
        node* left_impl() const override
        {
            return nullptr;
        }

        node* right_impl() const override
        {
            return nullptr;
        }
    };

    template <class T>
    class atom_base : public typed_node<T>, public untyped_atom_base
    {
    };

    // leaf nodes

    class variable : public atom_base<variable>
    {
      public:
        variable(name nm) : name_(nm)
        { }

        inline name varname() const
        {
            return name_;
        }
      private:
        name name_;
    };

    class int_constant : public atom_base<int_constant>
    {
      public:
        int_constant(int val) : val_(val)
        { }

        inline int value() const
        {
            return val_;
        }
      private:
        int val_;
    };

    class float_constant : public atom_base<float_constant>
    {
      public:
        float_constant(double val) : val_(val)
        { }

        inline double value() const
        {
            return val_;
        }
      private:
        double val_;
    };

    class string_constant : public atom_base<string_constant>
    {
      public:
        string_constant(name val) : val_(val)
        { }

        inline name value() const
        {
            return val_;
        }
      private:
        name val_;
    };

    // operations

    class untyped_unary_op_base : public virtual node
    {
      public:
        untyped_unary_op_base() : operand_(nullptr)
        { }

        const node* operand() const
        {
            return operand_;
        }

        void set_operand(node* v)
        {
            operand_ = v;
        }
      private:
        node* operand_;

        node* left_impl() const override
        {
            return operand_;
        }

        node* right_impl() const override
        {
            return nullptr;
        }
    };

    template <class T>
    class unary_op : public typed_node<T>, public untyped_unary_op_base
    {
    };

    class unary_minus : public unary_op<unary_minus>
    {
    };

    // binary ops

    class untyped_binary_op_base : public virtual node
    {
      public:
        untyped_binary_op_base() : left_(nullptr), right_(nullptr)
        { }

        void set_left(node* n)
        {
            left_ = n;
        }

        void set_right(node* n)
        {
            right_ = n;
        }

      private:
        node* left_;
        node* right_;

        node* left_impl() const override
        {
            return left_;
        }

        node* right_impl() const override
        {
            return right_;
        }
    };

    template <class T>
    class binary_op : public typed_node<T>, public untyped_binary_op_base
    {
    };

    class binary_plus : public binary_op<binary_plus>
    {
    };

    class binary_minus : public binary_op<binary_minus>
    {
    };

    class binary_mult : public binary_op<binary_mult>
    {
    };

    class binary_div : public binary_op<binary_div>
    {
    };

    class assign : public binary_op<assign>
    {
    };

    class block : public atom_base<block>
    {
      public:
        struct list_node
        {
            list_node* next;
            const node* entry;
        };

      public:
        block() : head_(nullptr)
        { }

        void push_front_statement(node* n, arena& ar)
        {
            auto newstmt = ar.create<list_node>();
            *newstmt = { head_, n };
            head_ = newstmt;
        }

        list_node* head() const
        {
            return head_;
        }

      private:
        list_node* head_;
    };

}//namespace ir
}//namespace pcsh

#if defined(_MSC_VER)
#pragma warning (default:4250)
#endif // defined(_MSC_VER)

#endif/*PCSH_IR_NODES_HPP*/
