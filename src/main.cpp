/**
 * \file main.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/parser.hpp"

int main(int argc, const char* argv[])
{
    using namespace pcsh::parser;
    parser(std::cin).parse_to_tree()->print(std::cerr);
    return 0;
}
