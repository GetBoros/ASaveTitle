#pragma once

#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	~AsTools();
	AsTools();

	static bool Is_Button_Pressed;

private:
	long long start_tick, end_tick, delta_tick;
};
//------------------------------------------------------------------------------------------------------------
