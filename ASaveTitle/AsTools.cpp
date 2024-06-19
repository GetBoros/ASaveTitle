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
unsigned short AsTools::Format_Text_Using_Patterns(const wchar_t *url, const wchar_t *pattern_str, const wchar_t *pattern_end)
{
	wchar_t *context;
	unsigned short save_to_file;
	const wchar_t *ptr_start = wcsstr(url, pattern_str);
	const int pattern_length = static_cast<int>(wcslen(pattern_str) );
	const wchar_t *ptr_context_begin = ptr_start + pattern_length;

	save_to_file = 0;

	if (ptr_start != 0 && ptr_context_begin != 0)
	{
		const wchar_t *ptr_end = wcsstr(ptr_context_begin, pattern_end);
		const int context_length = static_cast<int>(ptr_end - ptr_context_begin + 1);
		context = new wchar_t[context_length] {};
		const int context_end = static_cast<int>(wcslen(ptr_context_begin) - 1);

		wcsncpy_s(context, context_length, ptr_context_begin, context_end);
		save_to_file = static_cast<unsigned short>(std::stoi(context) );

		delete[] context;
		return save_to_file;
	}
	else
		return 0;
}
//------------------------------------------------------------------------------------------------------------
