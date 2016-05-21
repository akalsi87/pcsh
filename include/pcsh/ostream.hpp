/**
 * \file ostream.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_OSTREAM_HPP
#define PCSH_OSTREAM_HPP

#include <ostream>
#include <iostream>

namespace pcsh {

    using ostream = std::ostream;

    static ostream& DEFAULT_OUTPUT_STREAM = std::cout;

}//namespace pcsh

#endif/*PCSH_OSTREAM_HPP*/
