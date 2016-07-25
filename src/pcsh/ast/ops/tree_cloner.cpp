/**
 * \file tree_cloner.cpp
 * \date Feb 01, 2016
 */

#include "ast/nodes.hpp"
#include "ast/ops/tree_cloner.hpp"
#include "ast/symbol_table.hpp"

namespace pcsh {
namespace ast {

    void tree_cloner::visit_impl(const variable* v)
    {
        auto& ar = root_->get_arena();
        cloned_ = ar.create<variable>(ar.create_string(v->name()));
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
        cloned_ = ar.create<string_constant>(ar.create_string(v->value()));
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

    void tree_cloner::visit_impl(const unary_not* v)
    {
        v->operand()->accept(this);
        auto newoperand = cloned_;

        auto& ar = root_->get_arena();
        auto newuminus = ar.create<unary_not>();
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

        auto ty = symbol_table::lookup(curr_->table(), v->var()).type;
        symbol_table::set(root_->table(), newvar, newval, ty);

        cloned_ = newasgn;
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

            visit_block_postcbk(v,
                [this] (const node* a, bool) -> void {
                    out_stmts_.push_back(cloned_);
                });

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
        if (oldroot != nullptr) {
            root_ = oldroot;
        }
        cloned_ = oldcloned;
    }

    void tree_cloner::visit_impl(const if_stmt* v)
    {
        v->condition()->accept(this);
        auto cclone = cloned_;

        v->then_body()->accept(this);
        auto tbclone = cloned_;

        auto elsebody = v->else_body();
        if (elsebody) {
            elsebody->accept(this);
        } else {
            cloned_ = nullptr;
        }
        auto ebclone = cloned_;

        arena& ar = tree_->get_arena();
        auto ifs = ar.create<if_stmt>(cclone, tbclone);
        ifs->set_condition_type(v->condition_type());
        ifs->set_else_body(ebclone);
        cloned_ = ifs;
    }

    void tree_cloner::visit_impl(const comp_equals* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<comp_equals>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const comp_lt* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<comp_lt>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const comp_gt* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<comp_gt>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const comp_le* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<comp_le>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    void tree_cloner::visit_impl(const comp_ge* v)
    {
        v->left()->accept(this);
        auto newleft = cloned_;
        v->right()->accept(this);
        auto newright = cloned_;

        auto& ar = root_->get_arena();
        auto newbinop = ar.create<comp_ge>();
        newbinop->set_left(newleft);
        newbinop->set_right(newright);
        cloned_ = newbinop;
    }

    tree::ptr tree_cloner::cloned_tree()
    {
        tree_->set_root(root_);
        root_ = nullptr;
        return std::move(tree_);
    }

}//namespace ast
}//namespace pcsh
