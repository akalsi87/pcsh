/**
 * \file arena.cpp
 * \date Jan 28, 2016
 */

#include "pcsh/arena.hpp"

#include <algorithm>

namespace pcsh {

    struct header
    {
        size_t sz;

        header(size_t s, bool dtor = false) : sz((s & ~size_t(1)) | (dtor ? 1 : 0))
        {
        }

        size_t size() const
        {
            return sz & ~size_t(1);
        }

        bool has_dtor() const
        {
            return sz & size_t(1);
        }

        char* data()
        {
            auto ptr = reinterpret_cast<char*>(this);
            return ptr + sizeof(header) + (has_dtor() ? sizeof(void*) : 0);
        }

        header* next()
        {
            return reinterpret_cast<header*>(data() + size());
        }
    };

    struct dtor_header : public header
    {
        void* ptr;

        dtor_header(size_t s) : header(s, true)
        {
        }
    };

    struct segment
    {
        segment* fwd;
        size_t sz;
        size_t left;
        char* curr;

        segment(size_t s) : fwd(nullptr), sz(s), left(s), curr(nullptr)
        {
            curr = reinterpret_cast<char*>(const_cast<segment*>(this) + 1);
        }

        inline header* begin()
        {
            return reinterpret_cast<header*>(const_cast<segment*>(this) + 1);
        }

        inline header* end()
        {
            return reinterpret_cast<header*>(curr);
        }

        inline void call_dtors()
        {
            header* f = begin();
            header* const e = end();
            while (f != e) {
                if (f->has_dtor()) {
                    typedef void (*dtorfn)(void*);
                    static_assert(
                        std::is_same<dtorfn, arena::destroyfn>::value,
                        "Destructor functions must have compatible signatures!");
                    static_cast<dtorfn>(static_cast<dtor_header*>(f)->ptr)(f->data());
                }
                f = f->next();
            }
        }
    };

    void* call_malloc(size_t sz)
    {
        return ::operator new(sz);
    }

    void call_free(void* ptr)
    {
        ::operator delete(ptr);
    }

    segment* new_segment(size_t sz)
    {
        static const size_t ALIGN = 15;
        sz += sizeof(segment);
        const size_t req = (sz + ALIGN) & ~ALIGN;
        void* mem = call_malloc(req);
        return new (mem) segment(req - sizeof(segment));
    }

    void destroy_segments(segment* seg)
    {
        while (seg) {
            segment* next = seg->fwd;
            seg->call_dtors();
            call_free(seg);
            seg = next;
        }
    }

    void* allocate_from_seg(segment* seg, size_t sz, void* ptr)
    {
        header* h = nullptr;
        if (ptr) {
            dtor_header* hdr = new (seg->end()) dtor_header(sz);
            hdr->ptr = ptr;
            h = hdr;
        } else {
            header* hdr = new (seg->end()) header(sz);
            h = hdr;
        }
        seg->curr = reinterpret_cast<char*>(h->next());
        seg->left -= sz;
        return h->data();
    }

    struct arena::impl
    {
        segment* seg_;
        size_t minsz_;

        impl(size_t sz) : seg_(new_segment(sz)), minsz_(sz)
        {
        }

        ~impl()
        {
            if (seg_) {
                destroy_segments(seg_);
            }
        }

        void* allocate(size_t sz, void* fptr)
        {
            if (seg_->left > sz) {
                return allocate_from_seg(seg_, sz, fptr);
            } else {
                size_t segsz = std::max(sz + sizeof(header) + (fptr ? sizeof(void*) : 0), minsz_);
                segment* s = new_segment(segsz);
                s->fwd = seg_;
                seg_ = s;
                return allocate_from_seg(s, sz, fptr);
            }
        }
    };

    arena::arena(size_t sz) : impl_(new arena::impl(sz))
    {
    }

    arena::~arena()
    {
        delete impl_;
    }

    void* arena::allocate(size_t sz, arena::destroyfn fn)
    {
        sz = (sz + 3) & ~size_t(3);
        size_t szneeded = sz + (fn ? sizeof(void*) : 0);
        return impl_->allocate(szneeded, fn);
    }

}//namespace pcsh
