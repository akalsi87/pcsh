/**
 * \file parser.hpp
 * \date Jan 27, 2016
 */

#ifndef PCSH_PARSER_HPP
#define PCSH_PARSER_HPP

#include "pcsh/exportsym.h"
#include "pcsh/arena.hpp"
#include "pcsh/ir.hpp"
#include "pcsh/types.hpp"

#include <iostream>
#include <memory>

namespace pcsh {
namespace parser {

///
// Based on Bish
// http://www.github.com/tdenniston/bish
//

/*
Grammar:

module ::= block
block ::= '{' { stmt } '}'
stmt ::= assign ';'
| funcall ';'
| externcall ';'
| 'import' any ';'
| 'return' expr ';'
| 'break' ';'
| 'continue' ';'
| '#' any NEWLINE
| 'if' '(' expr ')' block
| 'if' '(' expr ')' block { 'else' 'if' '(' expr ')' block } 'else' block
| 'for' '(' var 'in' atom [ '..' atom ] ')' block
| 'def' var '(' varlist ')' block
| block
assign ::= location '=' expr
| location '=' '[' exprlist ']'
funcall ::= namespacedvar '(' exprlist ')'
externcall ::= '@' '(' interp ')'
expr ::= expr '|' logical | logical
logical ::= logical 'and' equality | logical 'or' equality | equality
equality ::= equality '==' relative | equality '!=' relative | relative
relative ::= relative '<' arith | relative '>' arith
| relative '<=' arith | relative '>=' arith
| arith
arith ::= arith '+' term | arith '-' term | term
term ::= term '*' unary | term '/' unary | term '%' unary | unary
unary ::= '-' unary | 'not' unary | factor
factor ::= '( expr ')' | funcall | externcall | atom
atom ::= location | NUMBER | '"' STRING '"' | 'true' | 'false'
var ::= { ALPHANUM | '_' }
location ::= namespacedvar | namespacedvar '[' expr ']'
namespacedvar ::= [ var '.' ] var
varlist ::= var { ',' var }
exprlist ::= expr { ',' expr }
interp ::= { str | '$' namespacedvar | '$' '(' any ')'}
*/

    //////////////////////////////////////////////////////////////////////////
    /// token_type
    //////////////////////////////////////////////////////////////////////////

    enum class token_type : byte
    {
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        SEMICOLON,
        ASSIGN,
        PLUS,
        MINUS,
        ASTERISK,
        DOT,
        FSLASH,
        SYMBOL,
        INTEGER,
        FLOATING,
        EOS,
        NONE,
        FAIL
    };

    //////////////////////////////////////////////////////////////////////////
    /// buff_string : in-memory buffer view of a stream
    //////////////////////////////////////////////////////////////////////////

    struct buff_string
    {
        name   ptr;
        size_t len;

        inline bool equals(const char* s) const
        {
            return strncmp(ptr, s, len) == 0;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    /// token
    //////////////////////////////////////////////////////////////////////////

    class PCSH_API token
    {
      public:
        token(const token& rhs) : type_(rhs.type_), str_(rhs.str_), len_(rhs.len_)
        { }

        inline bool defined() const
        {
            return type_ != token_type::NONE;
        }

        inline bool is_a(token_type x) const
        {
            return type_ == x;
        }

        inline size_t length() const
        {
            return len_;
        }

        inline buff_string str() const
        {
            buff_string rv = { str_, len_ };
            return rv;
        }

        std::string to_string() const;

      private:
        token(token_type t, name s, size_t l) : type_(t), str_(s), len_(l)
        { }

        static token get(token_type t, name nm = nullptr, size_t len = 0);

        friend class parser;

        token_type type_;
        name str_;
        size_t len_;
    };

    //////////////////////////////////////////////////////////////////////////
    /// parser
    //////////////////////////////////////////////////////////////////////////

    typedef size_t pos_t;

    class PCSH_API parser
    {
      public:
        parser(std::istream& is);

        ~parser();

        token peek(pos_t p = 0, pos_t* pactstart = nullptr);

        void advance(pos_t len);

        inline int line() const
        {
            return line_;
        }

        inline pos_t line_start() const
        {
            return line_start_;
        }

        pos_t curr_pos() const;

        ir::tree parse_to_tree(arena& a);

        void sync_stream();
      private:
        class buffered_stream;

        buffered_stream* strm_;
        int line_;
        pos_t line_start_;

        pos_t find_first_non_whitespace(pos_t start);
        pos_t skip_till_line_end(pos_t p);
        token read_number(pos_t p);
        token read_symbol(pos_t p);
    };

}// namespace parser
}// namespace pcsh

#endif/*PCSH_PARSER_HPP*/
