/**
 * \file parser.hpp
 * \date Jan 27, 2016
 */

#ifndef PCSH_PARSER_HPP
#define PCSH_PARSER_HPP

#include "pcsh/exportsym.h"
#include "pcsh/arena.hpp"
#include "pcsh/ast.hpp"
#include "pcsh/types.hpp"

#include <iostream>
#include <memory>

#if defined(_MSC_VER)
#  pragma warning(disable:4251)
#endif // defined(_MSC_VER)

#if !defined(NOTHROW)
#  if defined(_MSC_VER)
#    define NOTHROW throw()
#  else
#    define NOTHROW noexcept
#  endif // defined(_MSC_VER)
#endif // !defined(NOTHROW)

#if !defined(PCSH_INLINE)
#  if !defined(_MSC_VER)
#    define PCSH_INLINE __inline__ __attribute__((always_inline))
#  else
#    define PCSH_INLINE __forceinline
#  endif
#endif

#if !defined(_MSC_VER)
#  define PCSH_UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define PCSH_LIKELY(x)   __builtin_expect(!!(x), 1)
#else
#  define PCSH_UNLIKELY(x) (x)
#  define PCSH_LIKELY(x)   (x)
#endif

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
        ELSE,
        ISEQUAL,
        ISGT,
        ISLT,
        ISGE,
        ISLE,
        NOT,
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

        PCSH_INLINE bool equals(const char* s) const
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
        token(const token& rhs) : str_(rhs.str_), len_(rhs.len_), type_(rhs.type_)
        { }

        PCSH_INLINE token_type type() const
        {
            return type_;
        }

        PCSH_INLINE bool defined() const
        {
            return type_ != token_type::NONE;
        }

        PCSH_INLINE bool is_a(token_type x) const
        {
            return type_ == x;
        }

        PCSH_INLINE size_t length() const
        {
            return len_;
        }

        PCSH_INLINE buff_string str() const
        {
            buff_string rv = { str_, len_ };
            return rv;
        }

      private:
        token(token_type t, cstring s, size_t l) : str_(s), len_(l), type_(t)
        { }

        static token get(token_type t, cstring nm = nullptr, size_t len = 0);

        friend class parser;

        cstring str_;
        size_t len_;
        token_type type_;
    };

    //////////////////////////////////////////////////////////////////////////
    /// parser exception
    //////////////////////////////////////////////////////////////////////////

    void throw_parser_exception(const std::string& msg, const std::string& fname, const std::string& func, const std::string& line);

    class PCSH_API exception : public std::exception
    {
      public:
        ~exception();

        inline const std::string& message() const { return msg_; }

        inline const std::string& filename() const { return fname_; }

        inline const std::string& fcn() const { return func_; }

        inline const std::string& line() const { return line_; }

        inline const char* what() const NOTHROW { return what_.c_str(); }
      private:
        std::string msg_;
        std::string fname_;
        std::string func_;
        std::string line_;
        std::string what_;

        inline exception(const std::string& msg, const std::string& fname, const std::string& func, const std::string& line)
          : msg_(msg), fname_(fname), func_(func), line_(line), what_()
        {
            what_ = std::string("Exception!\n\t") +
                "  message:  " + msg_   + "\n\t"
                "  filename: " + fname_ + "\n\t"
                "  function: " + func_  + "\n\t"
                "  line:     " + line_  + "\n";
        }

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
        ast::tree::ptr parse_to_tree();

        void sync_stream();
      private:
        class buffered_stream;

        class parser_engine;

        buffered_stream* strm_;
        int line_;
        pos_t line_start_;
        std::string filename_;

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
