/**
 * \file parser_engine.cpp
 * \date Feb 15, 2016
 */

#include "ast/nodes.hpp"
#include "parser/parser_engine.hpp"

#include <vector>

#define ENSURE(x, msg)                    \
    do {                                  \
        !!(x) ? 0 : throw_error((msg));   \
    } while (0)

namespace pcsh {
namespace parser {

    int parser::parser_engine::throw_error(cstring msg)
    {
        pos_t ws = 0;
        parser_.peek(ws, &ws);
        parser_.advance(ws);
        const auto& linestr = "line " + std::to_string(parser_.line()) + ", char " + std::to_string(parser_.curr_pos() - parser_.line_start());
        std::string message(msg);
        message += " near: \"" + parser_.copy_line(0) + "\"";
        throw_parser_exception(message, parser_.filename_, func_, linestr);
        return 0;
    }

    PCSH_INLINE token parser::parser_engine::peek()
    {
        if (!parsed_) {
            do_peek();
        }
        return curr_;
    }

    void parser::parser_engine::do_peek()
    {
        PCSH_ASSERT(ws_ == 0);
        auto t = parser_.peek(0, &ws_);
        if (PCSH_UNLIKELY(t.is_a(token_type::FAIL))) {
            throw_error(t.str().ptr);
        }
        PCSH_ASSERT(!parsed_);
        parsed_ = true;
        curr_ = t;
    }

    PCSH_INLINE void parser::parser_engine::advance()
    {
        PCSH_ASSERT(parsed_);
        token t = curr_;
        if (!t.is_a(token_type::QUOTE)) {
            parser_.advance(ws_ + t.length());
        } else {
            parser_.advance(ws_ + t.length() + 2, false);
        }
        parsed_ = false;
        ws_ = 0;
#if !defined(NDEBUG)
        t = token::get(token_type::EOS, "\xFF", 1);
#endif // !defined(NDEBUG)
    }

    bool parser::parser_engine::is_unary_op(const token& t)
    {
        switch (t.type()) {
            case token_type::PLUS:
            case token_type::MINUS:
            case token_type::NOT:
                return true;
            default:
                return false;
        }
    }

    bool parser::parser_engine::is_binary_op(const token& t)
    {
        switch (t.type()) {
            case token_type::PLUS:
            case token_type::MINUS:
            case token_type::ASTERISK:
            case token_type::FSLASH:
            case token_type::ASSIGN:
            case token_type::ISEQUAL:
            case token_type::ISLT:
            case token_type::ISLE:
            case token_type::ISGT:
            case token_type::ISGE:
            case token_type::AND:
            case token_type::OR:
            case token_type::COLON:
                return true;
            default:
                return false;
        }
    }

    PCSH_INLINE ast::node* parser::parser_engine::call_mem_fn(parser_engine* p, nexttermfcn pfn, source_map& m)
    {
        return ((*p).*(pfn))(m);
    }

    ast::untyped_binary_op_base* parser::parser_engine::create_binary_op(const token& currtok, ast::node* a, source_map& m, nexttermfcn rghtgen)
    {
        ast::untyped_binary_op_base* op = nullptr;
        bool checkfortype = false;
        switch (currtok.type()) {
            case token_type::PLUS:
                op = arena_.create<ast::binary_plus>();
                break;
            case token_type::MINUS:
                op = arena_.create<ast::binary_minus>();
                break;
            case token_type::ASTERISK:
                op = arena_.create<ast::binary_mult>();
                break;
            case token_type::FSLASH:
                op = arena_.create<ast::binary_div>();
                break;
            case token_type::ISEQUAL:
                op = arena_.create<ast::comp_equals>();
                break;
            case token_type::ISGT:
                op = arena_.create<ast::comp_gt>();
                break;
            case token_type::ISGE:
                op = arena_.create<ast::comp_ge>();
                break;
            case token_type::ISLT:
                op = arena_.create<ast::comp_lt>();
                break;
            case token_type::ISLE:
                op = arena_.create<ast::comp_le>();
                break;
            case token_type::AND:
                op = arena_.create<ast::logical_and>();
                break;
            case token_type::OR:
                op = arena_.create<ast::logical_or>();
                break;
            case token_type::COLON:
                op = arena_.create<ast::var_decl>();
                ENSURE(dynamic_cast<ast::variable*>(a) != nullptr,
                       "Left term of variable declaration is not a variable. Variable name must be a non keyword, alpha - numeric and should not start with a digit.");
                rghtgen = &parser_engine::atom;
                checkfortype = true;
                break;
            case token_type::ASSIGN:
                op = arena_.create<ast::assign>();
                ENSURE(dynamic_cast<ast::variable*>(a) != nullptr,
                       "Left term of assignment is not a variable. Variable name must be a non keyword, alpha - numeric and should not start with a digit.");
                rghtgen = &parser_engine::expr;
                break;
            default:
                PCSH_ASSERT_MSG(false, "Invalid binary operation!");
                break;
        }
        m[op] = source_info{ parser_.filename_, std::to_string(parser_.line()), func_ };
        advance();
        op->set_left(a);
        auto right = call_mem_fn(this, rghtgen, m);
        if (checkfortype) {
            ENSURE(dynamic_cast<ast::type_constant*>(right) != nullptr,
                   "Right term of variable declaration is not a type. Type name must be one of `int', `string' or `float'.");
        }
        op->set_right(right);
        return op;
    }

    ast::untyped_unary_op_base* parser::parser_engine::create_unary_op(const token& nxt, source_map& m)
    {
        ast::untyped_unary_op_base* op = nullptr;
        switch (nxt.type()) {
            case token_type::MINUS:
                op = arena_.create<ast::unary_minus>();
                break;
            case token_type::PLUS:
                op = arena_.create<ast::unary_plus>();
                break;
            case token_type::NOT:
                op = arena_.create<ast::unary_not>();
                break;
            default:
                PCSH_ASSERT_MSG(false, "Invalid binary operation!");
                break;
        }
        m[op] = source_info{ parser_.filename_, std::to_string(parser_.line()), func_ };
        advance();
        op->set_operand(factor(m));
        return op;
    }

    ast::block* parser::parser_engine::block(source_map& m)
    {
        std::vector<ast::node*> stmts;

        auto t = peek();
        bool init_lbrace = t.is_a(token_type::LBRACE);

        if (init_lbrace) {
            advance();
        }

        while (true) {
            if (t.is_a(token_type::EOS)) {
                if (!init_lbrace) {
                    break;
                } else {
                    ENSURE(false, "Unexpected end of stream while reading a block. Expected a `}' before termination.");
                }
            }
            if (t.is_a(token_type::RBRACE)) {
                if (!init_lbrace) {
                    ENSURE(false, "Unexpected `}'. Did not see a `{' to start a block.");
                } else {
                    advance();
                    break;
                }
            }
            stmts.push_back(stmt(m));
            t = peek();
        }

        ast::block* blk = arena_.create<ast::block>(arena_);
        auto beg = stmts.rbegin();
        const auto end = stmts.rend();
        for (; beg != end; ++beg) {
            blk->push_front_statement(*beg);
        }
        return blk;
    }

    ast::node* parser::parser_engine::expr(source_map& m)
    {
        ast::node* a = arith(m);
        auto t = peek();
        while (t.is_a(token_type::PLUS) || t.is_a(token_type::MINUS) || t.is_a(token_type::ISEQUAL)) {
            a = create_binary_op(t, a, m, &parser_engine::arith);
            t = peek();
        }
        return a;
    }

    ast::node* parser::parser_engine::stmt(source_map& m)
    {
        auto t = peek();
        ast::node* op = nullptr;
        if (t.is_a(token_type::IF)) {
            op = ifstmt(m);
        } else if (t.is_a(token_type::LBRACE)) {
            op = block(m);
        } else {
            op = expr(m);
            ENSURE(peek().is_a(token_type::SEMICOLON), "Expected the end of a statement with `;'");
            advance();
        }
        return op;
    }

    ast::node* parser::parser_engine::arith(source_map& m)
    {
        auto a = term(m);
        auto t = peek();
        while (is_binary_op(t)) {
            a = create_binary_op(t, a, m, &parser_engine::term);
            t = peek();
        }
        return a;
    }

    ast::node* parser::parser_engine::term(source_map& m)
    {
        ast::node* a = unop(m);
        auto t = peek();
        if (is_binary_op(t)) {
            a = create_binary_op(t, a, m, &parser_engine::unop);
        }
        return a;
    }

    ast::node* parser::parser_engine::factor(source_map& m)
    {
        auto t = peek();
        if (t.is_a(token_type::LPAREN)) {
            advance();
            auto e = expr(m);
            t = peek();
            ENSURE(t.is_a(token_type::RPAREN), "Unmatched `(' when parsing an expression.");
            advance();
            return e;
        } else {
            return atom(m);
        }
    }

    ast::node* parser::parser_engine::ifstmt(source_map& m)
    {
        auto t = peek();
        PCSH_ASSERT_MSG(t.is_a(token_type::IF), "Expected an `if' statement.");
        advance();
        t = peek();
        ENSURE(t.is_a(token_type::LPAREN), "Expected a `(' after an if.");
        advance();
        auto cond = expr(m);
        t = peek();
        ENSURE(t.is_a(token_type::RPAREN), "Expected a `)' after an if statement expression.");
        advance();
        auto ifnode = arena_.create<ast::if_stmt>(cond, stmt(m));
        t = peek();
        if (t.is_a(token_type::ELSE)) {
            advance();
            ifnode->set_else_body(stmt(m));
        }
        return ifnode;
    }

    ast::node* parser::parser_engine::unop(source_map& m)
    {
        auto t = peek();
        if (is_unary_op(t)) {
            return create_unary_op(t, m);
        } else {
            return factor(m);
        }
    }

    namespace conversions {

        PCSH_INLINE int to_int(const token& t)
        {
            size_t len = t.length();
            char* s = const_cast<char*>(t.str().ptr);
            auto oldval = s[len];
            s[len] = '\0';
            int i = ::atoi(s);
            s[len] = oldval;
            return i;
        }

        PCSH_INLINE double to_double(const token& t)
        {
            size_t len = t.length();
            char* s = const_cast<char*>(t.str().ptr);
            auto oldval = s[len];
            s[len] = '\0';
            double f = ::atof(s);
            s[len] = oldval;
            return f;
        }

        result_type token_type_to_result(token_type t)
        {
            switch (t) {
                case token_type::TYPE_INT:
                    return result_type::INTEGER;
                case token_type::TYPE_STRING:
                    return result_type::STRING;
                case token_type::TYPE_FLOAT:
                    return result_type::FLOATING;
                default:
                    return result_type::FAILED;
            }
        }

    }//namespace conversions

    ast::node* parser::parser_engine::atom(source_map& m)
    {
        auto t = peek();
        ast::untyped_atom_base* v = nullptr;
        switch (t.type()) {
            case token_type::SYMBOL:
                v = arena_.create<ast::variable>(arena_.create_string(t.str().ptr, t.length()));
                break;
            case token_type::INTEGER:
                v = arena_.create<ast::int_constant>(conversions::to_int(t));
                break;
            case token_type::FLOATING:
                v = arena_.create<ast::float_constant>(conversions::to_double(t));
                break;
            case token_type::QUOTE: {
                // we have a static string's data here. copy into a new string
                cstring str = arena_.create_string(t.str().ptr);
                v = arena_.create<ast::string_constant>(str);
                break;
            }
            case token_type::TYPE_INT:
            case token_type::TYPE_STRING:
            case token_type::TYPE_FLOAT: {
                auto ty = conversions::token_type_to_result(t.type());
                ENSURE(ty != result_type::FAILED, "Expect a type, either `int', `string' or `float'.");
                v = arena_.create<ast::type_constant>(ty);
                break;
            }
            default:
                PCSH_ASSERT_MSG(false, "Invalid atom value!");
                break;
        }
        m[v] = source_info{ parser_.filename_, std::to_string(parser_.line()), func_ };
        advance();
        return v;
    }

}//namespace parser
}//namespace pcsh

