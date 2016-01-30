/**
 * \file parser.hpp
 * \date Jan 27, 2016
 */

#ifndef PCSH_PARSER_HPP
#define PCSH_PARSER_HPP

#include "pcsh/exportsym.h"
#include "pcsh/types.hpp"

namespace pcsh {
namespace parser {

///
// Based on Bish
// http://www.github.com/tdenniston/bish
//

/*
Grammar:

module ::= block
block ::= '{' { stmt } '}'
stmt ::= assign ';'
| funcall ';'
| externcall ';'
| 'import' any ';'
| 'return' expr ';'
| 'break' ';'
| 'continue' ';'
| '#' any NEWLINE
| 'if' '(' expr ')' block
| 'if' '(' expr ')' block { 'else' 'if' '(' expr ')' block } 'else' block
| 'for' '(' var 'in' atom [ '..' atom ] ')' block
| 'def' var '(' varlist ')' block
| block
assign ::= location '=' expr
| location '=' '[' exprlist ']'
funcall ::= namespacedvar '(' exprlist ')'
externcall ::= '@' '(' interp ')'
expr ::= expr '|' logical | logical
logical ::= logical 'and' equality | logical 'or' equality | equality
equality ::= equality '==' relative | equality '!=' relative | relative
relative ::= relative '<' arith | relative '>' arith
| relative '<=' arith | relative '>=' arith
| arith
arith ::= arith '+' term | arith '-' term | term
term ::= term '*' unary | term '/' unary | term '%' unary | unary
unary ::= '-' unary | 'not' unary | factor
factor ::= '( expr ')' | funcall | externcall | atom
atom ::= location | NUMBER | '"' STRING '"' | 'true' | 'false'
var ::= { ALPHANUM | '_' }
location ::= namespacedvar | namespacedvar '[' expr ']'
namespacedvar ::= [ var '.' ] var
varlist ::= var { ',' var }
exprlist ::= expr { ',' expr }
interp ::= { str | '$' namespacedvar | '$' '(' any ')'}
*/

    enum class token_type : byte
    {
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        SEMICOLON,
        ASSIGN,
        PLUS,
        MINUS,
        ASTERISK,
        FSLASH,
        SYMBOL,
        INTEGER,
        FLOATING,
        EOS,
        NONE
    };

    class PCSH_API token
    {
      public:
        static token get(token_type t, name nm = nullptr);

        token(const token& rhs) : type_(rhs.type_), str_(rhs.str_)
        { }

      private:
        token(token_type t, name s)  : type_(t), str_(s)
        { }

        token_type type_;
        name str_;
    };

}// namespace parser
}// namespace pcsh

#endif/*PCSH_PARSER_HPP*/
