/*! \file exportsym.h */
/* Export symbol definitions */

#if !defined(PCSH_EXPORTSYM_H)
#define PCSH_EXPORTSYM_H

#if defined(PCSH_LINK_STATIC)
/*! Macro to define library export symbol */
#  define PCSH_API 
#elif defined(PCSH_BUILD)
#  if defined(_MSC_VER)
/*! Macro to define library export symbol */
#    define PCSH_API __declspec(dllexport)
#  else/*GCC-like compiler*/
/*! Macro to define library export symbol */
#    define PCSH_API __attribute__((__visibility__("default")))
#  endif
#else/* import symbol */
#  if defined(_MSC_VER)
/*! Macro to define library export symbol */
#    define PCSH_API __declspec(dllimport)
#  else/*GCC-like compiler*/
/*! Macro to define library export symbol */
#    define PCSH_API 
#  endif
#endif/*defined(PCSH_LINK_STATIC)*/

/*! Allow deprecation? */
#define PCSH_ALLOW_DEPRECATION 1

#if PCSH_ALLOW_DEPRECATION
/*! Deprecated macro */
#  if defined(_MSC_VER)
#    define PCSH_DEPRECATED(x) __declspec(deprecated(x))
#  else/*GCC-like compiler*/
#    define PCSH_DEPRECATED(x) __attribute__((deprecated(x)))
#  endif/*defined(_MSC_VER)*/
#else/* render macro useless */
#  define PCSH_DEPRECATED(x) 
#endif/*!PCSH_ALLOW_DEPRECATION*/

#endif/*!defined(PCSH_EXPORTSYM_H)*/
