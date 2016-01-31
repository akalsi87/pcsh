/**
 * \file version.hpp
 * \date Jan 27, 2016
 */

#ifndef PCSH_VERSION_HPP
#define PCSH_VERSION_HPP

// export includes
#include "pcsh/exportsym.h"

namespace pcsh {

    PCSH_API int version_major();

    PCSH_API int version_minor();

    PCSH_API int version_patch();

    PCSH_API const char* version_str();

}// namespace pcsh

#endif/*PCSH_VERSION_HPP*/
