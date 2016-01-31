/**
 * \file noncopyable.hpp
 * \date Jan 31, 2016
 */

#ifndef PCSH_NONCOPYABLE_HPP
#define PCSH_NONCOPYABLE_HPP

namespace pcsh {

    class noncopyable
    {
      protected:
        inline noncopyable()
        { }

        inline ~noncopyable()
        { }

      private:
        // declared but never defined
        noncopyable(const noncopyable&);
        noncopyable& operator=(const noncopyable&);
    };

}//namespace pcsh

#endif/*PCSH_NONCOPYABLE_HPP*/
