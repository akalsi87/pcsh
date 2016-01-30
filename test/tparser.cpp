/**
 * \file tparser.cpp
 * \date Jan 30, 2016
 */

#include "unittest.hpp"

#include "pcsh/parser.hpp"

#include <sstream>

CPP_TEST( tokenizerCommentsAndLines )
{
    using namespace pcsh::parser;
    std::istringstream is(
        "#/usr/bin/env pcsh\n"
        "foo = 1; # comment after tokens\n"
        "bar = 2;\n"
        "213 + 456123.123;\n"
        "{ a.b }\n");
    parser p(is);
    {
        pos_t curr = 0;
        auto symfoo = p.peek(curr, &curr);
        TEST_TRUE(symfoo.is_a(token_type::SYMBOL));
        TEST_TRUE(symfoo.str().equals("foo"));
        p.advance(curr + symfoo.length());
    }
    {
        pos_t curr = 0;
        auto asgn = p.peek(curr, &curr);
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        p.advance(curr + asgn.length());
    }
    {
        pos_t curr = 0;
        auto one = p.peek(curr, &curr);
        TEST_TRUE(one.is_a(token_type::INTEGER));
        TEST_TRUE(one.str().equals("1"));
        p.advance(curr + one.length());
    }
    {
        pos_t curr = 0;
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(curr + sc.length());
    }
    {
        pos_t curr = 0;
        auto symbar = p.peek(curr, &curr);
        TEST_TRUE(symbar.is_a(token_type::SYMBOL));
        TEST_TRUE(symbar.str().equals("bar"));
        p.advance(curr + symbar.length());
    }
    {
        pos_t curr = 0;
        auto asgn = p.peek(curr, &curr);
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        p.advance(curr + asgn.length());
    }
    {
        pos_t curr = 0;
        auto two = p.peek(curr, &curr);
        TEST_TRUE(two.is_a(token_type::INTEGER));
        TEST_TRUE(two.str().equals("2"));
        p.advance(curr + two.length());
    }
    {
        pos_t curr = 0;
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(curr + sc.length());
    }
    {
        pos_t curr = 0;
        auto c1 = p.peek(curr, &curr);
        TEST_TRUE(c1.is_a(token_type::INTEGER));
        TEST_TRUE(c1.str().equals("213"));
        p.advance(curr + c1.length());
    }
    {
        pos_t curr = 0;
        auto plus = p.peek(curr, &curr);
        TEST_TRUE(plus.is_a(token_type::PLUS));
        p.advance(curr + plus.length());
    }
    {
        pos_t curr = 0;
        auto c2 = p.peek(curr, &curr);
        TEST_TRUE(c2.is_a(token_type::FLOATING));
        TEST_TRUE(c2.str().equals("456123.123"));
        p.advance(curr + c2.length());
    }
    {
        pos_t curr = 0;
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(curr + sc.length());
    }
    {
        pos_t curr = 0;
        auto lb = p.peek(curr, &curr);
        TEST_TRUE(lb.is_a(token_type::LBRACE));
        p.advance(curr + lb.length());
    }
    {
        pos_t curr = 0;
        auto syma = p.peek(curr, &curr);
        TEST_TRUE(syma.is_a(token_type::SYMBOL));
        TEST_TRUE(syma.str().equals("a"));
        p.advance(curr + syma.length());
    }
    {
        pos_t curr = 0;
        auto dot = p.peek(curr, &curr);
        TEST_TRUE(dot.is_a(token_type::DOT));
        p.advance(curr + dot.length());
    }
    {
        pos_t curr = 0;
        auto symb = p.peek(curr, &curr);
        TEST_TRUE(symb.is_a(token_type::SYMBOL));
        TEST_TRUE(symb.str().equals("b"));
        p.advance(curr + symb.length());
    }
    {
        pos_t curr = 0;
        auto rb = p.peek(curr, &curr);
        TEST_TRUE(rb.is_a(token_type::RBRACE));
        p.advance(curr + rb.length());
    }
    {
        pos_t curr = 0;
        auto eos = p.peek(curr, &curr);
        TEST_TRUE(eos.is_a(token_type::EOS));
        p.advance(curr + eos.length());
    }
    TEST_TRUE(p.curr_pos() == is.tellg());
}

CPP_TEST( tokenizerCommentsAndLinesCascade )
{
    using namespace pcsh::parser;
    std::istringstream is(
        "#/usr/bin/env pcsh\n"
        "foo = 1; # comment after tokens\n"
        "bar = 2;\n"
        "213 + 456123.123;\n"
        "{ a.b }\n");
    parser p(is);
    pos_t curr = 0;
    {
        auto symfoo = p.peek(curr, &curr);
        TEST_TRUE(symfoo.is_a(token_type::SYMBOL));
        TEST_TRUE(symfoo.str().equals("foo"));
        curr += symfoo.length();
    }
    {
        auto asgn = p.peek(curr, &curr);
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        curr += asgn.length();
    }
    {
        auto one = p.peek(curr, &curr);
        TEST_TRUE(one.is_a(token_type::INTEGER));
        TEST_TRUE(one.str().equals("1"));
        curr += one.length();
    }
    {
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        curr += sc.length();
    }
    {
        auto symbar = p.peek(curr, &curr);
        TEST_TRUE(symbar.is_a(token_type::SYMBOL));
        TEST_TRUE(symbar.str().equals("bar"));
        curr += symbar.length();
    }
    {
        auto asgn = p.peek(curr, &curr);
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        curr += asgn.length();
    }
    {
        auto two = p.peek(curr, &curr);
        TEST_TRUE(two.is_a(token_type::INTEGER));
        TEST_TRUE(two.str().equals("2"));
        curr += two.length();
    }
    {
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        curr += sc.length();
    }
    {
        auto c1 = p.peek(curr, &curr);
        TEST_TRUE(c1.is_a(token_type::INTEGER));
        TEST_TRUE(c1.str().equals("213"));
        curr += c1.length();
    }
    {
        auto plus = p.peek(curr, &curr);
        TEST_TRUE(plus.is_a(token_type::PLUS));
        curr += plus.length();
    }
    {
        auto c2 = p.peek(curr, &curr);
        TEST_TRUE(c2.is_a(token_type::FLOATING));
        TEST_TRUE(c2.str().equals("456123.123"));
        curr += c2.length();
    }
    {
        auto sc = p.peek(curr, &curr);
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        curr += sc.length();
    }
    {
        auto lb = p.peek(curr, &curr);
        TEST_TRUE(lb.is_a(token_type::LBRACE));
        curr += lb.length();
    }
    {
        auto syma = p.peek(curr, &curr);
        TEST_TRUE(syma.is_a(token_type::SYMBOL));
        TEST_TRUE(syma.str().equals("a"));
        curr += syma.length();
    }
    {
        auto dot = p.peek(curr, &curr);
        TEST_TRUE(dot.is_a(token_type::DOT));
        curr += dot.length();
    }
    {
        auto symb = p.peek(curr, &curr);
        TEST_TRUE(symb.is_a(token_type::SYMBOL));
        TEST_TRUE(symb.str().equals("b"));
        curr += symb.length();
    }
    {
        auto rb = p.peek(curr, &curr);
        TEST_TRUE(rb.is_a(token_type::RBRACE));
        curr += rb.length();
    }
    {
        auto eos = p.peek(curr, &curr);
        TEST_TRUE(eos.is_a(token_type::EOS));
    }
    TEST_TRUE(curr == is.tellg());
}
