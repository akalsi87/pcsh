/**
 * \file arena.hpp
 * \date Jan 28, 2016
 */

#ifndef PCSH_ARENA_HPP
#define PCSH_ARENA_HPP

#include "pcsh/exportsym.h"

#include <new>
#include <type_traits>

namespace pcsh {

    class PCSH_API arena
    {
      public:
        arena(size_t sz = 1024);

        ~arena();

        template <class T, class... Args>
        T* create(Args&&... args)
        {
            static const bool trivialdtor = std::is_trivially_destructible<T>::value;
            void* mem = allocate(sizeof(T), (trivialdtor ? nullptr : &destroyer<T>::act));
            return new (mem) T(std::forward<Args>(args)...);
        }

        template <class T>
        T* create_array(size_t n)
        {
            void* mem = allocate(sizeof(T) * n, nullptr);
            return reinterpret_cast<T*>(mem);
        }
      private:
        typedef void (*destroyfn)(void*);
        
        struct impl;

        void* allocate(size_t sz, destroyfn fn);

        impl* impl_;

        template <class T>
        struct destroyer
        {
            inline static void act(void* ptr)
            {
                static_cast<T*>(ptr)->~T();
            }
        };
    };

}//namespace pcsh

#endif/*PCSH_ARENA_HPP*/
