#pragma once
#include "VCommonDefines.h"

class VSystemManager: public VSingleton<VSystemManager>
{
public:
	VSystemManager(void);
	~VSystemManager(void);
public:
	void Start(void);
	void Stop(void);
};
