#include "AsTools.h"

// AsTools
AsTools::~AsTools()
{
	end_tick = __rdtsc();
	delta_tick = end_tick - start_tick;  // 194176 170824 192866

	if (delta_tick > 100000)  // 86000
		int yy = 0;  // Bed
	else if (delta_tick < 100000)
		int yy = 1;  // Good  // 25464
}
//------------------------------------------------------------------------------------------------------------
AsTools::AsTools()
 : start_tick(0), end_tick(0), delta_tick(0)
{
	start_tick = __rdtsc();
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Format_Url_Sub_WString(const wchar_t *content, const wchar_t *bgn, const wchar_t *end, wchar_t *&result)
{
	const wchar_t *ptr_start = wcsstr(content, bgn) + wcslen(bgn);
	if (!ptr_start != 0)
		return;

	const wchar_t *ptr_end = wcsstr(ptr_start, end);
	if (!ptr_end != 0)
		return;

	const int context_length = static_cast<int>(ptr_end - ptr_start) + 1;
	result = new wchar_t[context_length] {};

	wcsncpy_s(result, context_length, ptr_start, static_cast<rsize_t>(context_length - 1) );
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Format_Url_Sub_String(const char *content, const char *bgn, const char *end, char *&result)
{
	int title_length = 0;

	const char *title_str = strstr(content, bgn) + strlen(bgn);
	if (!title_str != 0)
		return;

	const char *title_end = strstr(title_str, end);
	if (!title_end != 0)
		return;

	title_length = (int)(title_end - title_str) + 1;  // + 1 for zero
	result = new char[title_length] {};

	strncpy_s(result, title_length, title_str, static_cast<rsize_t>(title_length - 1) );
}
//------------------------------------------------------------------------------------------------------------
