#ifndef SYNC_H
#define SYNC_H

#include "instructions.h"

#warning To atomic


template <typename T=uint8_t>
class SyncCounter {

	T value;
public:
	SyncCounter() : value{0} {}

	//------------------------------------------------------------------------
	T get()
	{
		cpu::dmb();
		return value;
	}

	//------------------------------------------------------------------------
	void set(T val)
	{
		T oldValue;
		do {
			oldValue = cpu::ldrex(&value);
		}while (cpu::strex(val, &value));
		cpu::dmb();
	}

	//------------------------------------------------------------------------
	SyncCounter<T>& operator++()
	{
		T oldValue;
		do {
			oldValue = cpu::ldrex(&value) + 1;
		}while (cpu::strex(oldValue, &value));
		cpu::dmb();

		return *this;
	}

	//------------------------------------------------------------------------
	void incMax(T max)
	{
		T oldValue;
		do {
			oldValue = cpu::ldrex(&value);
			if (oldValue < max)
				++oldValue;
		}while (cpu::strex(oldValue, &value));
		cpu::dmb();
	}

	//------------------------------------------------------------------------
	SyncCounter<T>& operator--()
	{
		T oldValue;
		do {
			oldValue = cpu::ldrex(&value) - 1;
		}while (cpu::strex(oldValue, &value));
		cpu::dmb();

		return *this;
	}

	//------------------------------------------------------------------------
	void decMin(T min)
	{
		T oldValue;
		do {
			oldValue = cpu::ldrex(&value);
			if (oldValue < min)
				--oldValue;
		}while (cpu::strex(oldValue, &value));
		cpu::dmb();
	}
};




#endif // SYNC_H
