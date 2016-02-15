/**
 * \file linebufistream.hpp
 * \date Feb 13, 2016
 */

#ifndef PCSH_LINEBUFISTREAM_HPP
#define PCSH_LINEBUFISTREAM_HPP

#include <cstdlib>
#include <string>

#if defined(_WIN32)
#  define EOT_CHAR_DEF '\x26'
#  define EOT_CHAR_UNIX_DEF '\x04'
#else
#  define EOT_CHAR_DEF '\x04'
#endif

namespace pcsh {

    namespace detail {

        class linebuff : public std::streambuf
        {
          private:
            std::istream&   istrm_;
            std::string     buffer_;
            char            ch_;
            bool            eos_;
            bool            check_eot_;

            using traits = std::char_traits<char>;

            static const int EOT_CHAR = EOT_CHAR_DEF;
          public:
            linebuff(std::istream& strm, bool checkeot)
              : istrm_(strm), buffer_(), ch_(-1), eos_(false), check_eot_(checkeot)
            {
                istrm_.rdbuf()->pubsetbuf(0, 0);
                buffer_.reserve(120);
            }

            int underflow()
            {
                auto gp = gptr();
                auto ep = egptr();
                if (gp < ep) {
                    return traits::to_int_type(*gp);
                }

                if (gp == ep) {
                    int c = EOF;
                    if (eos_ || ((c = istrm_.peek()) == EOF)) {
                        eos_ = true;
                        setg(nullptr, nullptr, nullptr);
                        return traits::eof();
                    }
                    buffer_.resize(0);
                    while (c != EOF) {
                        if (c == '\r' || c == '\n') {
                            istrm_.get();
                            if ((c == '\r') && (istrm_.peek() == '\n')) {
                                istrm_.get();
                            }
                            buffer_.push_back('\n');
                            break;
                        }
                        if (check_eot_ && 
                            ((c == EOT_CHAR)
#if defined(_WIN32)
                            || (c == EOT_CHAR_UNIX_DEF)
#endif//defined(_WIN32)
                            )
                           ) {
                            eos_ = true;
                            break;
                        }
                        buffer_.push_back(c);
                        istrm_.get();
                        c = istrm_.peek();
                    }
                    setg(&buffer_[0], &buffer_[0], &buffer_[buffer_.size()]);
                }

                return ((gp = gptr()) == egptr()) ? traits::eof()
                                                  : traits::to_int_type(*gp);
            }
        };

    }// namespace detail

    class linebuff_istream : public std::istream
    {
      private:
        detail::linebuff buff_;
      public:
        linebuff_istream(std::istream& parent, bool checkEOT = true)
          : std::istream(&buff_), buff_(parent, checkEOT)
        { }
    };

}//namespace pcsh

#endif/*PCSH_LINEBUFISTREAM_HPP*/
