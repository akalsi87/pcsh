/**
 * \file version.cpp
 * \date Jan 27, 2016
 */

#include "pcsh/version.hpp"

#define _STRINGIFY_(x, y, z) #x "." #y "." #z

#define STRINGIFY(x, y, z) _STRINGIFY_(x, y, z)

namespace pcsh {

    int version_major()
    {
        return PCSH_MAJ;
    }

    int version_minor()
    {
        return PCSH_MIN;
    }

    int version_patch()
    {
        return PCSH_PAT;
    }

    const char* version_str()
    {
        return STRINGIFY(PCSH_MAJ, PCSH_MIN, PCSH_PAT);
    }

}// namespace pcsh

