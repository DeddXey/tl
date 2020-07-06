#ifndef BUFFERDESCRIPTOR_H
#define BUFFERDESCRIPTOR_H

//#include <tuple>
#include "log/log.h"

#include <cstdint>
#include <utility>

//TODO: move to misc and make it template
using BuffPair = std::pair<uint8_t*, uint32_t>;

///
/// \brief Descriptor of simple memory buffer
///
template <typename M =uint8_t>
class BufferDescriptor {
    const M*		    buffer;
	uint32_t        	size;

public:
    BufferDescriptor(const M* b, const uint32_t len) :
	    buffer(b),
	    size(len) {}
    BufferDescriptor() = default;


	///
	/// \brief Get BufferDescriptor from discrete parameters
	/// \param buf
	/// \param len
	///
    void fromValues(const M* buf, const uint32_t len)
	{
		this->buffer = buf;
		this->size = len;
	}



//	///
//	/// \brief Prints buffer in hexadecimal in 16 columns
//	///
//	///
//	template <typename D>
//	void print(D& debug) const
//	{

//        con.debug() << Fg::cyan
//		      << Use::hex
//		      << Use::w8
//		      << "length: "
//		      << length()
//		      << ":"
//		      << length() * sizeof(T) ;

//		const auto *ptr8 = reinterpret_cast<const uint8_t*>(buffer);

//		for (uint32_t i = 0 ; i  < length() * sizeof (T); ++i) {
//			if (i%16 == 0)
//                con.debug().putChar('\n');

//            con.debug() << Use::w2<< ptr8[i] << " ";
//		}
//        con.debug() << Attr::reset << Use::endl;
//	}


    template <typename T, typename S>
    friend LogStream<T, true>& operator<<(LogStream<T, true>& stream,
                                   const BufferDescriptor<S> &bd);

	uint32_t length() const { return size; }
    const M* ptr() const { return buffer; }
};

//----------------------------------------------------------------------------
template <typename T, typename S>
LogStream<T, true>& operator<<(LogStream<T, true>& stream,
                               const BufferDescriptor<S> &bd)
{

            stream << Fg::cyan
                  << Use::hex
                  << Use::w8
                  << "length: "
                  << bd.length()
                  << ":"
                  << bd.length() * sizeof(T) ;

            const auto *ptr8 = reinterpret_cast<const uint8_t*>(bd.buffer);

            for (uint32_t i = 0 ; i  < bd.length() * sizeof (T); ++i) {
                if (i%16 == 0)
                    stream.putChar('\n');

                stream << Use::w2<< ptr8[i] << " ";
            }
            stream << Attr::reset << Use::endl;
            return stream;

}


#endif // BUFFERDESCRIPTOR_H
