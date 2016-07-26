/**
 * \file interpreter.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/parser.hpp"

#include "execution/interpreter.hpp"
#include "ast/nodes.hpp"
#include "ast/symbol_table.hpp"

namespace pcsh {
namespace execution {

    using namespace ast;

    template <bool isint>
    bool compare_eq(const comp_equals* v, const variable_accessor& acc, arena& ar);

    template <bool isint>
    bool compare_lt(const comp_lt* v, const variable_accessor& acc, arena& ar);

    template <bool isint>
    bool compare_gt(const comp_gt* v, const variable_accessor& acc, arena& ar);

    template <bool isint>
    bool compare_le(const comp_le* v, const variable_accessor& acc, arena& ar);

    template <bool isint>
    bool compare_ge(const comp_ge* v, const variable_accessor& acc, arena& ar);

    template <class T>
    class typed_interpreter;

    template <>
    class typed_interpreter<void>;

    template <>
    class typed_interpreter<ast::string_type>;

    template <class T>
    class typed_interpreter : public node_visitor
    {
      public:
        typed_interpreter(const sym_table_list& p, arena& ar) : accessor_(p), ar_(ar), value_()
        { }

        T value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
        arena& ar_;
        T value_;

        static T false_value() { return T(0); }
        static T true_value()  { return T(1); }

        void visit_impl(const variable* v) override
        {
            auto res = accessor_.lookup(v, true);
            if (res.evaluated) {
                res.ptr->accept(this);
                return;
            }
            auto msg = std::string("Variable `") + v->name() + "' used before it is assigned a value!";
            parser::throw_parser_exception(msg, "", "", "");
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

        void visit_impl(const unary_not* v) override
        {
            v->operand()->accept(this);
            if (result_type_of<T>::value != result_type::STRING) {
                value_ = (value_ != false_value()) ? false_value() : true_value();
            } else {
                parser::throw_parser_exception("Using `!' on an expression that cannot be cast to a boolean value!", "", "", "");
            }
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

        void visit_impl(const comp_equals* v) override
        {
            value_ = compare_eq<result_type_of<T>::value != result_type::STRING>(v, accessor_, ar_)
                ? true_value()
                : false_value();
        }

        void visit_impl(const comp_lt* v) override
        {
            value_ = compare_lt<result_type_of<T>::value != result_type::STRING>(v, accessor_, ar_)
                ? true_value()
                : false_value();
        }

        void visit_impl(const comp_gt* v) override
        {
            value_ = compare_gt<result_type_of<T>::value != result_type::STRING>(v, accessor_, ar_)
                ? true_value()
                : false_value();
        }

        void visit_impl(const comp_le* v) override
        {
            value_ = compare_le<result_type_of<T>::value != result_type::STRING>(v, accessor_, ar_)
                ? true_value()
                : false_value();
        }

        void visit_impl(const comp_ge* v) override
        {
            value_ = compare_ge<result_type_of<T>::value != result_type::STRING>(v, accessor_, ar_)
                ? true_value()
                : false_value();
        }

        void visit_impl(const logical_and* v) override
        {
            if (result_type_of<T>::value == result_type::STRING) {
                parser::throw_parser_exception("Using `&&' on an expression that cannot be cast to a boolean value!", "", "", "");
            }
            v->left()->accept(this);
            auto left = value_;

            if (left == false_value()) {
                // short-circuit false
                value_ = false_value();
                return;
            }

            v->right()->accept(this);
            auto right = value_;
            value_ = right != false_value();
        }

        void visit_impl(const logical_or* v) override
        {
            if (result_type_of<T>::value == result_type::STRING) {
                parser::throw_parser_exception("Using `||' on an expression that cannot be cast to a boolean value!", "", "", "");
            }
            v->left()->accept(this);
            auto left = value_;

            if (left != false_value()) {
                // short-circuit true
                value_ = true_value();
                return;
            }

            v->right()->accept(this);
            auto right = value_;
            value_ = right != false_value();
        }
    };

    template <>
    class typed_interpreter<ast::string_type> : public node_visitor
    {
      public:
        typed_interpreter(const sym_table_list& p, arena& ar) : accessor_(p), ar_(ar), value_(nullptr)
        { }

        ast::string_type value() const
        {
            return value_;
        }
      private:
        variable_accessor accessor_;
        arena& ar_;
        ast::string_type value_;

        void visit_impl(const variable* v) override
        {
            auto res = accessor_.lookup(v, true);
            if (res.evaluated) {
                res.ptr->accept(this);
                return;
            }
            auto msg = std::string("Variable `") + v->name() + "' used before it is assigned a value!";
            parser::throw_parser_exception(msg, "", "", "");
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

        void visit_impl(const comp_equals* v) override
        {
            parser::throw_parser_exception("Invalid use of `=='. Return type of expression must be integer.", "", "", "");
        }
    };

    template <>
    class typed_interpreter<void> : public node_visitor
    { };

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// operations
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    template <>
    inline bool compare_eq<false>(const comp_equals* v, const variable_accessor& acc, arena& ar)
    {
        parser::throw_parser_exception("Invalid use of `=='. Return type of expression must be integer.", "", "", "");
        return false;
    }

    template <>
    inline bool compare_eq<true>(const comp_equals* v, const variable_accessor& acc, arena& ar)
    {
        switch (v->comp_type()) {
            case result_type::STRING: {
                typed_interpreter<ast::string_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return ::strcmp(v1, v2) == 0;
            }
            case result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 == v2;
                break;
            }
            case result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 == v2;
            }
            default: {
                PCSH_ASSERT_MSG(false, "Invalid comparison type");
                return false;
            }
        }
    }

    template <>
    inline bool compare_lt<false>(const comp_lt* v, const variable_accessor& acc, arena& ar)
    {
        parser::throw_parser_exception("Invalid use of `<'. Return type of expression must be integer.", "", "", "");
        return false;
    }

    template <>
    inline bool compare_lt<true>(const comp_lt* v, const variable_accessor& acc, arena& ar)
    {
        switch (v->comp_type()) {
            case result_type::STRING: {
                typed_interpreter<ast::string_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return ::strcmp(v1, v2) < 0;
            }
            case result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 < v2;
                break;
            }
            case result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 < v2;
            }
            default: {
                PCSH_ASSERT_MSG(false, "Invalid comparison type");
                return false;
            }
        }
    }

    template <>
    inline bool compare_gt<false>(const comp_gt* v, const variable_accessor& acc, arena& ar)
    {
        parser::throw_parser_exception("Invalid use of `>'. Return type of expression must be integer.", "", "", "");
        return false;
    }

    template <>
    inline bool compare_gt<true>(const comp_gt* v, const variable_accessor& acc, arena& ar)
    {
        switch (v->comp_type()) {
            case result_type::STRING: {
                typed_interpreter<ast::string_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return ::strcmp(v1, v2) > 0;
            }
            case result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 > v2;
                break;
            }
            case result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 > v2;
            }
            default: {
                PCSH_ASSERT_MSG(false, "Invalid comparison type");
                return false;
            }
        }
    }

    template <>
    inline bool compare_le<false>(const comp_le* v, const variable_accessor& acc, arena& ar)
    {
        parser::throw_parser_exception("Invalid use of `<='. Return type of expression must be integer.", "", "", "");
        return false;
    }

    template <>
    inline bool compare_le<true>(const comp_le* v, const variable_accessor& acc, arena& ar)
    {
        switch (v->comp_type()) {
            case result_type::STRING: {
                typed_interpreter<ast::string_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return ::strcmp(v1, v2) <= 0;
            }
            case result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 <= v2;
                break;
            }
            case result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 <= v2;
            }
            default: {
                PCSH_ASSERT_MSG(false, "Invalid comparison type");
                return false;
            }
        }
    }

    template <>
    inline bool compare_ge<false>(const comp_ge* v, const variable_accessor& acc, arena& ar)
    {
        parser::throw_parser_exception("Invalid use of `>='. Return type of expression must be integer.", "", "", "");
        return false;
    }

    template <>
    inline bool compare_ge<true>(const comp_ge* v, const variable_accessor& acc, arena& ar)
    {
        switch (v->comp_type()) {
            case result_type::STRING: {
                typed_interpreter<ast::string_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return ::strcmp(v1, v2) >= 0;
            }
            case result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 >= v2;
                break;
            }
            case result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(acc.symtab_list(), ar);
                v->left()->accept(&eval);
                auto v1 = eval.value();
                v->right()->accept(&eval);
                auto v2 = eval.value();
                return v1 >= v2;
            }
            default: {
                PCSH_ASSERT_MSG(false, "Invalid comparison type");
                return false;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// evaluator
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void interpreter::visit_impl(const variable* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const int_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const float_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const string_constant* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const unary_plus* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const unary_minus* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const unary_not* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const binary_div* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const binary_minus* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const binary_mult* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const binary_plus* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const comp_lt* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const comp_gt* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const comp_le* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const comp_ge* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const logical_and* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const logical_or* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const assign* v)
    {
        auto oldvis = curr_visitor_;

        arena& ar = *ar_;

        // slightly wasteful, yes, but avoids dynamic allocation
        // and can be cleaned up later with a smarter union
        typed_interpreter<ast::int_type> intinterp(nested_tables_, ar);
        typed_interpreter<ast::float_type> dblinterp(nested_tables_, ar);
        typed_interpreter<ast::string_type> strinterp(nested_tables_, ar);

        variable_accessor acc(nested_tables_);

        const auto& ent = acc.lookup(v->var());
        result_type outty = ent.type;

        switch (outty) {
            case result_type::INTEGER:
                curr_visitor_ = &intinterp;
                break;
            case result_type::FLOATING:
                curr_visitor_ = &dblinterp;
                break;
            case result_type::STRING:
                curr_visitor_ = &strinterp;
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        auto oldlastasgn = last_assign_;
        v->right()->accept(this);

        node* newvalue = nullptr;

        if (oldlastasgn != last_assign_) {
            // cascading assignment operators
            acc.set(v->var(), last_assign_, outty, true);
            goto assign_done_cleanup;
        }

        switch (outty) {
            case result_type::INTEGER:
                newvalue = ar.create<int_constant>(intinterp.value());
                break;
            case result_type::FLOATING:
                newvalue = ar.create<float_constant>(dblinterp.value());
                break;
            case result_type::STRING:
                newvalue = ar.create<string_constant>(ar.create_string(strinterp.value()));
                break;
            default:
                PCSH_ENFORCE_MSG(false, "Incomplete implementation for evaluate!");
                break;
        }

        acc.set(v->var(), newvalue, outty, true);
        last_assign_ = newvalue;

assign_done_cleanup:
        curr_visitor_ = oldvis;
    }

    void interpreter::visit_impl(const comp_equals* v)
    {
        curr_visitor_->visit(v);
    }

    void interpreter::visit_impl(const block* v)
    {
        auto oldblk = curr_;
        auto oldvis = curr_visitor_;

        if (!ar_) {
            ar_ = &(v->get_arena());
        }

        {// visit this block
            curr_ = v;
            typed_interpreter<void> donothing;
            curr_visitor_ = &donothing;

            nested_tables_.push_back(&(v->table()));

            visit_block(v);

            nested_tables_.pop_back();
        }

        curr_ = oldblk;
        curr_visitor_ = oldvis;
    }

    void interpreter::visit_impl(const if_stmt* v)
    {
        auto c = v->condition();
        auto cty = v->condition_type();

        bool run_then_body = false;

        switch (cty) {
            case pcsh::result_type::INTEGER: {
                typed_interpreter<ast::int_type> eval(nested_tables_, *ar_);
                c->accept(&eval);
                run_then_body = (eval.value() != 0);
                break;
            }
            case pcsh::result_type::FLOATING: {
                typed_interpreter<ast::float_type> eval(nested_tables_, *ar_);
                c->accept(&eval);
                run_then_body = (eval.value() != 0.0);
                break;
            }
            case pcsh::result_type::STRING: {
                typed_interpreter<ast::string_type> eval(nested_tables_, *ar_);
                c->accept(&eval);
                ast::string_type str = eval.value();
                run_then_body = (str[0] != '\0');
                break;
            }
            default:
                PCSH_ASSERT_MSG(false, "Unknown condition type evaluation in if statement.");
                break;
        }

        node* elsebody = nullptr;
        if (run_then_body) {
            v->then_body()->accept(this);
        } else if ((elsebody = v->else_body())) {
            elsebody->accept(this);
        }
    }

}//namespace execution
}//namespace pcsh
