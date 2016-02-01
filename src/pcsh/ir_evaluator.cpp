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
        arena& ar = curr_->get_arena();
        const auto& ptab = curr_->table();
        result_type outty = symbol_table::lookup(ptab, v->var()).type;
        switch (outty) {
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

        auto varname = ar.create_string(v->var()->name());
        auto var = ar.create<variable>(varname);
        auto asgn = ar.create<assign>();
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
            default:
                PCSH_CRIT_ASSERT_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        asgn->set_left(var);
        asgn->set_right(newvalue);

        delete curr_visitor_;
        curr_visitor_ = nullptr;

        out_stmts_.push_back(asgn);
    }

    void evaluator::visit_impl(const block* v)
    {
        auto oldblk = curr_;
        auto oldvis = curr_visitor_;
        auto oldstmts = out_stmts_;
        auto oldroot = root_;

        {// visit this block
            arena& ar = tree_->get_arena();

            curr_ = v;
            curr_visitor_ = nullptr;
            out_stmts_.clear();
            root_ = ar.create<block>(ar);
            symbol_table::copy_into(v->table(), root_->table());

            visit_block(v);

            {// make a block out of all statements
                auto beg = out_stmts_.rbegin();
                const auto end = out_stmts_.rend();
                for (; beg != end; ++beg) {
                    root_->push_front_statement(*beg);
                }
            }
        }

        curr_ = oldblk;
        curr_visitor_ = oldvis;
        out_stmts_ = std::move(oldstmts);
        if (oldroot) {
            oldroot->push_front_statement(root_);
            root_ = oldroot;
        }
    }

    tree::ptr evaluator::evaluated_tree()
    {
        tree_->set_root(root_);
        return std::move(tree_);
    }

}//namespace ir
}//namespace pcsh

