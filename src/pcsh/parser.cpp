/**
 * \file parser.cpp
 * \date Jan 27, 2016
 */

// disable security warnings for MSVC for std::copy()
#if defined(_MSC_VER)
#  define _SCL_SECURE_NO_WARNINGS
#endif // defined(_MSC_VER)

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include "ir/nodes.hpp"
#include "ir/passes/type_checker.hpp"
#include "ir/tree_validation.hpp"
#include "parser_engine.hpp"

#include <cstring>
#include <string>

///
// Based on Bish
// http://www.github.com/tdenniston/bish
//

/*
Grammar:
-------
block ::= ['{'] [stmt]+ ['}']
stmt ::= expr ';' | block | if '(' expr ')' stmt
expr ::= expr '==' arith | var '=' arith | arith
arith ::= arith '+' term | arith '-' term | term
term ::= term '*' unary | term '/' unary | unary
unary ::= '-' unary | '+' unary | factor
factor ::= '( expr ')' | atom
atom ::= var | NUMBER | '"' STRING '"'
var ::= STRING
*/

namespace pcsh {
namespace parser {

    //////////////////////////////////////////////////////////////////////////
    /// token
    //////////////////////////////////////////////////////////////////////////

    token token::get(token_type t, cstring nm, size_t len)
    {
        switch (t) {
            case token_type::ASSIGN:
                PCSH_ASSERT(nm[0] == '=');
                PCSH_ASSERT(len == 1);
                nm = "=";
                break;
            case token_type::ASTERISK:
                PCSH_ASSERT(nm[0] == '*');
                PCSH_ASSERT(len == 1);
                nm = "*";
                break;
            case token_type::DOT:
                PCSH_ASSERT(nm[0] == '.');
                PCSH_ASSERT(len == 1);
                nm = ".";
                break;
            case token_type::EOS:
                PCSH_ASSERT(nm[0] == '\xFF');
                PCSH_ASSERT(len == 1);
                nm = "\xFF";
                break;
            case token_type::FLOATING:
                PCSH_ASSERT(nm);
                break;
            case token_type::FSLASH:
                PCSH_ASSERT(nm[0] == '/');
                PCSH_ASSERT(len == 1);
                nm = "/";
                break;
            case token_type::IF:
                PCSH_ASSERT(::strncmp(nm, "if", len) == 0);
                break;
            case token_type::INTEGER:
                PCSH_ASSERT(nm);
                break;
            case token_type::LBRACE:
                PCSH_ASSERT(nm[0] == '{');
                PCSH_ASSERT(len == 1);
                nm = "{";
                break;
            case token_type::LPAREN:
                PCSH_ASSERT(nm[0] == '(');
                PCSH_ASSERT(len == 1);
                nm = "(";
                break;
            case token_type::MINUS:
                PCSH_ASSERT(nm[0] == '-');
                PCSH_ASSERT(len == 1);
                nm = "-";
                break;
            case token_type::NONE:
                PCSH_ASSERT(!nm);
                PCSH_ASSERT(len == 0);
                nm = nullptr;
                break;
            case token_type::PLUS:
                PCSH_ASSERT(nm[0] == '+');
                PCSH_ASSERT(len == 1);
                nm = "+";
                break;
            case token_type::QUOTE:
                //PCSH_ASSERT(nm[0] == '"');
                //PCSH_ASSERT(len == 1);
                //nm = "\"";
                break;
            case token_type::RBRACE:
                PCSH_ASSERT(nm[0] == '}');
                PCSH_ASSERT(len == 1);
                nm = "}";
                break;
            case token_type::RPAREN:
                PCSH_ASSERT(nm[0] == ')');
                PCSH_ASSERT(len == 1);
                nm = ")";
                break;
            case token_type::SEMICOLON:
                PCSH_ASSERT(nm[0] == ';');
                PCSH_ASSERT(len == 1);
                nm = ";";
                break;
            case token_type::SYMBOL:
                PCSH_ASSERT(nm);
                break;
            case token_type::FAIL:
                PCSH_ASSERT(nm);
                len = strlen(nm);
                break;
            case token_type::ISEQUAL:
                PCSH_ASSERT((len == 2) && (nm[0] == '=') && (nm[1] == '='));
                break;
            default:
                PCSH_ASSERT_MSG(false, "Invalid token type!");
                break;
        }
        return token(t, nm, len);
    }

#if 0
    std::string token::to_string() const
    {
        auto str = std::string();
        switch (type_) {
            case token_type::ASSIGN:
                str += "assign        | ";
                break;
            case token_type::ASTERISK:
                str += "asterisk      | ";
                break;
            case token_type::DOT:
                str += "dot           | ";
                break;
            case token_type::EOS:
                str += "end-of-stream | ";
                break;
            case token_type::FLOATING:
                str += "float         | ";
                break;
            case token_type::FSLASH:
                str += "fwd-slash     | ";
                break;
            case token_type::INTEGER:
                str += "integer       | ";
                break;
            case token_type::LBRACE:
                str += "lbrace        | ";
                break;
            case token_type::LPAREN:
                str += "lparen        | ";
                break;
            case token_type::MINUS:
                str += "minus         | ";
                break;
            case token_type::NONE:
                str += "none          | ";
                break;
            case token_type::PLUS:
                str += "plus          | ";
                break;
            case token_type::QUOTE:
                str += "quote         | ";
                break;
            case token_type::RBRACE:
                str += "rbrace        | ";
                break;
            case token_type::RPAREN:
                str += "rparen        | ";
                break;
            case token_type::SEMICOLON:
                str += "semicolon     | ";
                break;
            case token_type::SYMBOL:
                str += "symbol        | ";
                break;
            case token_type::FAIL:
                str += "fail          | ";
                break;
            case token_type::IF:
                str += "if            |";
                break;
            case token_type::ISEQUAL:
                str += "isequal       |";
                break;
        }
        return str.append(str_, str_ + len_);
    }

#endif // 0

    //////////////////////////////////////////////////////////////////////////
    /// parser
    //////////////////////////////////////////////////////////////////////////

    namespace tokenize {

        PCSH_INLINE bool is_sign(char c)
        {
            return (c == '+') || (c == '-');
        }

        PCSH_INLINE bool is_newline(char c)
        {
            return (c == '\n') || (c == '\r');
        }

        PCSH_INLINE bool is_space(char c)
        {
            return (c == '\t') || (c == ' ');
        }

        PCSH_INLINE bool is_whitespace(char c)
        {
            return is_newline(c) || is_space(c);
        }

        PCSH_INLINE bool is_digit(char c)
        {
            return (c >= '0') && (c <= '9');
        }

        PCSH_INLINE bool is_lower_alpha(char c)
        {
            return (c >= 'a') && (c <= 'z');
        }

        PCSH_INLINE bool is_upper_alpha(char c)
        {
            return (c >= 'A') && (c <= 'Z');
        }

        PCSH_INLINE char to_lower(char c)
        {
            return (c - 'A' + 'a');
        }

        PCSH_INLINE char to_upper(char c)
        {
            return (c - 'a' + 'A');
        }

        PCSH_INLINE bool is_comment_char(char c)
        {
            return (c == '#');
        }

        PCSH_INLINE bool is_symbol_char(char c)
        {
            return is_digit(c) || is_lower_alpha(c) || is_upper_alpha(c) || (c == '_');
        }

        PCSH_INLINE bool is_start_of_number(char c, char n, char o)
        {
            return is_digit(c) || ((is_sign(c) || (c == '.')) && is_digit(n)) || (is_sign(c) && (n == '.') && is_digit(o));
        }

    }//namespace tokenize

#if defined(_MSC_VER)
#  pragma warning(disable:4351)
#endif // defined(_MSC_VER)

    template <int N>
    class flex_buffer
    {
      private:
        mutable char stackbuff[N + 1];
        size_t sz;
        mutable std::string heapbuff;
        bool useheap;
      public:
        flex_buffer() : stackbuff(), sz(0), heapbuff(), useheap(false)
        {
            stackbuff[N] = '\0';
        }

        inline char& operator[](size_t i) const
        {
            return useheap ? heapbuff[i] : stackbuff[i];
        }

        inline size_t size() const
        {
            return useheap ? heapbuff.size() : sz;
        }

        inline void resize(size_t s)
        {
            if (!useheap && (s <= N)) {
                sz = s;
                return;
            }

            if (!useheap) {
                heapbuff.reserve(2 * s);
            }

            heapbuff.resize(s);
            if (!useheap) {
                std::copy(stackbuff, stackbuff + sz, &heapbuff[0]);
                useheap = true;
            }
        }
    };

#if defined(_MSC_VER)
#  pragma warning(default:4996)
#endif // defined(_MSC_VER)

    /// buffered_stream
    class parser::buffered_stream
    {
      public:
        static const int EOS = -1;
      public:
        buffered_stream(std::istream& is) : strm_(is), buffer_(), buffpos_(0), buffsz_(0), pos_(0)
        {
        }

        PCSH_INLINE const char* buff() const
        {
            return &buffer_[buffpos_];
        }

        PCSH_INLINE int peek_at(pos_t pos)
        {
            return has_chars(pos + 1) ? buffer_[buffpos_ + pos] : EOS;
        }

        PCSH_INLINE pos_t pos() const
        {
            return pos_ + buffpos_;
        }

        void advance(pos_t n = 1)
        {
            size_t nleft = buffsz_ - buffpos_;
            if (nleft >= n) {
                buffpos_ += n;
                return;
            }
            pos_ += buffsz_;
            buffpos_ = 0;
            buffsz_ = 0;
            n -= nleft;
            while (n > 0) {
                fill_buffer(buffer_.size());
                if (buffsz_ == 0) {
                    break;
                }
                if (n < buffsz_) {
                    buffpos_ += n;
                    break;
                }
                pos_ += buffsz_;
                n -= buffsz_;
            }
        }

        void sync_stream()
        {
            strm_.seekg(pos_ + buffpos_, strm_.beg);
            if (!strm_.good()) {
                strm_.clear();
            }
            pos_ += buffpos_;
            buffpos_ = 0;
            buffsz_ = 0;
        }
      private:
        static const int INIT_SIZE = 256;

        std::istream&          strm_;
        flex_buffer<INIT_SIZE> buffer_;
        pos_t                  buffpos_;
        pos_t                  buffsz_;
        pos_t                  pos_;

        PCSH_INLINE bool has_chars(pos_t n)
        {
            if (buffsz_ >= (buffpos_ + n)) {
                return true;
            }
            fill_buffer(n);
            return buffsz_ >= (buffpos_ + n);
        }

        void fill_buffer(pos_t n)
        {
            n = std::max(n, pos_t(INIT_SIZE));

            if ((buffpos_ + n) > buffer_.size()) {
                buffer_.resize(buffpos_ + n);
            }

            strm_.read(&buffer_[buffsz_], n - (buffsz_ - buffpos_));
            buffsz_ += (pos_t)strm_.gcount();

            if (!strm_.good()) {
                strm_.clear();
            }
        }
    };

    token parser::peek(pos_t pstrt, pos_t* pactstrt)
    {
        using namespace tokenize;
        // skip whitespace
        auto p = find_first_non_whitespace(pstrt);
        if (pactstrt) {
            *pactstrt = p;
        }

        int c = strm_->peek_at(p);

        switch (c) {
            case buffered_stream::EOS:
                return token::get(token_type::EOS, "\xFF", 1);
            case '(':
                return token::get(token_type::LPAREN, "(", 1);
            case ')':
                return token::get(token_type::RPAREN, ")", 1);
            case '{':
                return token::get(token_type::LBRACE, "{", 1);
            case '}':
                return token::get(token_type::RBRACE, "}", 1);
            case ';':
                return token::get(token_type::SEMICOLON, ";", 1);
            case '=':
                return (strm_->peek_at(p + 1) == '=') ? token::get(token_type::ISEQUAL, "==", 2)
                                                      : token::get(token_type::ASSIGN, "=", 1);
            case '+':
                return token::get(token_type::PLUS, "+", 1);
            case '-':
                return token::get(token_type::MINUS, "-", 1);
            case '*':
                return token::get(token_type::ASTERISK, "*", 1);
            case '.':
                return token::get(token_type::DOT, ".", 1);
            case '"':
                return read_string(p);
            case '/':
                return token::get(token_type::FSLASH, "/", 1);
            default:
                return (is_digit(c) ? read_number(p) : read_name(p));
        }
    }

    void parser::advance(pos_t len, bool countnl)
    {
        pos_t p = 0;
        if (PCSH_LIKELY(countnl)) {
            while (p < len) {
                int c = strm_->peek_at(p);
                if (PCSH_UNLIKELY(tokenize::is_newline(c))) {
                    if (c == '\r' && strm_->peek_at(p + 1) == '\n') {
                        ++p;
                    }
                    ++line_;
                    line_start_ = strm_->pos() + p;
                }
                ++p;
            }
        }
        strm_->advance(len);
    }

    parser::parser(std::istream& is, const std::string& filename)
      : strm_(new buffered_stream(is))
      , line_(1)
      , line_start_(0)
      , filename_(filename)
    {
    }

    parser::~parser()
    {
        sync_stream();
        delete strm_;
    }

    void parser::sync_stream()
    {
        strm_->sync_stream();
    }

    PCSH_INLINE pos_t parser::find_first_non_whitespace(pos_t p)
    {
        using namespace tokenize;
        int c = strm_->peek_at(p);
        if (PCSH_LIKELY(!is_whitespace(c) && !is_comment_char(c))) {
            return p;
        }
        while (true) {
            c = strm_->peek_at(p);
            if (is_whitespace(c)) {
                ++p;
            } else if (is_comment_char(c)) {
                ++p;
                p = skip_till_line_end(p);
            } else {
                return p;
            }
        }
    }

    pos_t parser::skip_till_line_end(pos_t p)
    {
        using namespace tokenize;
        while (true) {
            int c = strm_->peek_at(p);
            if (PCSH_UNLIKELY(is_newline(c))) {
                return p;
            } else if (PCSH_UNLIKELY(c == strm_->EOS)) {
                return p;
            }
            ++p;
        }
    }

    token parser::read_string(pos_t p)
    {
        //return token::get(token_type::QUOTE, "\"", 1);
        static std::string buffer;
        buffer.reserve(1024);
        buffer.resize(0);
        pos_t startp = ++p; // skip past start quote
        int c = strm_->peek_at(p);
        while (true) {
            if (c == strm_->EOS) {
                return token::get(token_type::FAIL, "End-of-stream while reading string literal.");
            }
            if (c == '"') {
                break;
            }
            if (c == '\\') {
                int n = strm_->peek_at(++p);
                switch (n) {
                    case 't':
                        c = '\t';
                        break;
                    case 'n':
                        c = '\n';
                        break;
                    case 'v':
                        c = '\v';
                        break;
                    case '\\':
                        c = '\\';
                        break;
                    case 'a':
                        c = '\a';
                        break;
                    case 'r':
                        c = '\r';
                        break;
                    case 'b':
                        c = '\b';
                        break;
                    default:
                        c = n;
                        break;
                }
            }
            buffer.push_back(c);
            c = strm_->peek_at(++p);
        }

        return token::get(token_type::QUOTE, buffer.c_str(), p - startp/* acct for close quote */);
    }

    token parser::read_number(pos_t p)
    {
        using namespace tokenize;
        auto pstart = p;

        // find digits end
        auto digend = p;
        while (is_digit(strm_->peek_at(digend))) {
            ++digend;
        }

        // has decimal
        auto hasdec = strm_->peek_at(digend) == '.';

        // has fraction
        auto fracend = digend + hasdec;
        while (is_digit(strm_->peek_at(fracend))) {
            ++fracend;
        }

        bool hasbegdig = p != digend;
        bool hasfracpart = hasdec && ((digend + hasdec) != fracend);

        if (hasbegdig && !hasfracpart && !is_symbol_char(strm_->peek_at(digend))) {
            return token::get(token_type::INTEGER, strm_->buff() + pstart, digend - pstart);
        } else if (hasfracpart && !is_symbol_char(strm_->peek_at(fracend))) {
            return token::get(token_type::FLOATING, strm_->buff() + pstart, fracend - pstart);
        } else {
            return token::get(token_type::FAIL, "Error reading number");
        }
    }

    token parser::read_name(pos_t p)
    {
        using namespace tokenize;
        auto pstart = p;
        while (is_symbol_char(strm_->peek_at(p))) {
            ++p;
        }
        if (p != pstart) {
            buff_string bs { strm_->buff() + pstart, p - pstart };
            if (bs.equals("if")) {
                return token::get(token_type::IF, bs.ptr, bs.len);
            } else {
                return token::get(token_type::SYMBOL, bs.ptr, bs.len);
            }
        } else {
            return token::get(token_type::FAIL, "Error reading symbol");
        }
        return (p != pstart) ? token::get(token_type::SYMBOL, strm_->buff() + pstart, p - pstart)
                             : token::get(token_type::FAIL, "Error reading symbol");
    }

    pos_t parser::curr_pos() const
    {
        return strm_->pos();
    }

    ir::tree::ptr parser::parse_to_tree()
    {
        auto treeptr = ir::tree::create();
        source_map sm;
        sm.reserve(20);
        parser_engine eng(*this, treeptr->get_arena());
        treeptr->set_root(eng.parse(sm));
        try {
            validate_tree(treeptr);
        } catch(const ir::type_checker_error& ex) {
            const source_info& info = sm[ex.left];
            throw_parser_exception(ex.msg, info.filename, info.fcn, info.line);
        }
        return treeptr;
    }

    std::string parser::copy_line(pos_t p)
    {
        std::string str;
        int c;
        while (((c = strm_->peek_at(p++)) != strm_->EOS) && !tokenize::is_newline(c)) {
            str.push_back(c);
        }
        return str;
    }

    void throw_parser_exception(const std::string& msg, const std::string& fname, const std::string& func, const std::string& line)
    {
        throw exception(msg, fname, func, line);
    }

}// namespace parser
}// namespace pcsh
