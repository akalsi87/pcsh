/**
 * \file tree_validation.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"

#include "populate_symbol_table.hpp"
#include "tree_validation.hpp"
#include "type_checker.hpp"

namespace pcsh {

    void validate_tree(const ir::tree::ptr& p)
    {
        // populate symbol tables
        {
            ir::populate_symbol_table populater;
            p->accept(&populater);
        }
        // type check
        {
            ir::type_checker checker;
            p->accept(&checker);
        }
    }

}//namespace pcsh
