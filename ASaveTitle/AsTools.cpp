#include "AsTools.h"

// AsTools
bool AsTools::Is_Button_Pressed = false;
AsTools::~AsTools()
{
	end_tick = __rdtsc();
	delta_tick = end_tick - start_tick;  // 194176 170824 192866

	if (delta_tick > 4086646)
		int yy = 0;  // Bed
	else if (delta_tick < 4086646 / 2)
		int yy = 1;  // Good
}
//------------------------------------------------------------------------------------------------------------
AsTools::AsTools()
	: start_tick(0), end_tick(0), delta_tick(0)
{
	start_tick = __rdtsc();
}
//------------------------------------------------------------------------------------------------------------
