/**
 * \file main.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/ir.hpp"
#include "pcsh/ir_operations.hpp"
#include "pcsh/parser.hpp"

#include "linebufistream.hpp"

void die_handling_exception()
{
    try {
        throw;
    } catch (const pcsh::parser::exception& ex) {
        pcsh::assert_fail(ex.message().c_str(), ex.filename().c_str(), ex.line().c_str(), ex.function().c_str());
    } catch (const std::bad_alloc&) {
        PCSH_ENFORCE_MSG(false, "Out of memory!");
    } catch (...) {
        PCSH_ENFORCE_MSG(false, "Unknown exception encountered.");
    }
}

int main(int argc, const char* argv[])
{
    using namespace pcsh;

    linebuff_istream in(std::cin);
    auto& out = std::cout;

    ir::tree::ptr treep;
    try {
        treep = parser::parser(in).parse_to_tree();
    } catch(...) {
        die_handling_exception();
    }

    out << "\n";
    ir::print(treep.get(), out);

    out << "--- Evaluation\n";
    try {
        ir::evaluate(treep.get());
    } catch (...) {
        die_handling_exception();
    }

    ir::print_variables(treep.get(), out);

    return 0;
}
