/**
 * \file main.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/parser.hpp"

int main(int argc, const char* argv[])
{
    using namespace pcsh::parser;
    auto& in = std::cin;
    auto& out = std::cerr;
    auto t1 = parser(in).parse_to_tree();
    t1->print(out);
    out << "\n --- Evaluation --- \n";
    t1->evaluate()->print(out, false);
    return 0;
}
