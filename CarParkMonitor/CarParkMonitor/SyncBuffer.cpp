#include "SyncBuffer.h"

template<class T>
SyncBuffer<T>::~SyncBuffer( void )
{

}

template<class T>
SyncBuffer<T>::SyncBuffer( void )
{

}


template<class T>
T SyncBuffer<T>::getNextItem()
{
	return this->container[0];
}

template<class T>
bool SyncBuffer<T>::hasItems()
{
	return true;
}