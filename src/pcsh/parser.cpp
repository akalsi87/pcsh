/**
 * \file parser.cpp
 * \date Jan 27, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include "ir_nodes.hpp"

#include <cstring>
#include <string>
#include <vector>

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
            return is_digit(c) || ((is_sign(c) || (c == '.')) && is_digit(n)) || (is_sign(c) && (n == '.') && is_digit(o));
        }

    }//namespace tokenize

    namespace conversions {

        int to_int(const token& t)
        {
            size_t len = t.length();
            char* s = const_cast<char*>(t.str().ptr);
            auto oldval = s[len];
            s[len] = '\0';
            int i = ::atoi(s);
            s[len] = oldval;
            return i;
        }

        double to_double(const token& t)
        {
            size_t len = t.length();
            char* s = const_cast<char*>(t.str().ptr);
            auto oldval = s[len];
            s[len] = '\0';
            double f = ::atof(s);
            s[len] = oldval;
            return f;
        }

    }//namespace conversions

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
            if (strm_.peek() == EOF) {
                strm_.clear();
                return;
            }
            strm_.read(&buffer_[buffsz_], n - (buffsz_ - buffpos_));
            auto actread = strm_.gcount();
            if (!strm_.good()) {
                strm_.clear();
            }
            buffsz_ += (pos_t)actread;
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
        char c = strm_->peek_at(p);

        if ( is_digit(c) ||
            ((is_sign(c) || (c == '.')) && is_digit(strm_->peek_at(p + 1))) ||
            (is_sign(c) && (strm_->peek_at(p + 1) == '.') && is_digit(strm_->peek_at(p + 2)))) {
            return read_number(p);
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
                return read_symbol(p);
        }
    }

    void parser::advance(pos_t len)
    {
        pos_t p = 0;
        while (p < len) {
            char c = strm_->peek_at(p);
            if (tokenize::is_newline(c)) {
                if (c == '\r' && strm_->peek_at(p + 1) == '\n') {
                    ++p;
                }
                ++line_;
                line_start_ = strm_->pos() + p;
            }
            ++p;
        }
        strm_->advance(len);
    }

    parser::parser(std::istream& is, const std::string& filename)
      : strm_(new buffered_stream(is))
      , filename_(filename)
      , line_(1)
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

    token parser::read_number(pos_t p)
    {
        using namespace tokenize;
        auto pstart = p;
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
            return token::get(token_type::INTEGER, strm_->buff() + pstart, digend - pstart);
        } else if (hasfracpart && !is_symbol_char(strm_->peek_at(fracend))) {
            return token::get(token_type::FLOATING, strm_->buff() + pstart, fracend - pstart);
        } else {
            return token::get(token_type::FAIL, "Error reading number");
        }
    }

    token parser::read_symbol(pos_t p)
    {
        using namespace tokenize;
        auto pstart = p;
        while (is_symbol_char(strm_->peek_at(p))) {
            ++p;
        }

        return (p != pstart) ? token::get(token_type::SYMBOL, strm_->buff() + pstart, p - pstart)
                             : token::get(token_type::FAIL, "Error reading symbol");
    }

    pos_t parser::curr_pos() const
    {
        return strm_->pos();
    }
    

    /// parser_engine
    class parser::parser_engine
    {
      public:
        parser_engine(parser& p, arena& a) : parser_(p), arena_(a), func_("(main)")
        { }

        ir::tree* parse()
        {
            return arena_.create<ir::tree>(block());
        }
      private:
        parser& parser_;
        arena&  arena_;
        std::string func_;

        int quit_with_error(name msg)
        {
            const auto& linestr = "line " + std::to_string(parser_.line());
            return pcsh::assert_fail(msg, parser_.filename_.c_str(), linestr.c_str(), func_.c_str());
        }

#define ENFORCE(x, msg)                       \
    do {                                      \
        !!(x) ? 0 : quit_with_error((msg));   \
    } while (0)

        ir::node* block()
        {
            std::vector<ir::node*> stmts;
            stmts.reserve(20);
            if (peek().is_a(token_type::LBRACE)) {
                advance();
                do {
                    stmts.push_back(stmt());
                } while (!peek().is_a(token_type::RBRACE));
            } else {
                do {
                    stmts.push_back(stmt());
                } while (!peek().is_a(token_type::EOS));
            }
            ir::block* blk = arena_.create<ir::block>();
            auto beg = stmts.rbegin();
            const auto end = stmts.rend();
            for (; beg != end; ++beg) {
                blk->push_front_statement(*beg, arena_);
            }
            return blk;
        }

        ir::node* stmt()
        {
            auto v = var();
            ENFORCE(peek().is_a(token_type::ASSIGN), "Expected assignment in a statement.");
            advance(); /* consume = */
            auto e = expr();
            ENFORCE(peek().is_a(token_type::SEMICOLON), "Expected a semicolon to end a statement.");
            advance(); /* consume ; */
            auto op = arena_.create<ir::assign>();
            op->set_left(v);
            op->set_right(e);
            return op;
        }

        ir::variable* var()
        {
            auto t = peek();
            ENFORCE(t.is_a(token_type::SYMBOL), "Variable must be a non keyword and alpha-numeric.");
            name nm = arena_.create_string(t.str().ptr, t.length());
            advance();
            return arena_.create<ir::variable>(nm);
        }

        ir::node* expr()
        {
            auto t = peek();
            if (t.is_a(token_type::LPAREN)) {
                advance();
                auto e = expr();
                ENFORCE(peek().is_a(token_type::RPAREN), "Unmatched '('.");
                return e;
            } else if (is_unary_op(t)) {
                return unop();
            } else if (is_binary_op(t)) {
                return binop();
            } else {
                return atom();
            }
        }

        ir::node* binop()
        {
            ir::binary_op* op = nullptr;
            auto nodea = atom();
            auto nxt = peek();
            while (is_binary_op(nxt)) {
                op = create_binary_op(nxt);
                advance();
                op->set_left(nodea);
                op->set_right(atom());
                nxt = peek();
                nodea = op;
            }
            return op;
        }

        ir::node* unop()
        {
            auto op = create_unary_op(peek());
            advance();
            op->set_operand(atom());
            return op;
        }

        ir::node* atom()
        {
            auto t = peek();
            ir::node* v = nullptr;
            switch (t.type()) {
                case token_type::SYMBOL:
                    v = arena_.create<ir::variable>(arena_.create_string(t.str().ptr, t.length()));
                    break;
                case token_type::INTEGER:
                    v = arena_.create<ir::int_constant>(conversions::to_int(t));
                    break;
                case token_type::FLOATING:
                    v = arena_.create<ir::float_constant>(conversions::to_double(t));
                    break;
                default:
                    PCSH_ASSERT_MSG(false, "Invalid atom value!");
                    break;
            }
            advance();
            return v;
        }

        // parser manipulation

        token peek()
        {
            pos_t ws = 0;
            auto t = parser_.peek(0, &ws);
            if (t.is_a(token_type::FAIL)) {
                // advance ws up to the failure
                parser_.advance(ws);
                auto errtok = parser_.peek();
                PCSH_ASSERT(errtok.is_a(token_type::FAIL));
                quit_with_error(errtok.str().ptr);
            }
            return t;
        }

        void advance()
        {
            pos_t ws = 0;
            auto t = parser_.peek(ws, &ws);
            parser_.advance(ws + t.length());
        }

        bool is_unary_op(const token& t)
        {
            return t.is_a(token_type::PLUS) || t.is_a(token_type::MINUS);
        }

        bool is_binary_op(const token& nxt)
        {
            switch (nxt.type()) {
                case token_type::PLUS:
                case token_type::MINUS:
                case token_type::ASTERISK:
                case token_type::FSLASH:
                case token_type::ASSIGN:
                    return true;
                default:
                    return false;
            }
        }

        ir::binary_op* create_binary_op(const token& nxt)
        {
            switch (nxt.type()) {
                case token_type::PLUS:
                    return arena_.create<ir::binary_plus>();
                case token_type::MINUS:
                    return arena_.create<ir::binary_minus>();
                case token_type::ASTERISK:
                    return arena_.create<ir::binary_mult>();
                case token_type::FSLASH:
                    return arena_.create<ir::binary_div>();
                case token_type::ASSIGN:
                    return arena_.create<ir::assign>();
                default:
                    PCSH_ASSERT_MSG(false, "Invalid binary operation!");
                    return nullptr;
            }
        }

        ir::unary_op* create_unary_op(const token& nxt)
        {
            switch (nxt.type()) {
                case token_type::PLUS:
                    return arena_.create<ir::unary_plus>();
                case token_type::MINUS:
                    return arena_.create<ir::unary_minus>();
                default:
                    PCSH_ASSERT_MSG(false, "Invalid binary operation!");
                    return nullptr;
            }
        }
    };

    ir::tree* parser::parse_to_tree(arena& a)
    {
        return parser_engine(*this, a).parse();
    }

}// namespace parser
}// namespace pcsh
