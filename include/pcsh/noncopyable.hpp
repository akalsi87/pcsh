/**
 * \file noncopyable.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_NONCOPYABLE_HPP
#define PCSH_NONCOPYABLE_HPP

// Disable MSVC warnings for non DLL-interface inheritance
// from noncopyable
#if defined(_MSC_VER)
#pragma warning (disable:4275)
#endif // defined(_MSC_VER)

namespace pcsh {

    class noncopyable
    {
      protected:
        inline noncopyable()
        { }

        inline virtual ~noncopyable()
        { }

      private:
        // declared but never defined
        noncopyable(const noncopyable&);
        noncopyable& operator=(const noncopyable&);
    };

}//namespace pcsh

// Unfortunately, this disabling must propagate for
// it to correctly silence the MSVC warnings.
//#if defined(_MSC_VER)
//#pragma warning (default:4275)
//#endif // defined(_MSC_VER)

#endif/*PCSH_NONCOPYABLE_HPP*/
