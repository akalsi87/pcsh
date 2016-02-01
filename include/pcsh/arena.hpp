/**
 * \file arena.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_ARENA_HPP
#define PCSH_ARENA_HPP

#include "pcsh/exportsym.h"

#include <algorithm>
#include <cstring>
#include <new>
#include <type_traits>

namespace pcsh {

    //////////////////////////////////////////////////////////////////////////
    /// arena
    //////////////////////////////////////////////////////////////////////////

    class PCSH_API arena
    {
      public:
        arena(size_t sz = 1024);

        ~arena();

        inline const char* create_string(const char* str)
        {
            return create_string(str, ::strlen(str));
        }

        inline const char* create_string(const char* str, size_t len)
        {
            auto arr = create_array<char>(len + 1);
            ::memcpy(arr, str, len);
            arr[len] = '\0';
            return arr;
        }

        template <class T, class... Args>
        inline T* create(Args&&... args)
        {
            static const bool trivialdtor = std::is_trivially_destructible<T>::value;
            void* mem = allocate(sizeof(T), (trivialdtor ? nullptr : &destroyer<T>::act));
            return new (mem) T(std::forward<Args>(args)...);
        }

        template <class T>
        inline T* create_array(size_t n)
        {
            void* mem = allocate(sizeof(T) * n, nullptr);
            return reinterpret_cast<T*>(mem);
        }

        typedef void (*destroyfn)(void*);
      private:
        struct impl;

        template <class T>
        struct destroyer
        {
            inline static void act(void* ptr)
            {
                static_cast<T*>(ptr)->~T();
            }
        };

        void* allocate(size_t sz, destroyfn fn);

        impl* impl_;
    };

}//namespace pcsh

#endif/*PCSH_ARENA_HPP*/
