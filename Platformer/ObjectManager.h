#ifndef __OBJECTMANAGER_H_INCLUDED__
#define __OBJECTMANAGER_H_INCLUDED__
#include "Common.h"

class ObjectManager : UniversalInterface<ObjectManager>
{
public:
	bool Init()
	{
		return true;
	}
	bool Process()
	{
		return true;
	}
	void Release()
	{
		return;
	}
};

#endif