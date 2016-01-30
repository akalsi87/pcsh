/**
 * \file ir.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_IR_HPP
#define PCSH_IR_HPP

#include "pcsh/exportsym.h"
#include "pcsh/types.hpp"

namespace pcsh {
namespace ir {

    class node
    {
      public:
        inline name to_string() const
        {
            return to_string_impl();
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
        virtual name to_string_impl() const = 0;
        virtual node* left_impl() const = 0;
        virtual node* right_impl() const = 0;
    };

    class tree
    {
      public:
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
