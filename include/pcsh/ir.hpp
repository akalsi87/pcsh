/**
 * \file ir.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_IR_HPP
#define PCSH_IR_HPP

#include "pcsh/exportsym.h"
#include "pcsh/arena.hpp"
#include "pcsh/noncopyable.hpp"
#include "pcsh/types.hpp"

#include <string>

namespace pcsh {
namespace ir {

    class node : public noncopyable
    {
      public:
        inline std::string to_string() const
        {
            std::string str;
            to_string_impl(str);
            return str;
        }

        inline node* left() const
        {
            return left_impl();
        }

        inline node* right() const
        {
            return right_impl();
        }
      private:
        virtual void to_string_impl(std::string& str) const = 0;
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
      private:
        node* root_;
    };

}// namespace ir
}// namespace pcsh

#endif/*PCSH_IR_HPP*/
