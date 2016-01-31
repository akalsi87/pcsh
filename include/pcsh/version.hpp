/**
 * \file version.hpp
 * \date Jan 27, 2016
 */

#ifndef PCSH_VERSION_HPP
#define PCSH_VERSION_HPP

// export includes
#include "pcsh/exportsym.h"

namespace pcsh {
namespace version {

    PCSH_API int major();

    PCSH_API int minor();

    PCSH_API int patch();

    PCSH_API const char* str();

}// namespace version
}// namespace pcsh

#endif/*PCSH_VERSION_HPP*/
