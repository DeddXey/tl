#ifndef SYNC_H
#define SYNC_H

#include "instructions.h"
#include <atomic>

namespace tl{
//TODO: make usable
class atomic_mutex {
  std::atomic_flag flag{ATOMIC_FLAG_INIT};

public:
  void lock()
  {
    while (flag.test_and_set());
  }

  void unlock()
  {
    flag.clear();
  }
};
}

template <typename T=uint8_t>
 class [[deprecated]] SyncCounter  {

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
