/**
 * \file ir_nodes.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_IR_NODES_HPP
#define PCSH_IR_NODES_HPP

#include "pcsh/ir.hpp"

#include <sstream>

namespace pcsh {
namespace ir {

    //////////////////////////////////////////////////////////////////////////
    /// node types
    //////////////////////////////////////////////////////////////////////////

    class atom_base : public node
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

    // leaf nodes

    class variable : public atom_base
    {
      public:
        variable(name nm) : name_(nm)
        { }
      private:
        name name_;

        void to_string_impl(std::string& str) const override
        {
            str = name_;
        }
    };

    class int_constant : public atom_base
    {
      public:
        int_constant(int val) : val_(val)
        { }
      private:
        int val_;

        void to_string_impl(std::string& str) const override
        {
            str = std::to_string(val_);
        }
    };

    class float_constant : public atom_base
    {
      public:
        float_constant(double val) : val_(val)
        { }
      private:
        double val_;

        void to_string_impl(std::string& str) const override
        {
            str = std::to_string(val_);
        }
    };

    class string_constant : public atom_base
    {
      public:
        string_constant(name val) : val_(val)
        { }
      private:
        name val_;

        void to_string_impl(std::string& str) const override
        {
            str = val_;
        }
    };

    // operations

    class unary_op : public node
    {
      public:
        unary_op() : operand_(nullptr)
        { }

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

    class unary_minus : public unary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "- (unary)";
        }
    };

    class unary_plus : public unary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "+ (unary)";
        }
    };

    class binary_op : public node
    {
      public:
        binary_op() : left_(nullptr), right_(nullptr)
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

    class binary_plus : public binary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "+";
        }
    };

    class binary_minus : public binary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "-";
        }
    };

    class binary_mult : public binary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "*";
        }
    };

    class binary_div : public binary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "/";
        }
    };

    class assign : public binary_op
    {
      private:
        void to_string_impl(std::string& str) const override
        {
            str = "=";
        }
    };

    class block : public atom_base
    {
      public:
        struct list_node
        {
            list_node* next;
            node* entry;
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

        void to_string_impl(std::string& str) const override
        {
            std::ostringstream strm;
            strm << "block at " << this;
            str = strm.str();
        }
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_NODES_HPP*/
