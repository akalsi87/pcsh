/**
 * \file interpreter.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_EXECUTION_INTERPRETER_HPP
#define PCSH_EXECUTION_INTERPRETER_HPP

#include "ast/symbol_table.hpp"
#include "ast/visitor.hpp"

#include <list>

namespace pcsh {
namespace execution {

    class interpreter final : public ast::node_visitor
    {
    public:
        inline interpreter() : curr_(nullptr), curr_visitor_(nullptr), ar_(nullptr), nested_tables_(), last_assign_(nullptr)
        { }
    private:
        const ast::block* curr_;
        ast::node_visitor* curr_visitor_;
        arena* ar_;
        ast::sym_table_list nested_tables_;
        ast::node* last_assign_;

        void visit_impl(const ast::variable* v) override;
        void visit_impl(const ast::int_constant* v) override;
        void visit_impl(const ast::float_constant* v) override;
        void visit_impl(const ast::string_constant* v) override;
        void visit_impl(const ast::unary_plus* v) override;
        void visit_impl(const ast::unary_minus* v) override;
        void visit_impl(const ast::binary_div* v) override;
        void visit_impl(const ast::binary_minus* v) override;
        void visit_impl(const ast::binary_mult* v) override;
        void visit_impl(const ast::binary_plus* v) override;
        void visit_impl(const ast::assign* v) override;
        void visit_impl(const ast::comp_equals* v) override;
        void visit_impl(const ast::block* v) override;
        void visit_impl(const ast::if_stmt* v) override;
    };

}//namespace execution
}//namespace pcsh

#endif/*PCSH_EXECUTION_INTERPRETER_HPP*/
