#include "stdafx.h"
#include "AsTools.h"

// AsTools
unsigned long long AsTools::How_Fast = 0LL;
//------------------------------------------------------------------------------------------------------------
AsTools::~AsTools()
{
	end_tick = __rdtsc();
	delta_tick = end_tick - start_tick;  // 194176 170824 192866
	How_Fast = delta_tick;

	if (delta_tick > 1999888)  // 86000
		How_Fast = 0;  // Good
	else if (delta_tick < 999888)
		How_Fast = 0;  // Bad |
}
//------------------------------------------------------------------------------------------------------------
AsTools::AsTools()
 : start_tick(0LL), end_tick(0LL), delta_tick(0LL)
{
	start_tick = __rdtsc();
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Format_Sub_WString(const wchar_t *content, const wchar_t *bgn, const wchar_t *end, wchar_t *&result)
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
void AsTools::Format_Sub_String(const char *content, const char *bgn, const char *end, char *&result)
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
void AsTools::Format_Wide_Char_To_Char(const wchar_t *w_char, char *&result)
{
	const int size = WideCharToMultiByte(CP_UTF8, 0, w_char, -1, 0, 0, 0, 0);
	result = new char[size] {};
	WideCharToMultiByte(CP_UTF8, 0, w_char, -1, result, size, 0, 0);
}
//------------------------------------------------------------------------------------------------------------
void AsTools::Format_Char_To_Wide_Char(const char *c_char, wchar_t *&result)
{
	const int size = MultiByteToWideChar(CP_UTF8, 0, c_char, -1, 0, 0);
	result = new wchar_t[size] {};
	MultiByteToWideChar(CP_UTF8, 0, c_char, -1, result, size);
}
//------------------------------------------------------------------------------------------------------------
