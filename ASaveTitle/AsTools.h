#pragma once

#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	~AsTools();
	AsTools();

	static void Format_Sub_WString(const wchar_t *content, const wchar_t *bgn, const wchar_t *end, wchar_t *&result);  // result with start pattern
	static void Format_Sub_String(const char *content, const char *bgn, const char *end, char *&result);
	static void Format_Wide_Char_To_Char(const wchar_t *w_char, char *&result);
	static void Format_Char_To_Wide_Char(const char *c_char, wchar_t *&result);

private:
	unsigned long long start_tick, end_tick, delta_tick;
};
//------------------------------------------------------------------------------------------------------------
