/**
 * \file tree_validation.cpp
 * \date Jan 31, 2016
 */

#include "pcsh/assert.hpp"

#include "ast/passes/populate_symbol_table.hpp"
#include "ast/passes/type_checker.hpp"
#include "ast/tree_validation.hpp"

namespace pcsh {

    void validate_tree(const ast::tree::ptr& p)
    {
        // populate symbol tables
        {
            ast::populate_symbol_table populater;
            p->accept(&populater);
        }
        // type check
        {
            ast::type_checker checker;
            p->accept(&checker);
        }
    }

}//namespace pcsh
