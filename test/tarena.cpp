/**
 * \file tarena.cpp
 * \date Jan 28, 2016
 */

#include "unittest.hpp"

#include "pcsh/arena.hpp"

CPP_TEST( arena_none )
{
    pcsh::arena a(1024);
}

CPP_TEST( arena_basic )
{
    pcsh::arena a(1024);
    {/* simple create */
        int* p = a.create<int>();
        TEST_TRUE(*p == 0);
    }
    {/* create int array */
        int* arr = a.create_array<int>(50);
        arr[0] = 0;
        arr[49] = 49;
    }
    {/* variadic create */
        struct Foo
        {
            size_t a;
            void* b;

            Foo(size_t x, void* y) : a(x), b(y) { }
        };

        auto pfoo = a.create<Foo>(42, reinterpret_cast<void*>(42));
        TEST_TRUE(pfoo->a == 42);
        TEST_TRUE(pfoo->b == reinterpret_cast<void*>(42));
    }
    {/* new segment create */
        struct Foo
        {
            char bytes[2048];
        };

        auto pfoo = a.create<Foo>();
    }
    {/* new segment create; non trivial dtor */
        struct Foo
        {
            char bytes[2048];

            ~Foo()
            {
                printf("Ran Foo::~Foo()\n");
            }
        };

        auto pfoo = a.create<Foo>();
    }
}
