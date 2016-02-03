/**
 * \file ir_evaluator.cpp
 * \date Jan 31, 2016
 */

#include "ir_evaluator.hpp"
#include "ir_nodes.hpp"
#include "symbol_table.hpp"

namespace pcsh {
namespace ir {

    class evaluator::variable_accessor
    {
      public:
        variable_accessor(const sym_table_list& l) : list_(l)
        { }

        symbol_table::entry lookup(const ir::variable* v) const
        {
            auto it = list_.rbegin();
            auto end = list_.rend();
            for (; it != end; ++it) {
                const auto& tblptr = *it;
                auto res = symbol_table::lookup(*tblptr, v);
                if (res.ptr) {
                    return res;
                }
            }
            return { nullptr, result_type::UNDETERMINED };
        }

        void set(const ir::variable* v, ir::node* value, result_type ty = result_type::UNDETERMINED) const
        {
            auto it = list_.rbegin();
            auto end = list_.rend();
            for (; it != end; ++it) {
                const auto& tblptr = *it;
                auto res = symbol_table::lookup(*tblptr, v);
                if (res.ptr) {
                    symbol_table::set(*tblptr, v, value, ty);
                    return;
                }
            }
        }
      private:
        const sym_table_list& list_;
    };

    template <class T>
    class evaluator::typed_evaluate : public node_visitor
    {
      public:
        typed_evaluate(const sym_table_list& p) : accessor_(p), value_()
        { }

        T value() const
        {
            return value_;
        }
      private:
        evaluator::variable_accessor accessor_;
        T value_;

        void visit_impl(const variable* v) override
        {
            accessor_.lookup(v).ptr->accept(this);
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
                curr_visitor_ = new typed_evaluate<int>(nested_tables_);
                break;
            case result_type::FLOATING:
                curr_visitor_ = new typed_evaluate<double>(nested_tables_);
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
            nested_tables_.push_back(&(v->table()));

            visit_block(v);

            nested_tables_.pop_back();
        }

        curr_ = oldblk;
        curr_visitor_ = oldvis;
    }

}//namespace ir
}//namespace pcsh

