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

block ::= '{' { stmt ';' } '}'
stmt ::= var '=' expr
var ::= NAME
expr ::= '(' expr ')' | unop atom | atom { binop atom } | atom
binop ::= '+' | '-' | '*' | '/'
unop ::= '-'
atom ::= var | NUMBER
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

        inline token_type type() const
        {
            return type_;
        }

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
        parser(std::istream& is, const std::string& filename = "(test)");

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

        ir::tree* parse_to_tree(arena& a);

        void sync_stream();
      private:
        class buffered_stream;

        class parser_engine;

        buffered_stream* strm_;
        int line_;
        pos_t line_start_;
        const std::string& filename_;

        pos_t find_first_non_whitespace(pos_t start);
        pos_t skip_till_line_end(pos_t p);
        token read_number(pos_t p);
        token read_symbol(pos_t p);
    };

}// namespace parser
}// namespace pcsh

#endif/*PCSH_PARSER_HPP*/
