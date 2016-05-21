/**
 * \file main.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"
#include "pcsh/ir.hpp"
#include "pcsh/ir_operations.hpp"
#include "pcsh/parser.hpp"

#include "linebufistream.hpp"

#include <fstream>

void die_usage(int e)
{
    std::cout << "pcsh [-h | filename]\n";
    exit(e);
}

void die_handling_exception()
{
    try {
        throw;
    } catch (const pcsh::parser::exception& ex) {
        pcsh::assert_fail(ex.message().c_str(), ex.filename().c_str(), ex.line().c_str(), ex.fcn().c_str());
    } catch (const std::bad_alloc&) {
        PCSH_ENFORCE_MSG(false, "Out of memory!");
    } catch (...) {
        PCSH_ENFORCE_MSG(false, "Unknown exception encountered.");
    }
}

void die_if_unable_to_open_file(std::ifstream& in, const std::string& fn)
{
    if (in.fail()) {
        auto msg = "Failed to open file `" + fn + "'.";
        PCSH_ENFORCE_MSG(!in.fail(), msg.c_str());
    }
}

void run(std::istream& in, pcsh::ostream& out)
{
    using namespace pcsh;

    ir::tree::ptr treep;
    try {
        treep = parser::parser(in).parse_to_tree();
    } catch(...) {
        die_handling_exception();
    }

    //ir::print(treep.get(), out);

    try {
        ir::evaluate(treep.get());
    } catch (...) {
        die_handling_exception();
    }

    out << "\n--- Evaluated results ---\n";
    ir::print_variables(treep.get(), out);
}

int main(int argc, const char* argv[])
{
    if (argc == 1) {
        pcsh::linebuff_istream in(std::cin);
        auto& out = std::cout;
        run(in, out);
    } else if (argc == 2) {
        if (::strcmp(argv[1], "-h") == 0) {
            die_usage(0);
        }
        std::ifstream fs(argv[1], std::ios_base::in | std::ios_base::binary);
        die_if_unable_to_open_file(fs, argv[1]);
        pcsh::linebuff_istream in(fs);
        auto& out = std::cout;
        run(in, out);
    } else {
        die_usage(1);
    }

    return 0;
}
