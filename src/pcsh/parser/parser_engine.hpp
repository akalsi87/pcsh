/**
 * \file parser_engine.hpp
 * \date Feb 15, 2016
 */

#ifndef PCSH_PARSER_ENGINE_HPP
#define PCSH_PARSER_ENGINE_HPP

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include "ast/nodes_fwd.hpp"

#include <string>
#include <unordered_map>

namespace pcsh {
namespace parser {

    /// map node to source info
    struct source_info
    {
        std::string filename;
        std::string line;
        std::string fcn;
    };

    using source_map = std::unordered_map<ast::node*, source_info>;

    /// parser_engine
    class parser::parser_engine
    {
      public:
        parser_engine(parser& p, arena& a)
          : parser_(p), arena_(a)
          , curr_(token::get(token_type::EOS, "\xFF", 1))
          , ws_(0)
          , func_("(main)")
          , parsed_(false)
        { }

        //
        // throws exceptions if parse fails.
        //
        inline ast::block* parse(source_map& m)
        {
            return block(m);
        }
      private:
        parser& parser_;
        arena&  arena_;
        token curr_;
        pos_t ws_;
        std::string func_;
        bool parsed_;

        int throw_error(cstring msg);

        ast::block* block(source_map& m);

        ast::node* stmt(source_map& m);

        ast::node* factor(source_map& m);

        ast::node* term(source_map& m);

        ast::node* expr(source_map& m);

        ast::node* arith(source_map& m);

        ast::node* unop(source_map& m);

        ast::node* atom(source_map& m);

        ast::node* ifstmt(source_map& m);

        // parser manipulation

        token peek();

        void do_peek();

        void advance();

        static bool is_unary_op(const token& t);

        static bool is_binary_op(const token& nxt);

        typedef ast::node* (parser_engine::*nexttermfcn)(source_map&);

        ast::node* call_mem_fn(parser_engine* p, nexttermfcn pfn, source_map& m);

        ast::untyped_binary_op_base* create_binary_op(const token& currtok, ast::node* a, source_map& m, nexttermfcn rghtgen);

        ast::untyped_unary_op_base* create_unary_op(const token& nxt, source_map& m);
    };

}//namespace parser
}//namespace pcsh

#endif/*PCSH_PARSER_ENGINE_HPP*/
