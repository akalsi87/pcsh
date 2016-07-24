/**
 * \file types.hpp
 * \date Jan 29, 2016
 */

#ifndef PCSH_TYPES_HPP
#define PCSH_TYPES_HPP

#include <cstddef>

namespace pcsh {

    typedef const char* cstring;

    typedef unsigned char byte;

namespace ast {

    using int_type = int64_t;
    using float_type = double;
    using string_type = cstring;

}//namespace ast
}//namespace pcsh

#endif/*PCSH_TYPES_HPP*/
