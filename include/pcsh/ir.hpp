/**
 * \file ir.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_IR_HPP
#define PCSH_IR_HPP

#include "pcsh/arena.hpp"
#include "pcsh/exportsym.h"
#include "pcsh/noncopyable.hpp"
#include "pcsh/ostream.hpp"
#include "pcsh/types.hpp"

#include <memory>

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

    class PCSH_API tree : public noncopyable
    {
      private:
        struct tree_destroyer
        {
            void operator()(tree* p) const
            {
                delete p->arena_;
            }
        };
      public:
        typedef std::unique_ptr<tree, tree_destroyer> ptr;

        static ptr create()
        {
            arena* parena = new arena;
            ptr p(parena->create<tree>());
            p->arena_ = parena;
            return std::move(p);
        }

        tree(node* root = nullptr) : root_(root), arena_(nullptr)
        { }

        inline node* root() const
        {
            return root_;
        }

        inline void accept(node_visitor* v) const
        {
            root_->accept(v);
        }

        inline arena& get_arena()
        {
            return *arena_;
        }

        inline void set_root(node* p)
        {
            root_ = p;
        }

        void print(ostream& os, bool vartypes = true);
      private:
        node* root_;
        arena* arena_;
    };

}// namespace ir
}// namespace pcsh

#endif/*PCSH_IR_HPP*/
