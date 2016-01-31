/**
 * \file tpcshver.cpp
 * \date Jan 27, 2016
 */

#include "unittest.hpp"
#include <cstring>

#include "pcsh/version.hpp"

CPP_TEST( version )
{
    TEST_TRUE(pcsh::version_major() == 0);
    TEST_TRUE(pcsh::version_minor() == 0);
    TEST_TRUE(pcsh::version_patch() == 1);
    TEST_TRUE(strcmp(pcsh::version_str(), "0.0.1") == 0);
}
