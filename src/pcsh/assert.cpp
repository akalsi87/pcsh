/**
 * \file assert.cpp
 * \date Jan 29, 2016
 */

#include "pcsh/assert.hpp"

#include <cstdlib>

#if defined(_MSC_VER)

#define _CRT_SECURE_NO_WARNINGS
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <io.h>

#else//UNIX

#include <unistd.h>
#include <fcntl.h>

#endif//defined(_MSC_VER)

#define STDERR_FILENO 2

namespace pcsh {

    namespace {

        void write_char_stderr(const char ch)
        {
            int rv = 0;
            while (rv != 1) {
#if defined(_MSC_VER)
                rv = _write(STDERR_FILENO, &ch, 1);
#else//UNIX
                rv = write(STDERR_FILENO, &ch, 1);
#endif//defined(_MSC_VER)
            }
        }

        void write_string_stderr(const char* str)
        {
            while (*str != '\0') {
                write_char_stderr(*str++);
            }
        }

    }//namespace

    int assert_fail(const char* msg, const char* file, const char* line, const char* fcn)
    {
        write_string_stderr("ASSERTION failed\n  ");
        write_string_stderr(file);
        write_string_stderr(" (");
        write_string_stderr(line);
        write_string_stderr(") : ");
        write_string_stderr(fcn);
        write_string_stderr(" : ");
        write_string_stderr(msg);
        write_string_stderr("\n");      
        abort();
        return 0;
    }

}//namespace pcsh
