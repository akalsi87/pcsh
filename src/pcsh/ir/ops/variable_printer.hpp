/**
 * \file variable_printer.hpp
 * \date Feb 01, 2016
 */

#ifndef PCSH_IR_VARIABLE_PRINTER_HPP
#define PCSH_IR_VARIABLE_PRINTER_HPP

#include "ir/ops/printer.hpp"
#include "ir/symbol_table.hpp"
#include "ir/visitor.hpp"

namespace pcsh {
namespace ir {

    class var_value_printer final : public node_visitor
    {
      public:
        var_value_printer(ostream& os) : strm_(os), nesting_(0), tbl_(nullptr), prn_(nullptr)
        { }

        ~var_value_printer()
        {
            if (prn_) {
                delete prn_;
            }
        }
      private:
        ostream& strm_;
        int nesting_;
        const symbol_table::ptr* tbl_;
        node_visitor* prn_;

        void visit_impl(const block* v) override;

        void print_spacing() const;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_IR_VARIABLE_PRINTER_HPP*/
