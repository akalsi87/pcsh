/**
 * \file tree_cloner.cpp
 * \date Feb 01, 2016
 */

#include "ir_nodes.hpp"
#include "symbol_table.hpp"
#include "tree_cloner.hpp"

namespace pcsh {
namespace ir {

    void tree_cloner::visit_impl(const variable* v)
    {
        auto& ar = root_->get_arena();
        cloned_ = ar.create<variable>(v->name());
    }

    void tree_cloner::visit_impl(const int_constant* v)
    {
        auto& ar = root_->get_arena();
        cloned_ = ar.create<int_constant>(v->value());
    }

    void tree_cloner::visit_impl(const float_constant* v)
    {
        auto& ar = root_->get_arena();
        cloned_ = ar.create<float_constant>(v->value());
    }

    void tree_cloner::visit_impl(const string_constant* v)
    {
        auto& ar = root_->get_arena();
        cloned_ = ar.create<string_constant>(v->value());
    }

    void tree_cloner::visit_impl(const unary_plus* v)
    {
        v->operand()->accept(this);
        auto newoperand = cloned_;

        auto& ar = root_->get_arena();
        auto newuplus = ar.create<unary_plus>();
        newuplus->set_operand(newoperand);
        cloned_ = newuplus;
    }

    void tree_cloner::visit_impl(const unary_minus* v)
    {
        v->operand()->accept(this);
        auto newoperand = cloned_;

        auto& ar = root_->get_arena();
        auto newuminus = ar.create<unary_minus>();
        newuminus->set_operand(newoperand);
        cloned_ = newuminus;
    }

    void tree_cloner::visit_impl(const binary_div* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<binary_div>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const binary_minus* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<binary_minus>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const binary_mult* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<binary_mult>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const binary_plus* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<binary_plus>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const assign* v)
    {
        v->var()->accept(this);
        auto newvar = reinterpret_cast<variable*>(cloned_);
        v->right()->accept(this);
        auto newval = cloned_;
        
        auto& ar = root_->get_arena();
        auto newasgn = ar.create<assign>();
        newasgn->set_left(newvar);
        newasgn->set_right(newval);
        out_stmts_.push_back(newasgn);

        auto ty = symbol_table::lookup(curr_->table(), v->var()).type;
        symbol_table::set(root_->table(), newvar, newval, ty);
        /* don't assign to cloned_, push back statement */
    }

    void tree_cloner::visit_impl(const block* v)
    {
        auto oldblk = curr_;
        auto oldstmts = out_stmts_;
        auto oldroot = root_;
        auto oldcloned = cloned_;

        {// visit this block
            arena& ar = tree_->get_arena();

            curr_ = v;
            out_stmts_.clear();
            root_ = ar.create<block>(ar);
            cloned_ = nullptr;

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
        out_stmts_ = std::move(oldstmts);
        if (oldroot) {
            oldroot->push_front_statement(root_);
            root_ = oldroot;
        }
        cloned_ = oldcloned;
    }

    tree::ptr tree_cloner::cloned_tree()
    {
        tree_->set_root(root_);
        root_ = nullptr;
        return std::move(tree_);
    }

}//namespace ir
}//namespace pcsh
