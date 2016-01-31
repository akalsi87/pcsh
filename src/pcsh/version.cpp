/**
 * \file version.cpp
 * \date Jan 27, 2016
 */

#include "pcsh/version.hpp"

#define _STRINGIFY_(x, y, z) #x "." #y "." #z

#define STRINGIFY(x, y, z) _STRINGIFY_(x, y, z)

namespace pcsh {
namespace version {

    int major()
    {
        return PCSH_MAJ;
    }

    int minor()
    {
        return PCSH_MIN;
    }

    int patch()
    {
        return PCSH_PAT;
    }

    const char* str()
    {
        return STRINGIFY(PCSH_MAJ, PCSH_MIN, PCSH_PAT);
    }

}// namespace version
}// namespace pcsh

