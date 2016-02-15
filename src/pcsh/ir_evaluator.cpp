/**
 * \file ir_evaluator.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/parser.hpp"

#include "ir_evaluator.hpp"
#include "ir_nodes.hpp"
#include "symbol_table.hpp"

namespace pcsh {
namespace ir {

    template <class T>
    class evaluator::typed_evaluate : public node_visitor
    {
      public:
        typed_evaluate(const sym_table_list& p, arena& ar) : accessor_(p), ar_(ar), value_()
        { }

        T value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
        arena& ar_;
        T value_;

        void visit_impl(const variable* v) override
        {
            auto res = accessor_.lookup(v, true);
            if (res.evaluated) {
                res.ptr->accept(this);
                return;
            }
            auto msg = std::string("Variable `") + v->name() + "' used before it is assigned a value!";
            throw parser::exception(msg, "", "", "");
        }

        void visit_impl(const int_constant* v) override
        {
            value_ = static_cast<T>(v->value());
        }

        void visit_impl(const float_constant* v) override
        {
            value_ = static_cast<T>(v->value());
        }

        void visit_impl(const unary_plus* v) override
        {
            v->operand()->accept(this);
            // value_ = value_;
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

        void visit_impl(const assign* v) override
        {
            auto res = accessor_.lookup(v->var());
            if (!res.evaluated) {
                v->right()->accept(this);
                accessor_.set(v->var(), ar_.create<float_constant>(value_),
                              result_type_of<T>::value, true);
            } else {
                res.ptr->accept(this);
            }
        }
    };

    template <>
    class evaluator::typed_evaluate<cstring> : public node_visitor
    {
      public:
        typed_evaluate(const sym_table_list& p, arena& ar) : accessor_(p), ar_(ar), value_(nullptr)
        { }

        cstring value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
        arena& ar_;
        cstring value_;

        void visit_impl(const variable* v) override
        {
            auto res = accessor_.lookup(v, true);
            if (res.evaluated) {
                res.ptr->accept(this);
                return;
            }
            auto msg = std::string("Variable `") + v->name() + "' used before it is assigned a value!";
            throw parser::exception(msg, "", "", "");
        }

        void visit_impl(const assign* v) override
        {
            auto res = accessor_.lookup(v->var());
            if (!res.evaluated) {
                v->right()->accept(this);
                accessor_.set(v->var(), ar_.create<string_constant>(ar_.create_string(value_)),
                              result_type::STRING, true);
            } else {
                res.ptr->accept(this);
            }
        }

        void visit_impl(const string_constant* v) override
        {
            value_ = v->value();
        }
    };

    template <>
    class evaluator::typed_evaluate<void> : public node_visitor
    { };

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

    void evaluator::visit_impl(const unary_plus* v)
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
        auto oldvis = curr_visitor_;

        arena& ar = *ar_;

        variable_accessor acc(nested_tables_);

        const auto& ent = acc.lookup(v->var());
        result_type outty = ent.type;

        switch (outty) {
            case result_type::INTEGER:
                curr_visitor_ = new typed_evaluate<int>(nested_tables_, ar);
                break;
            case result_type::FLOATING:
                curr_visitor_ = new typed_evaluate<double>(nested_tables_, ar);
                break;
            case result_type::STRING:
                // do nothing, we retain the same value as of now
                curr_visitor_ = new typed_evaluate<cstring>(nested_tables_, ar);
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        auto oldlastasgn = last_assign_;
        v->right()->accept(this);

        union
        {
            double  dblval;
            int     intval;
            cstring strval;
        } uval;

        node* newvalue = nullptr;

        if (oldlastasgn != last_assign_) {
            acc.set(v->var(), last_assign_, outty, true);
            goto assign_done_cleanup;
        }

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
                uval.strval = static_cast<typed_evaluate<cstring>*>(curr_visitor_)->value();
                newvalue = ar.create<string_constant>(ar.create_string(uval.strval));
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        acc.set(v->var(), newvalue, outty, true);
        last_assign_ = newvalue;
  assign_done_cleanup:
        delete curr_visitor_;
        curr_visitor_ = oldvis;
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
            typed_evaluate<void> donothing;
            curr_visitor_ = &donothing;

            nested_tables_.push_back(&(v->table()));

            visit_block(v);

            nested_tables_.pop_back();
        }

        curr_ = oldblk;
        curr_visitor_ = oldvis;
    }

    void evaluator::visit_impl(const if_stmt* v)
    {
        auto c = v->condition();
        auto cty = v->condition_type();

        bool execBody = false;

        switch (cty) {
            case pcsh::result_type::INTEGER: {
                typed_evaluate<int> eval(nested_tables_, *ar_);
                c->accept(&eval);
                execBody = (eval.value() != 0);
                break;
            }
            case pcsh::result_type::FLOATING: {
                typed_evaluate<double> eval(nested_tables_, *ar_);
                c->accept(&eval);
                execBody = (eval.value() != 0.0);
                break;
            }
            case pcsh::result_type::STRING: {
                typed_evaluate<cstring> eval(nested_tables_, *ar_);
                c->accept(&eval);
                cstring str = eval.value();
                execBody = (str[0] != '\0');
                break;
            }
            default:
                PCSH_ASSERT_MSG(false, "Unknown condition type evaluation in if statement.");
                break;
        }

        if (execBody) {
            v->body()->accept(this);
        }
    }

}//namespace ir
}//namespace pcsh

