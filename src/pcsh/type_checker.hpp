/**
 * \file type_checker.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_TYPE_CHECKER_HPP
#define PCSH_TYPE_CHECKER_HPP

#include "ir_visitor.hpp"

#include "result_type.hpp"

namespace pcsh {
namespace ir {

    struct type_checker_error
    {
        std::string msg;
        result_type left;
        result_type right;

        type_checker_error(const std::string& m, result_type a, result_type b)
          : msg(m)
          , left(a)
          , right(b)
        { }
    };

    class type_checker : public node_visitor
    {
      public:
        type_checker() : curr_(result_type::UNDETERMINED), curr_blk_(nullptr)
        { }
      private:
        result_type curr_;
        const block* curr_blk_;

        void visit_impl(const int_constant* v) override;
        void visit_impl(const float_constant* v) override;
        void visit_impl(const string_constant* v) override;
        void visit_impl(const unary_minus* v) override;
        void visit_impl(const binary_div* v) override;
        void visit_impl(const binary_minus* v) override;
        void visit_impl(const binary_mult* v) override;
        void visit_impl(const binary_plus* v) override;
        void visit_impl(const assign* v) override;
        void visit_impl(const block* v) override;
    };

}//namespace ir
}//namespace pcsh

#endif/*PCSH_TYPE_CHECKER_HPP*/
