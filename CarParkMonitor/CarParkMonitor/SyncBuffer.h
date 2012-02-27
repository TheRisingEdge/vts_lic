#pragma once
#include <vector>

template<class T>
class SyncBuffer
{
private:
	T* container;

public:
	SyncBuffer(void);
	~SyncBuffer(void);

	bool hasItems();
	T getNextItem();
};




