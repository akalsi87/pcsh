/**
 * \file tparser.cpp
 * \date Jan 30, 2016
 */

#include "unittest.hpp"

#include "pcsh/ir.hpp"
#include "pcsh/ir_operations.hpp"
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
    TEST_TRUE(p.curr_pos() == (size_t)is.tellg());
}

CPP_TEST( tokenizerCommentsAndLinesCascade )
{
    using namespace pcsh::parser;
    std::istringstream is(
        "#!/usr/bin/env pcsh\n"
        "foo = 1;## test if double comment works\n"
        "bar = -121;\n"
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
        TEST_TRUE(two.is_a(token_type::MINUS));
        curr += two.length();
    }
    {
        auto two = p.peek(curr, &curr);
        TEST_TRUE(two.is_a(token_type::INTEGER));
        TEST_TRUE(two.str().equals("121"));
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
    TEST_TRUE(curr == (size_t)is.tellg());
}

CPP_TEST( irCreationBasic )
{
    using namespace pcsh;
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = 1;## test if double comment works\n"
            "bar = 2;\n"
            "#{ y = 1; } { z = 2; }\n"
            "213 + 456123.123; # cannot handle this line yet as it is not a statement\n"
            "#{ a.b } - cannot handle this line yet as it is unsupported\n");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "bar").int_val == 2);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "{\n"
            "    a = 1;\n"
            "    b = -42;\n"
            "    c = a + b;\n"
            "}\n");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "a").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "b").int_val == -42);
        TEST_TRUE(ir::query(ptree.get(), "c").int_val == -41);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "\n"
            "    foo = 1;\n"
            "    doo = -1.0 + foo;\n"
            "");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "doo").dbl_val == 0.0);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = 1;\n"
            "{\n"
            "    doo = -1.0 + foo;\n"
            "}");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "doo").dbl_val == 0.0);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = 1;\n"
            "{\n"
            "    doo = \"a literal string.\";\n"
            "}");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "doo").str_val == std::string("a literal string."));
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = 1;\n"
            "{\n"
            "    doo = \"a\n\";\n"
            "}");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").int_val == 1);
        TEST_TRUE(ir::query(ptree.get(), "doo").str_val == std::string("a\n"));
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "{\n"
            "    doo = -1.0f;\n"
            "}");
        parser::parser p(is);
        bool shouldBeTrue = false;
        try {
            p.parse_to_tree();
        } catch (const parser::exception&) {
            shouldBeTrue = true;
        }
        TEST_TRUE(shouldBeTrue);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "{\n"
            "    doo = -1.0;\n"
            "    doo = \"foo\";\n"
            "}");
        parser::parser p(is);
        bool shouldBeTrue = false;
        try {
            p.parse_to_tree();
        } catch (const parser::exception& ex) {
            shouldBeTrue = (ex.message().find("Type") != std::string::npos);
        }
        TEST_TRUE(shouldBeTrue);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = 1 + -9 + +9 * 71.5;\n");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").dbl_val == 635.5);
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "a = 1;\n"
            "foo = \"asd\";\n"
            "if (foo) foo = \"bar\";\n"
            "{\n"
            "    if (a) {\n"
            "        a = -1*a;\n"
            "    }\n"
            "}\n");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "a").int_val == -1);
        TEST_TRUE(ir::query(ptree.get(), "foo").str_val == std::string("bar"));
    }
    {
        std::istringstream is(
            "#!/usr/bin/env pcsh\n"
            "foo = bar = (1 + (car = 20.0));\n");
        parser::parser p(is);
        auto ptree = p.parse_to_tree();
        ir::print(ptree.get(), std::cout);
        ir::evaluate(ptree.get());
        TEST_TRUE(ir::query(ptree.get(), "foo").dbl_val == 21.0);
        TEST_TRUE(ir::query(ptree.get(), "bar").dbl_val == 21.0);
        TEST_TRUE(ir::query(ptree.get(), "car").dbl_val == 20.0);
    }
}
