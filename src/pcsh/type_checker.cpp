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

    void type_checker::visit_impl(const unary_minus* v)
    {
        v->left()->accept(this);
        auto lfttype = curr_;
        auto fintype = propagate(lfttype, /*fake value*/result_type::BOOLEAN);
        if (fintype == result_type::FAILED) {
            throw type_checker_error("Invalid application of unary `-'.", lfttype, result_type::UNDETERMINED);
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
            throw type_checker_error("Invalid application of `/'.", lfttype, rgttype);
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
            throw type_checker_error("Invalid application of `-'.", lfttype, rgttype);
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
            throw type_checker_error("Invalid application of `*'.", lfttype, rgttype);
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
            throw type_checker_error("Invalid application of `+'.", lfttype, rgttype);
        }
        curr_ = (fintype == result_type::BOOLEAN) ? result_type::INTEGER : fintype;
    }

    void type_checker::visit_impl(const assign* v)
    {
        v->right()->accept(this);
        auto ty = curr_;
        PCSH_ASSERT_MSG(ty != result_type::FAILED, "Assigned FAILED result type to variable.");
        if (ty == result_type::UNDETERMINED) {
            throw type_checker_error("Value of variable `" + std::string(v->var()->name()) + "' is undetermined!", ty, ty);
        }
        symbol_table::set_var_type(curr_blk_->table(), v->var(), ty);
    }

    void type_checker::visit_impl(const block* v)
    {
        auto oldtype = curr_;
        auto oldblk = curr_blk_;
        {
            curr_ = result_type::UNDETERMINED;
            curr_blk_ = v;
            visit_block(v);
        }
        curr_ = oldtype;
        curr_blk_ = oldblk;
    }

}//namespace ir
}//namespace pcsh
