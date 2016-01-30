/**
 * \file assert.hpp
 * \date Jan 29, 2016
 */

#ifndef PCSH_ASSERT_HPP
#define PCSH_ASSERT_HPP

#include "pcsh/exportsym.h"

namespace pcsh {

    PCSH_API int assert_fail(const char* msg, const char* file, const char* line, const char* func);

}//namespace pcsh

namespace pcsh {
namespace detail {

    inline void __useless()
    {
#if !defined(FUNCTION_NAME)
#  if defined(__GNUC__)
#    define FUNCTION_NAME __PRETTY_FUNCTION__
#  elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#    define FUNCTION_NAME __func__
#  else
#    define FUNCTION_NAME "(unknown function)"
#  endif
#endif
    }

}//namespace detail
}//namespace pcsh

#define __STRINGIFY_2(x) #x
#define STRINGIFY(x) __STRINGIFY_2(x)

#if !defined(NDEBUG)//debug enabled

#define PCSH_ASSERT(x)                                                                        \
    do {                                                                                      \
        (!!(x)) ? 0 : pcsh::assert_fail(#x, __FILE__, STRINGIFY(__LINE__), FUNCTION_NAME);    \
    } while (0)

#define PCSH_ASSERT_MSG(x, msg)                                                               \
    do {                                                                                      \
        (!!(x)) ? 0 : pcsh::assert_fail((msg), __FILE__, STRINGIFY(__LINE__), FUNCTION_NAME); \
    } while (0)

#else//no debugging

#define PCSH_ASSERT(x)                                                                        \
    static_cast<void>(0)

#define PCSH_ASSERT_MSG(x, msg)                                                               \
    static_cast<void>(0)


#endif//!defined(NDEBUG)

#endif/*PCSH_ASSERT_HPP*/
