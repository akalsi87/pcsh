/**
 * \file ir.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_IR_HPP
#define PCSH_IR_HPP

#include "pcsh/exportsym.h"
#include "pcsh/noncopyable.hpp"
#include "pcsh/ostream.hpp"
#include "pcsh/types.hpp"

namespace pcsh {
namespace ir {

    class node_visitor;

    class PCSH_API node : public noncopyable
    {
      public:
        inline void accept(node_visitor* v) const
        {
            accept_impl(v);
        }

        inline node* left() const
        {
            return left_impl();
        }

        inline node* right() const
        {
            return right_impl();
        }

        inline virtual ~node()
        { }
      private:
        virtual void accept_impl(node_visitor* v) const = 0;
        virtual node* left_impl() const = 0;
        virtual node* right_impl() const = 0;
    };

    class tree : public noncopyable
    {
      public:
        tree(node* root) : root_(root)
        { }

        inline node* root() const
        {
            return root_;
        }

        inline void accept(node_visitor* v) const
        {
            root_->accept(v);
        }
      private:
        node* root_;
    };

}// namespace ir
}// namespace pcsh

#endif/*PCSH_IR_HPP*/
