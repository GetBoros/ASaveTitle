#pragma once

#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	~AsTools();
	AsTools();

	static void Format_Text_Using_Patterns(const wchar_t *url, const wchar_t *pattern_str, const wchar_t *pattern_end, wchar_t **result);  // result with start pattern
	static void Format_Text_Using_Patterns_Extended(const wchar_t *url, const wchar_t *pattern_str, const wchar_t *pattern_end, wchar_t **result);  // result with start pattern
	
	static bool Is_Button_Pressed;

private:
	long long start_tick, end_tick, delta_tick;
};
//------------------------------------------------------------------------------------------------------------
