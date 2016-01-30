/**
 * \file parser.cpp
 * \date Jan 27, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

namespace pcsh {
namespace parser {

    token token::get(token_type t, name nm /*= nullptr*/)
    {
        switch (t) {
            case token_type::ASSIGN:
                PCSH_ASSERT(nm[0] == '=');
                nm = "=";
                break;
            case token_type::ASTERISK:
                PCSH_ASSERT(nm[0] == '*');
                nm = "*";
                break;
            case token_type::EOS:
                PCSH_ASSERT(nm[0] == '\xFF');
                nm = "\xFF";
                break;
            case token_type::FLOATING:
                PCSH_ASSERT(nm);
                break;
            case token_type::FSLASH:
                PCSH_ASSERT(nm[0] == '/');
                nm = "/";
                break;
            case token_type::INTEGER:
                PCSH_ASSERT(nm);
                break;
            case token_type::LBRACE:
                PCSH_ASSERT(nm[0] == '{');
                nm = "{";
                break;
            case token_type::LPAREN:
                PCSH_ASSERT(nm[0] == '(');
                nm = "(";
                break;
            case token_type::MINUS:
                PCSH_ASSERT(nm[0] == '-');
                nm = "-";
                break;
            case token_type::NONE:
                PCSH_ASSERT(!nm);
                nm = nullptr;
                break;
            case token_type::PLUS:
                PCSH_ASSERT(nm[0] == '+');
                nm = "+";
                break;
            case token_type::RBRACE:
                PCSH_ASSERT(nm[0] == '}');
                nm = "}";
                break;
            case token_type::RPAREN:
                PCSH_ASSERT(nm[0] == ')');
                nm = ")";
                break;
            case token_type::SEMICOLON:
                PCSH_ASSERT(nm[0] == ';');
                nm = ";";
                break;
            case token_type::SYMBOL:
                PCSH_ASSERT(nm);
                break;
            default:
                PCSH_ASSERT_MSG(false, "Invalid token type!");
                break;
        }
        return token(t, nm);
    }

}// namespace parser
}// namespace pcsh
