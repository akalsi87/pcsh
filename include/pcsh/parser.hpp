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

#if defined(_MSC_VER)
#  pragma warning(disable:4251)
#endif // defined(_MSC_VER)

namespace pcsh {
namespace parser {

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
        QUOTE,
        SYMBOL,
        INTEGER,
        FLOATING,
        EOS,
        IF,
        ISEQUAL,
        NONE,
        FAIL
    };

    //////////////////////////////////////////////////////////////////////////
    /// buff_string : in-memory buffer view of a stream
    //////////////////////////////////////////////////////////////////////////

    struct buff_string
    {
        cstring ptr;
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

      private:
        token(token_type t, cstring s, size_t l) : type_(t), str_(s), len_(l)
        { }

        static token get(token_type t, cstring nm = nullptr, size_t len = 0);

        friend class parser;

        token_type type_;
        cstring str_;
        size_t len_;
    };

    //////////////////////////////////////////////////////////////////////////
    /// parser exception
    //////////////////////////////////////////////////////////////////////////

    void throw_parser_exception(const std::string& msg, const std::string& fname, const std::string& func, const std::string& line);

    class exception
    {
      public:
        inline ~exception()
        { }

        inline const std::string& message() const { return msg_; }

        inline const std::string& filename() const { return fname_; }

        inline const std::string& fcn() const { return func_; }

        inline const std::string& line() const { return line_; }
      private:
        std::string msg_;
        std::string fname_;
        std::string func_;
        std::string line_;

        inline exception(const std::string& msg, const std::string& fname, const std::string& func, const std::string& line)
          : msg_(msg), fname_(fname), func_(func), line_(line)
        { }

        friend void throw_parser_exception(const std::string&, const std::string&, const std::string&, const std::string&);
    };

    //////////////////////////////////////////////////////////////////////////
    /// parser
    //////////////////////////////////////////////////////////////////////////

    using pos_t = size_t;

    class PCSH_API parser
    {
      public:
        parser(std::istream& is, const std::string& filename = "(test)");

        ~parser();

        token peek(pos_t p = 0, pos_t* pactstart = nullptr);

        void advance(pos_t len, bool countnl = true);

        inline int line() const
        {
            return line_;
        }

        inline pos_t line_start() const
        {
            return line_start_;
        }

        pos_t curr_pos() const;

        // returns a valid executable tree, except for use before assign errors.
        ir::tree::ptr parse_to_tree();

        void sync_stream();
      private:
        class buffered_stream;

        class parser_engine;

        buffered_stream* strm_;
        int line_;
        pos_t line_start_;
        std::string filename_;

        inline token peek_impl(pos_t p = 0, pos_t* pactstart = nullptr);
        inline void advance_impl(pos_t len, bool countnl = true);
        pos_t find_first_non_whitespace(pos_t start);
        pos_t skip_till_line_end(pos_t p);
        token read_string(pos_t p);
        token read_number(pos_t p);
        token read_name(pos_t p);
        std::string copy_line(pos_t p);
    };

}// namespace parser
}// namespace pcsh

#if defined(_MSC_VER)
#  pragma warning(default:4251)
#endif // defined(_MSC_VER)

#endif/*PCSH_PARSER_HPP*/
