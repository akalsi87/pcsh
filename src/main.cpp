/**
 * \file main.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include "linebufistream.hpp"

int main(int argc, const char* argv[])
{
    using namespace pcsh;

    auto& in = linebuff_istream(std::cin);
    auto& out = std::cout;

    ir::tree::ptr t1;
    try {
        t1 = parser::parser(in).parse_to_tree();
    } catch(const parser::exception& ex) {
        assert_fail(ex.message().c_str(), ex.filename().c_str(), ex.line().c_str(), ex.function().c_str());
    } catch(const std::bad_alloc&) {
        PCSH_ENFORCE_MSG(false, "Out of memory!");
    } catch(...) {
        PCSH_ENFORCE_MSG(false, "Unknown exception encountered.");
    }
    t1->print(out);
    out << "\n --- Evaluation --- \n";
    // t1->clone()->print(out);
    t1->evaluate().print_variables(out);
    return 0;
}
