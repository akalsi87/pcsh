/**
 * \file interpreter.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_EXECUTION_INTERPRETER_HPP
#define PCSH_EXECUTION_INTERPRETER_HPP

#include "ir/symbol_table.hpp"
#include "ir/visitor.hpp"

#include <list>

namespace pcsh {
namespace execution {

    class interpreter : public ir::node_visitor
    {
    public:
        inline interpreter() : curr_(nullptr), curr_visitor_(nullptr), ar_(nullptr), nested_tables_(), last_assign_(nullptr)
        { }
    private:
        const ir::block* curr_;
        ir::node_visitor* curr_visitor_;
        arena* ar_;
        ir::sym_table_list nested_tables_;
        ir::node* last_assign_;

        void visit_impl(const ir::variable* v) override;
        void visit_impl(const ir::int_constant* v) override;
        void visit_impl(const ir::float_constant* v) override;
        void visit_impl(const ir::string_constant* v) override;
        void visit_impl(const ir::unary_plus* v) override;
        void visit_impl(const ir::unary_minus* v) override;
        void visit_impl(const ir::binary_div* v) override;
        void visit_impl(const ir::binary_minus* v) override;
        void visit_impl(const ir::binary_mult* v) override;
        void visit_impl(const ir::binary_plus* v) override;
        void visit_impl(const ir::assign* v) override;
        void visit_impl(const ir::comp_equals* v) override;
        void visit_impl(const ir::block* v) override;
        void visit_impl(const ir::if_stmt* v) override;
    };

}//namespace execution
}//namespace pcsh

#endif/*PCSH_EXECUTION_INTERPRETER_HPP*/
