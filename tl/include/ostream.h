#ifndef OSTREAM_H
#define OSTREAM_H

#include <cstdint>
//#include <string_view>
#include <array>
#include "utility.h"


enum class Fg:uint8_t {
	black = '0', ///< Черный
	red = '1',
	green = '2',
	yellow = '3',
	blue = '4',
	magenta = '5',
	cyan = '6',
	white = '7',
	default_ = '9'
};

enum class Bg:uint8_t {
	black = '0', ///< Черный
	red = '1',
	green = '2',
	yellow = '3',
	blue = '4',
	magenta = '5',
	cyan = '6',
	white = '7',
	default_ = '9'
};

enum class Attr:uint8_t {
	reset = '0',
	bright = '1',
	dim = '2',
	standout = '3',
	undercore = '4',
	blink = '5',
	reverse = '7',
	hidden = '8'
};

enum class Use:uint8_t {
	dec,
	hex,
	endl,
	tab,
	w0,
	w2,
	w4,
	w8
};



template<typename I,
         int flag>
class Ostream;

template<typename I>
class Ostream<I, 1>
{
	uint8_t base = 10;
	uint8_t trailing = 0;

public:
	typename I::OutStream port;
//	using Port = typename I::OutStream;

	//------------------------------------------------------------------------
	void putNumber(uint32_t num)
	{
		std::array<uint8_t, 20> buffer;
		uint8_t i = 0;
		int32_t sum = num;
		do
		{
			int32_t digit;
			digit = sum % base;
			if (digit < 0xA)
				buffer[i] = '0' + digit;
			else
				buffer[i] = 'A' + digit - 0xA;
			sum /= base;
			++i;
		}while (sum);

		for (int j = 0; j < trailing - i; ++j) {
			putChar('0');
		}

		for (; i != 0; --i) {
			putChar(buffer[i - 1]);
		}
	}

	//------------------------------------------------------------------------
	void putNumber(int32_t num)
	{
		std::array<uint8_t, 20> buffer;
		uint8_t i = 0;
		int32_t sum = num;

		if (num < 0) {
			putChar('-');
			uint32_t *ptr = reinterpret_cast<uint32_t *>(&num);
			sum = ~(*ptr - 1);
		}


		do
		{
			int32_t digit;
			digit = sum % base;
			if (digit < 0xA)
				buffer[i] = '0' + digit;
			else
				buffer[i] = 'A' + digit - 0xA;
			sum /= base;
			++i;
		}while (sum);

		for (int j = 0; j < trailing - i; ++j) {
			putChar('0');
		}

		for (; i != 0; --i) {
			putChar(buffer[i - 1]);
		}
	}

	//------------------------------------------------------------------------
	void putNumber(int16_t num)
	{
		std::array<uint8_t, 20> buffer;
		uint8_t i = 0;
		int16_t sum = num;

		if (num < 0) {
			putChar('-');
			uint16_t *ptr = reinterpret_cast<uint16_t *>(&num);
			sum = ~(*ptr - 1);
		}


		do
		{
			int16_t digit;
			digit = sum % base;
			if (digit < 0xA)
				buffer[i] = '0' + digit;
			else
				buffer[i] = 'A' + digit - 0xA;
			sum /= base;
			++i;
		}while (sum);

		for (int j = 0; j < trailing - i; ++j) {
			putChar('0');
		}

		for (; i != 0; --i) {
			putChar(buffer[i - 1]);
		}
	}


	//------------------------------------------------------------------------
	void init()
	{
		I::preInit();
	}

	//------------------------------------------------------------------------
	uint8_t getBase() const
	{
		return base;
	}

	//------------------------------------------------------------------------
	uint8_t getTrailing() const
	{
		return trailing;
	}

	//------------------------------------------------------------------------
	void putRawData(const uint8_t* buf, const uint16_t len)
	{
		for (int i = 0; i < len; ++i) {
			putChar(buf[i]);
		}
	}

	//------------------------------------------------------------------------
	void putChar(const uint8_t byte)
	{
		port.putChar(byte);
		//Port::putChar(byte);
	}

	//------------------------------------------------------------------------
	void setBase(const uint8_t base)
	{
		this->base = base;
	}

	//------------------------------------------------------------------------
	void setWidth(const uint8_t width)
	{
		this->trailing = width;
	}
};

template<typename I>
class Ostream<I, 0>
{
public:
	typename I::OutStream port;

	void putNumber(UNUSED const int32_t num) {}
	inline void putRawData(UNUSED const uint8_t* buf,
	                       UNUSED const uint16_t len){}
	inline void putChar(UNUSED const uint8_t byte){}
	inline void setBase(UNUSED const uint8_t base) {}
	void setWidth(UNUSED const uint8_t width) {}
	void init() {}
};

template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const char* val)
{
	while (*val)
		stream.putChar(*val++);
	return stream;
}


//template <typename T, int flag>
//Ostream<T, flag>& operator<<(Ostream<T, flag>& stream,
//                         const std::string_view<char> val)
//{
//	for (auto a: val) {
//		stream.putChar(a);
//	}
//	return stream;
//}

template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const int32_t val)
{
	stream.putNumber(val);

	return stream;
}


template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Fg val)
{
	const uint8_t buf[] {27, '[', '3', static_cast<uint8_t>(val), 'm'};
	stream.putRawData(buf, sizeof (buf));
	return stream;
}

template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Bg val)
{
	const uint8_t buf[] {27, '[', '4', static_cast<uint8_t>(val), 'm'};
	stream.putRawData(buf, sizeof (buf));
	return stream;
}

template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Attr val)
{
	const uint8_t buf[] {27, '[', static_cast<uint8_t>(val), 'm'};
	stream.putRawData(buf, sizeof (buf));
	return stream;
}

template <typename T, int flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, Use val)
{
	switch (val) {
	case Use::dec:
		stream.setBase(10);
		break;
	case Use::hex:
		stream.setBase(16);
		break;
	case Use::endl:
	//	stream.putChar('\n');
		stream.putChar('\r');
		break;
	case Use::tab:
		stream.putChar('\t');
		break;
	case Use::w0:
		stream.setWidth(0);
		break;
	case Use::w2:
		stream.setWidth(2);
		break;
	case Use::w4:
		stream.setWidth(4);
		break;
	case Use::w8:
		stream.setWidth(8);
		break;
	}
	return stream;
}


#endif // OSTREAM_H
