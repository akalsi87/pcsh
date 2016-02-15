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
        typed_evaluate(const sym_table_list& p) : accessor_(p), value_()
        { }

        T value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
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
    };

    template <>
    class evaluator::typed_evaluate<cstring> : public node_visitor
    {
      public:
        typed_evaluate(const sym_table_list& p) : accessor_(p), value_(nullptr)
        { }

        cstring value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
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

        void visit_impl(const string_constant* v) override
        {
            value_ = v->value();
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
        arena& ar = *ar_;

        variable_accessor acc(nested_tables_);

        const auto& ent = acc.lookup(v->var());
        result_type outty = ent.type;

        switch (outty) {
            case result_type::INTEGER:
                curr_visitor_ = new typed_evaluate<int>(nested_tables_);
                break;
            case result_type::FLOATING:
                curr_visitor_ = new typed_evaluate<double>(nested_tables_);
                break;
            case result_type::STRING:
                // do nothing, we retain the same value as of now
                curr_visitor_ = new typed_evaluate<cstring>(nested_tables_);
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        v->right()->accept(this);

        union
        {
            double  dblval;
            int     intval;
            cstring strval;
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
                uval.strval = static_cast<typed_evaluate<cstring>*>(curr_visitor_)->value();
                newvalue = ar.create<string_constant>(ar.create_string(uval.strval));
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        acc.set(v->var(), newvalue, outty, true);

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

    void evaluator::visit_impl(const if_stmt* v)
    {
        auto c = v->condition();
        auto cty = v->condition_type();

        bool execBody = false;

        switch (cty) {
            case pcsh::result_type::INTEGER: {
                typed_evaluate<int> eval(nested_tables_);
                c->accept(&eval);
                execBody = (eval.value() != 0);
                break;
            }
            case pcsh::result_type::FLOATING: {
                typed_evaluate<double> eval(nested_tables_);
                c->accept(&eval);
                execBody = (eval.value() != 0.0);
                break;
            }
            case pcsh::result_type::STRING: {
                typed_evaluate<cstring> eval(nested_tables_);
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

