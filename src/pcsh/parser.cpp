/**
 * \file parser.cpp
 * \date Jan 27, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include <cstring>

namespace pcsh {
namespace parser {

    //////////////////////////////////////////////////////////////////////////
    /// token
    //////////////////////////////////////////////////////////////////////////

    token token::get(token_type t, name nm, size_t len)
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
            default:
                PCSH_ASSERT_MSG(false, "Invalid token type!");
                break;
        }
        return token(t, nm, len);
    }

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
        }
        return str.append(str_, str_ + len_);
    }

    //////////////////////////////////////////////////////////////////////////
    /// parser
    //////////////////////////////////////////////////////////////////////////

    namespace tokenize {

        inline bool is_sign(char c)
        {
            return (c == '+') || (c == '-');
        }

        inline bool is_newline(char c)
        {
            return (c == '\n') || (c == '\r');
        }

        inline bool is_space(char c)
        {
            return (c == '\t') || (c == ' ');
        }

        inline bool is_whitespace(char c)
        {
            return is_newline(c) || is_space(c);
        }

        inline bool is_digit(char c)
        {
            return (c >= '0') && (c <= '9');
        }

        inline bool is_lower_alpha(char c)
        {
            return (c >= 'a') && (c <= 'z');
        }

        inline bool is_upper_alpha(char c)
        {
            return (c >= 'A') && (c <= 'Z');
        }

        inline char to_lower(char c)
        {
            return (c - 'A' + 'a');
        }

        inline char to_upper(char c)
        {
            return (c - 'a' + 'A');
        }

        inline bool is_comment_char(char c)
        {
            return (c == '#');
        }

        inline bool is_symbol_char(char c)
        {
            return is_digit(c) || is_lower_alpha(c) || is_upper_alpha(c) || (c == '_');
        }

        inline bool is_start_of_number(char c, char n, char o)
        {
            using namespace tokenize;
            return is_digit(c) || ((is_sign(c) || (c == '.')) && is_digit(n)) || (is_sign(c) && (n == '.') && is_digit(o));
        }

    }//namespace tokenize

    /// buffered_stream
    class parser::buffered_stream
    {
      public:
        static const char EOS = (char)-1;
      public:
        buffered_stream(std::istream& is) : strm_(is), buffer_(), buffpos_(0), buffsz_(0), pos_(0)
        {
            buffer_.reserve(1024);
        }

        inline const char* buff() const
        {
            return &buffer_[buffpos_];
        }

        inline char peek_at(pos_t pos)
        {
            return has_chars(pos + 1) ? buffer_[buffpos_ + pos] : EOS;
        }

        inline pos_t pos() const
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
            PCSH_ASSERT(pos_ == strm_.tellg());
            buffpos_ = 0;
            buffsz_ = 0;
        }
      private:
        std::istream& strm_;
        std::string   buffer_;
        pos_t         buffpos_;
        pos_t         buffsz_;
        pos_t         pos_;

        inline bool has_chars(pos_t n)
        {
            if (buffsz_ >= (buffpos_ + n)) {
                return true;
            }
            fill_buffer(n);
            return buffsz_ >= (buffpos_ + n);
        }

        inline void fill_buffer(pos_t n)
        {
            if ((buffpos_ + n) > buffer_.size()) {
                buffer_.resize(buffpos_ + n);
            }
            if (strm_.peek() == EOS) {
                return;
            }
            strm_.read(&buffer_[buffsz_], n - (buffsz_ - buffpos_));
            auto actread = strm_.gcount();
            buffsz_ += (pos_t)actread;
        }
    };

    token parser::peek()
    {
        using namespace tokenize;
        // skip whitespace
        auto p = find_first_non_whitespace(0);
        strm_->advance(p);
        char c = strm_->peek_at(0);

        if (is_start_of_number(c, strm_->peek_at(1), strm_->peek_at(2))) {
            return read_number();
        }

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
                return token::get(token_type::ASSIGN, "=", 1);
            case '+':
                return token::get(token_type::PLUS, "+", 1);
            case '-':
                return token::get(token_type::MINUS, "-", 1);
            case '*':
                return token::get(token_type::ASTERISK, "*", 1);
            case '.':
                return token::get(token_type::DOT, ".", 1);
            case '/':
                return token::get(token_type::FSLASH, "/", 1);
            default:
                return read_symbol();
        }
    }

    void parser::advance(const token& t)
    {
        PCSH_ASSERT(!t.is_a(token_type::FAIL));
        strm_->advance(t.str().len);
    }

    ir::tree parser::parse_to_tree(arena& a)
    {
        return ir::tree();
    }

    parser::parser(std::istream& is)
      : strm_(new buffered_stream(is))
      , line_(0)
      , line_start_(0)
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

    pos_t parser::find_first_non_whitespace(pos_t p)
    {
        using namespace tokenize;
        while (true) {
            char c = strm_->peek_at(p);
            if (is_whitespace(c)) {
                if (is_newline(c)) {
                    if (c == '\r' && strm_->peek_at(p + 1) == '\n') {
                        ++p;
                    }
                    ++line_;
                    line_start_ = strm_->pos() + p;
                }
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
            char c = strm_->peek_at(p);
            if (is_newline(c)) {
                return p;
            } else if (c == strm_->EOS) {
                return p;
            }
            ++p;
        }
    }

    token parser::read_number()
    {
        using namespace tokenize;
        auto p = find_first_non_whitespace(0);
        auto beg = p;
        char c = strm_->peek_at(p);

        // chomp sign if exists
        auto hassign = is_sign(c);

        // find digits end
        auto digend = p + hassign;
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

        bool hasbegdig = (p + hassign) != digend;
        bool hasfracpart = hasdec && ((digend + hasdec) != fracend);

        if (hasbegdig && !hasfracpart && !is_symbol_char(strm_->peek_at(digend))) {
            return token::get(token_type::INTEGER, strm_->buff(), digend);
        } else if (hasfracpart && !is_symbol_char(strm_->peek_at(fracend))) {
            return token::get(token_type::FLOATING, strm_->buff(), fracend);
        } else {
            return token::get(token_type::FAIL, "Error reading number");
        }
    }

    token parser::read_symbol()
    {
        using namespace tokenize;
        auto p = find_first_non_whitespace(0);
        while (is_symbol_char(strm_->peek_at(p))) {
            ++p;
        }

        return (p != 0) ? token::get(token_type::SYMBOL, strm_->buff(), p)
                        : token::get(token_type::FAIL, "Error reading symbol");
    }

    pos_t parser::curr_pos() const
    {
        return strm_->pos();
    }

}// namespace parser
}// namespace pcsh
