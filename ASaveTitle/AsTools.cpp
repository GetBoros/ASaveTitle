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
void AsTools::Format_Text_Using_Patterns(const wchar_t *url, const wchar_t *pattern_str, const wchar_t *pattern_end, wchar_t **result)
{
	const wchar_t *ptr_start = wcsstr(url, pattern_str);

	if (!ptr_start)
		return;

	const wchar_t *ptr_end = wcsstr(ptr_start, pattern_end);
	if (!ptr_end)  // don`t found end
	{
		*result = new wchar_t[1]{};
		return;
	}
	const int context_length = static_cast<int>(ptr_end - ptr_start + 1);
	*result = new wchar_t[context_length] {};
	const int context_end = context_length - 1;
	wcsncpy_s(*result, context_length, ptr_start, context_end);
}
//------------------------------------------------------------------------------------------------------------
