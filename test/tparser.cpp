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
        auto symfoo = p.peek();
        TEST_TRUE(symfoo.is_a(token_type::SYMBOL));
        TEST_TRUE(symfoo.str().equals("foo"));
        p.advance(symfoo);
    }
    {
        auto asgn = p.peek();
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        p.advance(asgn);
    }
    {
        auto one = p.peek();
        TEST_TRUE(one.is_a(token_type::INTEGER));
        TEST_TRUE(one.str().equals("1"));
        p.advance(one);
    }
    {
        auto sc = p.peek();
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(sc);
    }
    {
        auto symbar = p.peek();
        TEST_TRUE(symbar.is_a(token_type::SYMBOL));
        TEST_TRUE(symbar.str().equals("bar"));
        p.advance(symbar);
    }
    {
        auto asgn = p.peek();
        TEST_TRUE(asgn.is_a(token_type::ASSIGN));
        p.advance(asgn);
    }
    {
        auto two = p.peek();
        TEST_TRUE(two.is_a(token_type::INTEGER));
        TEST_TRUE(two.str().equals("2"));
        p.advance(two);
    }
    {
        auto sc = p.peek();
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(sc);
    }
    {
        auto c1 = p.peek();
        TEST_TRUE(c1.is_a(token_type::INTEGER));
        TEST_TRUE(c1.str().equals("213"));
        p.advance(c1);
    }
    {
        auto plus = p.peek();
        TEST_TRUE(plus.is_a(token_type::PLUS));
        p.advance(plus);
    }
    {
        auto c2 = p.peek();
        TEST_TRUE(c2.is_a(token_type::FLOATING));
        TEST_TRUE(c2.str().equals("456123.123"));
        p.advance(c2);
    }
    {
        auto sc = p.peek();
        TEST_TRUE(sc.is_a(token_type::SEMICOLON));
        TEST_TRUE(sc.str().equals(";"));
        p.advance(sc);
    }
    {
        auto lb = p.peek();
        TEST_TRUE(lb.is_a(token_type::LBRACE));
        p.advance(lb);
    }
    {
        auto syma = p.peek();
        TEST_TRUE(syma.is_a(token_type::SYMBOL));
        TEST_TRUE(syma.str().equals("a"));
        p.advance(syma);
    }
    {
        auto dot = p.peek();
        TEST_TRUE(dot.is_a(token_type::DOT));
        p.advance(dot);
    }
    {
        auto symb = p.peek();
        TEST_TRUE(symb.is_a(token_type::SYMBOL));
        TEST_TRUE(symb.str().equals("b"));
        p.advance(symb);
    }
    {
        auto rb = p.peek();
        TEST_TRUE(rb.is_a(token_type::RBRACE));
        p.advance(rb);
    }
    {
        auto eos = p.peek();
        TEST_TRUE(eos.is_a(token_type::EOS));
    }
}
