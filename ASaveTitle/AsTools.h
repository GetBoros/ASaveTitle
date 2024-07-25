#pragma once

#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	~AsTools();
	AsTools();

	static void Format_Url_Sub_WString(const wchar_t *content, const wchar_t *bgn, const wchar_t *end, wchar_t *&result);  // result with start pattern
	static void Format_Url_Sub_String(const char *content, const char *bgn, const char *end, char *&result);

private:
	long long start_tick, end_tick, delta_tick;
};
//------------------------------------------------------------------------------------------------------------
