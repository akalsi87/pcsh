/**
 * \file result_type.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_RESULT_TYPE_HPP
#define PCSH_RESULT_TYPE_HPP

#include "pcsh/types.hpp"

namespace pcsh {

    enum class result_type : byte
    {
        BOOLEAN = 0,
        INTEGER,
        FLOATING,
        STRING,
        UNDETERMINED,
        FAILED
    };

    inline cstring to_string(result_type t)
    {
        switch (t) {
            case pcsh::result_type::BOOLEAN:
                return "boolean";
            case pcsh::result_type::INTEGER:
                return "int";
            case pcsh::result_type::FLOATING:
                return "double";
            case pcsh::result_type::STRING:
                return "string";
            case pcsh::result_type::UNDETERMINED:
                return "unknown";
            case pcsh::result_type::FAILED:
                return "FAILED";
            default:
                return "(unknown result_type)";
        }
    }

}//namespace pcsh

#endif/*PCSH_RESULT_TYPE_HPP*/
