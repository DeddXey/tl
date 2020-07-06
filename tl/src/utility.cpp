#include "utility.h"

void simpleDelay(volatile uint32_t delay) // noinline НЕ ТРОГАТЬ!
{
    for (volatile uint32_t i = 0; i < delay; ++i) {
        NOP; // чтобы оптимизатор не оптимизировал
    }
}

//---------------------------------------------------------------------------
uint8_t hexLetterToByte(const uint8_t letter)
{
    uint8_t out = 0;

    if ((letter >= 0x30) &&
            letter <= 0x39) {
        out = letter - 0x30;
    }
    else {
        const uint8_t normalizedLetter = letter & 0x4f;

        if ((normalizedLetter >= 0x41) &&
                normalizedLetter <= 0x46) {
            out = normalizedLetter - 0x41 + 0x0A;
        }
    }
    return out;
}

//---------------------------------------------------------------------------
uint8_t decLetterToByte(const uint8_t letter)
{
    uint8_t out = 0;

    if ((letter >= 0x30) &&
            letter <= 0x39) {
        out = letter - 0x30;
    }

    return out;
}

//---------------------------------------------------------------------------
int32_t readAdcSample(const uint8_t *ptr)
{
	int32_t tmp = *(ptr) << 24 |
	               (*(ptr + 1) << 16)|
	               (*(ptr + 2) << 8);
//	tmp = __REV(tmp);
//	int32_t* pTmp = reinterpret_cast<int32_t*>(&tmp);
//	*pTmp >>= 8;
//	return *pTmp;

	return tmp >> 8;
}

//---------------------------------------------------------------------------
#define MAX_PRECISION	(10)
static const double rounders[MAX_PRECISION + 1] =
{
    0.5,				// 0
    0.05,				// 1
    0.005,				// 2
    0.0005,				// 3
    0.00005,			// 4
    0.000005,			// 5
    0.0000005,			// 6
    0.00000005,			// 7
    0.000000005,		// 8
    0.0000000005,		// 9
    0.00000000005		// 10
};

char * ftoa(double f, char * buf, int precision)
{
	char * ptr = buf;
	char * p = ptr;
	char * p1;
	char c;
	long intPart;

	// check precision bounds
	if (precision > MAX_PRECISION)
		precision = MAX_PRECISION;

	// sign stuff
	if (f < 0)
	{
		f = -f;
		*ptr++ = '-';
	}

	if (precision < 0)  // negative precision == automatic precision guess
	{
		if (f < 1.0) precision = 6;
		else if (f < 10.0) precision = 5;
		else if (f < 100.0) precision = 4;
		else if (f < 1000.0) precision = 3;
		else if (f < 10000.0) precision = 2;
		else if (f < 100000.0) precision = 1;
		else precision = 0;
	}

	// round value according the precision
	if (precision)
		f += rounders[precision];

	// integer part...
	intPart = f;
	f -= intPart;

	if (!intPart)
		*ptr++ = '0';
	else
	{
		// save start pointer
		p = ptr;

		// convert (reverse order)
		while (intPart)
		{
			*p++ = '0' + intPart % 10;
			intPart /= 10;
		}

		// save end pos
		p1 = p;

		// reverse result
		while (p > ptr)
		{
			c = *--p;
			*p = *ptr;
			*ptr++ = c;
		}

		// restore end pos
		ptr = p1;
	}

	// decimal part
	if (precision)
	{
		// place decimal point
		*ptr++ = '.';

		// convert
		while (precision--)
		{
			f *= 10.0;
			c = f;
			*ptr++ = '0' + c;
			f -= c;
		}
	}

	// terminating zero
	*ptr = 0;

	return buf;
}
