#ifndef CYCLEBUFFER_H
#define CYCLEBUFFER_H

#include <cstdint>
#include "sync.h"

///
/// Потокобезопасный циклический пул буферов
///
template <uint32_t size, uint8_t num, typename T= uint8_t>
class CycleBuffer {

	T data [num][size];
	volatile uint8_t	writeIndex = 0;
	volatile uint8_t	readIndex = 0;

	SyncCounter<uint8_t>		used;

public:

	//........................................................................
	CycleBuffer():
	    writeIndex{0},
	    readIndex{0} { }

	//........................................................................
	void resetAndWrite()
	{
		writeIndex = 0;
		readIndex = 0;
		used.set(0);
	}

	T* currentWrite()
	{
		return data[0];
	}

	//........................................................................
	T* nextWrite()
	{
		if (used.get() == num)
			return nullptr;

		uint8_t tmpIndex = writeIndex;

		used.decMin(0);

		++writeIndex;
		if (writeIndex == num)
			writeIndex = 0;

		return data[tmpIndex];
	}

	//........................................................................
	T* nextRead()
	{
		if (used.get() == 0)
			return nullptr;

		uint8_t tmpIndex = readIndex;

		used.decMin(0);

		++readIndex;
		if (readIndex == num)
			readIndex = 0;

		return data[tmpIndex];
	}
};

///
/// Потокобезопасный циклический пул всего
///
template <uint8_t num, typename T= uint8_t>
class Cycler {

	T data [num];
	volatile uint8_t	writeIndex = 0;
	volatile uint8_t	readIndex = 0;

	std_atomic<uint8_t>		used; //TODO: to atomic
//	int used;

public:

	//........................................................................
	Cycler():
	    writeIndex{0},
	    readIndex{0} { }

	//........................................................................
	void reset()
	{
		writeIndex = 0;
		readIndex = 0;
		used.set(0);
	}

	T* currentWrite()
	{
		return data[writeIndex];
	}

	//........................................................................
	T* currentRead()
	{
		return data[readIndex];
	}

	//........................................................................
	bool isEmpty()
	{
		return (used.get() == 0);
	}

	//........................................................................
	T* nextWrite()
	{
		if (used.get() == num)
			return nullptr;

		uint8_t tmpIndex = writeIndex;

		used.incMax(num);

		++writeIndex;
		if (writeIndex == num)
			writeIndex = 0;

		return &data[tmpIndex];
	}

	//........................................................................
	T* nextRead()
	{
		if (used.get() == 0)
			return nullptr;

		uint8_t tmpIndex = readIndex;

		used.decMin(0);

		++readIndex;
		if (readIndex == num)
			readIndex = 0;

		return &data[tmpIndex];
	}
};





#endif //CYCLEBUFFER_H
