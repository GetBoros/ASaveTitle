#pragma once

#include "AsTools.h"

enum class EConfig_Params : int
{
	ECP_Standart_Background,
	ECP_Coordinates,
	ECP_Something_Else
};

enum class EDictionary_Ñhar : int
{
	EDC_None = -1,
	EDC_A = 65,

	EDC_P = 80,

	EDC_Z = 90,
	EDC_End = 99
};




// AsConfig
class AsConfig
{
public:
	~AsConfig();
	AsConfig();

	static const char Dictionary_Main_Folder[];
	static const char Path_Sites_Folder[];

	static const wchar_t Seasons[];
	static const wchar_t Image_Folder[];
	static const wchar_t Main_Image_Folder[];
	static const wchar_t *Season[];

	static const int Invalid_Chars_Len = 3;
	static const wchar_t *Invalid_Chars[Invalid_Chars_Len];
	static const wchar_t Invalid_Chars_Valid[];
	static const wchar_t *Text_Program_Names[];
	static const char *Settings[];
	static const int Settings_Size;

	static const int Max_Loadstring = 100;
	static const int Global_Scale = 3;
	static const int Ch_W = 8;
	static const int Ch_H = 16;
	static constexpr int Window_Width = (int)( (double)960 * 1.5);
	static constexpr int Window_Height = (int)((double)540 * 1.5);
	static constexpr int Main_Image_Width = 424;
	static constexpr int Main_Image_Height = 818;
	static constexpr int User_Input_Buffer = Window_Width / Ch_W;
	static const unsigned long long LL_Max= 9223372036854775807LL;  // 9 22 33 72 03 68 54 77 58 07 LL || 8
	static const unsigned long long ULL_Max = 18446744073709551615ULL;  // 18 [44 67 44] [07 37 09] [55 16 15] ULL || 9
	static const unsigned long long ULL_Length = 1000000000000000000ULL;  // 18 [44 67 44] [07 37 09] [55 16 15] ULL || 9
	
	
	static const COLORREF Color_White;
	static const COLORREF Color_Green;
	static const COLORREF Color_Text_Green;
	static const COLORREF Color_Dark;
	static const COLORREF Color_Yellow;
	static const COLORREF Color_Red;
	static const COLORREF Color_Backgrount_Text;

	static const HBRUSH Brush_White;
	static const HBRUSH Brush_Gray;
	static const HBRUSH Brush_Green;
	static const HBRUSH Brush_Green_Dark;
	static const HBRUSH Brush_Background;
	static const HBRUSH Brush_Background_Button_Update;

	static RECT Window_Rect;
};
//------------------------------------------------------------------------------------------------------------

