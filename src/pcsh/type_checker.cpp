/**
 * \file type_checker.cpp
 * \date Jan 31, 2016
 */

#include "ir_nodes.hpp"
#include "type_checker.hpp"

namespace pcsh {
namespace ir {

    result_type propagate(result_type lfttype, result_type rgttype, result_type minvalid = result_type::BOOLEAN, result_type maxvalid = result_type::FLOATING)
    {
        auto lower = std::min(lfttype, rgttype);
        if (lower < minvalid) {
            return result_type::FAILED;
        }
        auto higher = std::max(lfttype, rgttype);
        if (higher > maxvalid) {
            return result_type::FAILED;
        }
        if (higher < result_type::STRING) {
            return higher;
        }
        if (rgttype != lfttype) {
            return result_type::FAILED;
        }
        return lfttype;
    }

    void type_checker::visit_impl(const int_constant* v)
    {
        curr_ = result_type::INTEGER;
    }

    void type_checker::visit_impl(const float_constant* v)
    {
        curr_ = result_type::FLOATING;
    }

    void type_checker::visit_impl(const string_constant* v)
    {
        curr_ = result_type::STRING;
    }

    void type_checker::visit_impl(const unary_plus* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        auto fintype = propagate(lfttype, /*fake value*/result_type::BOOLEAN);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of unary `+'.", v->left(), nullptr);
        }
        curr_ = fintype;
    }

    void type_checker::visit_impl(const unary_minus* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        auto fintype = propagate(lfttype, /*fake value*/result_type::BOOLEAN);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of unary `-'.", v->left(), nullptr);
        }
        curr_ = fintype;
    }

    void type_checker::visit_impl(const binary_div* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        v->right()->accept(this);
        auto rgttype = curr_;
        auto fintype = propagate(lfttype, rgttype, result_type::INTEGER);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of `/'.", v->left(), v->right());
        }
        curr_ = fintype;
    }

    void type_checker::visit_impl(const binary_minus* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        v->right()->accept(this);
        auto rgttype = curr_;
        auto fintype = propagate(lfttype, rgttype);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of `-'.", v->left(), v->right());
        }
        curr_ = (fintype == result_type::BOOLEAN) ? result_type::INTEGER : fintype;
    }

    void type_checker::visit_impl(const binary_mult* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        v->right()->accept(this);
        auto rgttype = curr_;
        auto fintype = propagate(lfttype, rgttype);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of `*'.", v->left(), v->right());
        }
        curr_ = (fintype == result_type::BOOLEAN) ? result_type::INTEGER : fintype;
    }

    void type_checker::visit_impl(const binary_plus* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        v->right()->accept(this);
        auto rgttype = curr_;
        auto fintype = propagate(lfttype, rgttype);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of `+'.", v->left(), v->right());
        }
        curr_ = (fintype == result_type::BOOLEAN) ? result_type::INTEGER : fintype;
    }

    void type_checker::visit_impl(const assign* v)
    {
        v->right()->accept(this);
        auto ty = curr_;
        PCSH_ASSERT_MSG(ty != result_type::FAILED, "Assigned FAILED result type to variable.");
        if (ty == result_type::UNDETERMINED) {
            throw type_checker_error("Value of variable `" + std::string(v->var()->name()) + "' is undetermined!", v->left(), v->right());
        }

        variable_accessor acc(nested_tables_);
        auto sym = acc.lookup(v->var());
        if (sym.type == result_type::UNDETERMINED) {
            acc.set(v->var(), sym.ptr, ty);
        } else if (sym.type != ty) {
            throw type_checker_error("Type of variable `" + std::string(v->var()->name()) + "' is changed!", v->right(), nullptr);
        }
    }

    void type_checker::visit_impl(const block* v)
    {
        auto oldtype = curr_;
        auto oldblk = curr_blk_;
        {
            curr_ = result_type::UNDETERMINED;
            curr_blk_ = v;
            nested_tables_.push_back(&(v->table()));
            visit_block(v);
            nested_tables_.pop_back();
        }
        curr_ = oldtype;
        curr_blk_ = oldblk;
    }

    void type_checker::visit_impl(const if_stmt* v)
    {
        v->condition()->accept(this);
        auto condty = curr_;
        PCSH_ASSERT_MSG(condty != result_type::FAILED, "If condition result type is undefined well.");
        v->set_condition_type(condty);
        v->body()->accept(this);
    }

}//namespace ir
}//namespace pcsh
