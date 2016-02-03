/**
 * \file ir_evaluator.cpp
 * \date Jan 31, 2016
 */

#include "ir_evaluator.hpp"
#include "ir_nodes.hpp"
#include "symbol_table.hpp"

namespace pcsh {
namespace ir {

    template <class T>
    class typed_evaluate : public node_visitor
    {
      public:
        typed_evaluate(const symbol_table::ptr& p) : ptab_(p), value_()
        { }

        T value() const
        {
            return value_;
        }
      private:
        const symbol_table::ptr& ptab_;
        T value_;

        void visit_impl(const variable* v) override
        {
            symbol_table::lookup(ptab_, v).ptr->accept(this);
        }

        void visit_impl(const int_constant* v) override
        {
            value_ = static_cast<T>(v->value());
        }

        void visit_impl(const float_constant* v) override
        {
            value_ = static_cast<T>(v->value());
        }

        void visit_impl(const unary_minus* v) override
        {
            v->operand()->accept(this);
            value_ = -value_;
        }

        void visit_impl(const binary_div* v) override
        {
            v->left()->accept(this);
            auto left = value_;
            v->right()->accept(this);
            auto right = value_;
            value_ = left / right;
        }

        void visit_impl(const binary_minus* v) override
        {
            v->left()->accept(this);
            auto left = value_;
            v->right()->accept(this);
            auto right = value_;
            value_ = left - right;
        }

        void visit_impl(const binary_mult* v) override
        {
            v->left()->accept(this);
            auto left = value_;
            v->right()->accept(this);
            auto right = value_;
            value_ = left * right;
        }

        void visit_impl(const binary_plus* v) override
        {
            v->left()->accept(this);
            auto left = value_;
            v->right()->accept(this);
            auto right = value_;
            value_ = left + right;
        }
    };

    void evaluator::visit_impl(const variable* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const int_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const float_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const string_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const unary_minus* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const binary_div* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const binary_minus* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const binary_mult* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const binary_plus* v)
    {
        curr_visitor_->visit(v);
    }

    void evaluator::visit_impl(const assign* v)
    {
        arena& ar = *ar_;
        const auto& ptab = curr_->table();

        const auto& ent = symbol_table::lookup(ptab, v->var());
        result_type outty = ent.type;

        switch (outty) {
            case result_type::STRING:
            case result_type::INTEGER:
                curr_visitor_ = new typed_evaluate<int>(ptab);
                break;
            case result_type::FLOATING:
                curr_visitor_ = new typed_evaluate<double>(ptab);
                break;
            default:
                PCSH_CRIT_ASSERT_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        v->right()->accept(this);

        union
        {
            double dblval;
            int    intval;
        } uval;

        node* newvalue = nullptr;

        switch (outty) {
            case result_type::INTEGER:
                uval.intval = static_cast<typed_evaluate<int>*>(curr_visitor_)->value();
                newvalue = ar.create<int_constant>(uval.intval);
                break;
            case result_type::FLOATING:
                uval.dblval = static_cast<typed_evaluate<double>*>(curr_visitor_)->value();
                newvalue = ar.create<float_constant>(uval.dblval);
                break;
            case result_type::STRING:
                newvalue = ent.ptr;
                break;
            default:
                PCSH_CRIT_ASSERT_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        if ((newvalue != ent.ptr) || (outty != ent.type)) {
            symbol_table::set(ptab, v->var(), newvalue, outty);
        }

        delete curr_visitor_;
        curr_visitor_ = nullptr;
    }

    void evaluator::visit_impl(const block* v)
    {
        auto oldblk = curr_;
        auto oldvis = curr_visitor_;

        if (!ar_) {
            ar_ = &(v->get_arena());
        }

        {// visit this block
            curr_ = v;
            curr_visitor_ = nullptr;

            visit_block(v);
        }

        curr_ = oldblk;
        curr_visitor_ = oldvis;
    }

}//namespace ir
}//namespace pcsh

