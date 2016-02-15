/**
 * \file ir_operations.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/ir.hpp"
#include "pcsh/ir_operations.hpp"

#include "ir_evaluator.hpp"
#include "ir_nodes.hpp"
#include "ir_printer.hpp"
#include "ir_variable_printer.hpp"
#include "symbol_table.hpp"
#include "tree_cloner.hpp"

namespace pcsh {
namespace ir {

    void print(const tree* ptree, ostream& os, bool types)
    {
        printer p(os, types);
        ptree->accept(&p);
    }

    void print_variables(const tree* ptree, ostream& os)
    {
        var_value_printer p(os);
        ptree->accept(&p);
    }

    tree::ptr clone(const tree* ptree)
    {
        tree_cloner c;
        ptree->accept(&c);
        return std::move(c.cloned_tree());
    }

    void evaluate(const tree* ptree)
    {
        evaluator e;
        ptree->accept(&e);
    }

    var_value query(const tree* ptree, cstring name)
    {
        struct sym_list_extractor : public node_visitor
        {
            sym_table_list tablist;
          private:
            void visit_impl(const block* v) override
            {
                tablist.push_back(&(v->table()));
                visit_block(v);
            }
        };

        sym_list_extractor xtrac;
        ptree->accept(&xtrac);
        variable_accessor acc(xtrac.tablist);
        variable v(name);
        auto res = acc.lookup(&v);

        var_value rv;
        rv.type = result_type::FAILED;
        rv.int_val = 0;
        if (res.ptr && res.evaluated) {
            rv.type = res.type;
            switch (res.type) {
                case result_type::INTEGER:
                    rv.int_val = static_cast<int_constant*>(res.ptr)->value();
                    break;
                case result_type::FLOATING:
                    rv.dbl_val = static_cast<float_constant*>(res.ptr)->value();
                    break;
                case result_type::STRING:
                    rv.str_val = static_cast<string_constant*>(res.ptr)->value();
                    break;
                default:
                    rv.type = result_type::FAILED;
                    break;
            }
        }
        return rv;
    }

}// namespace ir
}// namespace pcsh
