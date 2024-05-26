#include "AsTools.h"

// AsTools
bool AsTools::Is_Button_Pressed = false;
AsTools::~AsTools()
{
	end_tick = __rdtsc();
	delta_tick = end_tick - start_tick;  //112 197 229, 120 479 993, v7bs - 2 500 999 564 || 5 863 272 714 | 2 393 526 266
}
//------------------------------------------------------------------------------------------------------------
AsTools::AsTools()
	: start_tick(0), end_tick(0), delta_tick(0)
{
	start_tick = __rdtsc();
}
//------------------------------------------------------------------------------------------------------------
