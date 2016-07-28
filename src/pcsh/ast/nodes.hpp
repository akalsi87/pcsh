/**
 * \file nodes.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_AST_NODES_HPP
#define PCSH_AST_NODES_HPP

#include "pcsh/arena.hpp"
#include "pcsh/assert.hpp"
#include "pcsh/ast.hpp"

#include "ast/visitor.hpp"
#include "ast/symbol_table.hpp"

#include <sstream>

// Disable MSVC warnings for dominant method inheritance in
// diamond classes
#if defined(_MSC_VER)
#pragma warning (disable:4250)
#endif // defined(_MSC_VER)

namespace pcsh {
namespace ast {

    //////////////////////////////////////////////////////////////////////////
    /// node types
    //////////////////////////////////////////////////////////////////////////

    class untyped_atom_base : public node
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
    class atom_base : public untyped_atom_base
    {
      private:
        void accept_impl(node_visitor* v) const
        {
            v->visit(static_cast<const T*>(this));
        }
    };

    // leaf nodes

    class type_constant final : public atom_base<type_constant>
    {
      public:
        type_constant(result_type r) : type_(r)
        { }

        inline result_type type() const
        {
            return type_;
        }
      private:
        result_type type_;
    };

    class variable final : public atom_base<variable>
    {
      public:
        variable(cstring nm) : name_(nm)
        { }

        inline cstring name() const
        {
            return name_;
        }

        inline void set_name(cstring n)
        {
            name_ = n;
        }
      private:
        cstring name_;
    };

    class int_constant final : public atom_base<int_constant>
    {
      public:
        int_constant(int_type val) : val_(val)
        { }

        inline int_type value() const
        {
            return val_;
        }
      private:
        int_type val_;
    };

    class float_constant final : public atom_base<float_constant>
    {
      public:
        float_constant(float_type val) : val_(val)
        { }

        inline float_type value() const
        {
            return val_;
        }
      private:
        float_type val_;
    };

    class string_constant final : public atom_base<string_constant>
    {
      public:
        string_constant(string_type val) : val_(val)
        { }

        inline string_type value() const
        {
            return val_;
        }
      private:
        string_type val_;
    };

    // operations

    class untyped_unary_op_base : public node
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
    class unary_op : public untyped_unary_op_base
    {
      private:
        void accept_impl(node_visitor* v) const
        {
            v->visit(static_cast<const T*>(this));
        }
    };

    class unary_plus final : public unary_op<unary_plus>
    { };

    class unary_minus final : public unary_op<unary_minus>
    { };

    class unary_not final : public unary_op<unary_not>
    { };

    // binary ops

    class untyped_binary_op_base : public node
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

      protected:
        node* left_;
        node* right_;
      private:
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
    class binary_op : public untyped_binary_op_base
    {
      private:
        void accept_impl(node_visitor* v) const
        {
            v->visit(static_cast<const T*>(this));
        }
    };

    class binary_plus final : public binary_op<binary_plus>
    { };

    class binary_minus final : public binary_op<binary_minus>
    { };

    class binary_mult final : public binary_op<binary_mult>
    { };

    class binary_div final : public binary_op<binary_div>
    { };

    class logical_and final : public binary_op<logical_and>
    { };

    class logical_or final : public binary_op<logical_or>
    { };

    class assign final : public binary_op<assign>
    {
      public:
        inline variable* var() const
        {
            PCSH_ASSERT_MSG(dynamic_cast<variable*>(left_) != nullptr, "Assignment node left must be a variable.");
            return reinterpret_cast<variable*>(left_);
        }
    };

    class var_decl final : public binary_op<var_decl>
    {
      public:
        inline type_constant* type() const
        {
            PCSH_ASSERT_MSG(dynamic_cast<type_constant*>(right_) != nullptr, "VarDecl node right must be a type_constant.");
            return reinterpret_cast<type_constant*>(right_);            
        }
    };

    class comp_equals final : public binary_op<comp_equals>
    {
      public:
        comp_equals()
          : comp_ty_(result_type::UNDETERMINED)
        { }

        inline result_type comp_type() const
        {
            return comp_ty_;
        }

        inline void set_comp_type(result_type ty) const
        {
            comp_ty_ = ty;
        }

      private:
        mutable result_type comp_ty_;
    };

    class comp_lt final : public binary_op<comp_lt>
    {
      public:
        comp_lt()
          : comp_ty_(result_type::UNDETERMINED)
        { }

        inline result_type comp_type() const
        {
            return comp_ty_;
        }

        inline void set_comp_type(result_type ty) const
        {
            comp_ty_ = ty;
        }

      private:
        mutable result_type comp_ty_;
    };

    class comp_gt final : public binary_op<comp_gt>
    {
      public:
        comp_gt()
          : comp_ty_(result_type::UNDETERMINED)
        { }

        inline result_type comp_type() const
        {
            return comp_ty_;
        }

        inline void set_comp_type(result_type ty) const
        {
            comp_ty_ = ty;
        }

      private:
        mutable result_type comp_ty_;
    };

    class comp_le final : public binary_op<comp_le>
    {
      public:
        comp_le()
          : comp_ty_(result_type::UNDETERMINED)
        { }

        inline result_type comp_type() const
        {
            return comp_ty_;
        }

        inline void set_comp_type(result_type ty) const
        {
            comp_ty_ = ty;
        }

      private:
        mutable result_type comp_ty_;
    };

    class comp_ge final : public binary_op<comp_ge>
    {
      public:
        comp_ge()
          : comp_ty_(result_type::UNDETERMINED)
        { }

        inline result_type comp_type() const
        {
            return comp_ty_;
        }

        inline void set_comp_type(result_type ty) const
        {
            comp_ty_ = ty;
        }

      private:
        mutable result_type comp_ty_;
    };

    class block final : public atom_base<block>
    {
      public:
        struct list_node
        {
            list_node* next;
            const node* entry;
        };

      public:
        block(arena& ar) : arena_(ar), head_(nullptr), symtab_(symbol_table::make_new())
        { }

        inline void insert(ast::variable* v, ast::node* value) const
        {
            symbol_table::set(symtab_, v, value);
        }

        inline const symbol_table::ptr& table() const
        {
            return symtab_;
        }

        void push_front_statement(node* n)
        {
            auto newstmt = arena_.create<list_node>();
            *newstmt = { head_, n };
            head_ = newstmt;
        }

        list_node* head() const
        {
            return head_;
        }

        arena& get_arena() const
        {
            return arena_;
        }

      private:
        arena& arena_;
        list_node* head_;
        symbol_table::ptr symtab_;
    };

    class if_stmt final : public atom_base<if_stmt>
    {
      public:
        if_stmt(node* c = nullptr, node* b = nullptr)
          : cond_(c), body_(b), else_(nullptr), cond_ty_(result_type::UNDETERMINED)
        { }

        inline node* condition() const
        {
            return cond_;
        }

        inline node* then_body() const
        {
            return body_;
        }

        inline node* else_body() const
        {
            return else_;
        }

        inline result_type condition_type() const
        {
            return cond_ty_;
        }

        inline void set_condition(node* c)
        {
            cond_ = c;
        }

        inline void set_then_body(node* b)
        {
            body_ = b;
        }

        inline void set_else_body(node* b)
        {
            else_ = b;
        }

        inline void set_condition_type(result_type ty) const
        {
            cond_ty_ = ty;
        }
      private:
        node* cond_;
        node* body_;
        node* else_;
        mutable result_type cond_ty_;
    };

}//namespace ast
}//namespace pcsh

#if defined(_MSC_VER)
#pragma warning (default:4250)
#endif // defined(_MSC_VER)

#endif/*PCSH_AST_NODES_HPP*/
