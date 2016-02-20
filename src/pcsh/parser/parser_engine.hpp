/**
 * \file parser_engine.hpp
 * \date Feb 15, 2016
 */

#ifndef PCSH_PARSER_ENGINE_HPP
#define PCSH_PARSER_ENGINE_HPP

#include "pcsh/assert.hpp"
#include "pcsh/parser.hpp"

#include "ir/nodes_fwd.hpp"

#include <string>
#include <unordered_map>

#if !defined(PCSH_INLINE)
#  if !defined(_MSC_VER)
#    define PCSH_INLINE __inline__ __attribute__((always_inline))
#  else
#    define PCSH_INLINE __forceinline
#  endif
#endif

#if !defined(_MSC_VER)
#  define PCSH_UNLIKELY(x) __builtin_expect(!!(x), 0)
#  define PCSH_LIKELY(x)   __builtin_expect(!!(x), 1)
#else
#  define PCSH_UNLIKELY(x) (x)
#  define PCSH_LIKELY(x)   (x)
#endif

namespace pcsh {
namespace parser {

    /// map node to source info
    struct source_info
    {
        std::string filename;
        std::string line;
        std::string fcn;
    };

    using source_map = std::unordered_map<ir::node*, source_info>;

    /// parser_engine
    class parser::parser_engine
    {
      public:
        parser_engine(parser& p, arena& a)
          : parser_(p), arena_(a)
          , func_("(main)")
          , parsed_(false)
          , ws_(0)
          , curr_(token::get(token_type::EOS, "\xFF", 1))
        { }

        //
        // throws exceptions if parse fails.
        //
        inline ir::block* parse(source_map& m)
        {
            return block(m);
        }
      private:
        parser& parser_;
        arena&  arena_;
        std::string func_;
        bool parsed_;
        pos_t ws_;
        token curr_;

        int throw_error(cstring msg);

        ir::block* block(source_map& m);

        ir::node* stmt(source_map& m);

        ir::node* factor(source_map& m);

        ir::node* term(source_map& m);

        ir::node* expr(source_map& m);

        ir::node* arith(source_map& m);

        ir::node* unop(source_map& m);

        ir::node* atom(source_map& m);

        ir::node* ifstmt(source_map& m);

        // parser manipulation

        token peek();

        void do_peek();

        void advance();

        static bool is_unary_op(const token& t);

        static bool is_binary_op(const token& nxt);

        typedef ir::node* (parser_engine::*nexttermfcn)(source_map&);

        ir::node* call_mem_fn(parser_engine* p, nexttermfcn pfn, source_map& m);

        ir::untyped_binary_op_base* create_binary_op(const token& currtok, ir::node* a, source_map& m, nexttermfcn rghtgen);

        ir::untyped_unary_op_base* create_unary_op(const token& nxt, source_map& m);
    };

}//namespace parser
}//namespace pcsh

#endif/*PCSH_PARSER_ENGINE_HPP*/
