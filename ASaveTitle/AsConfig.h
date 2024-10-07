#pragma once

#include "AsTools.h"

// AFont
class AFont
{
public:
	~AFont();
	AFont(int height, int weight, int family, const wchar_t *face_name, bool is_italic = false);

	void Select(HDC hdc) const;

private:
	HFONT Font_Handle;
};
//------------------------------------------------------------------------------------------------------------



// AsConfig
class AsConfig
{
public:
	~AsConfig();
	AsConfig();

	static void Throw();

	static HWND Hwnd;

	static const char Dictionary_Main_Folder[];
	static const char Image_Name_File[];
	static const char Path_Saves_Folder[];
	static const char Pattern_Default[];
	static const char Pattern_Default_Anime_Bit[];
	static const char *Settings[];
	static const char *Folders_Save[];
	static const char *Saved_Path[];
	static const char *Temporary_File_Name[];

	static constexpr int Menu_Main_Button_Count = 6;
	static constexpr int Context_Button_Count = Menu_Main_Button_Count - 1;

	static const wchar_t Patterns_Info[];
	static const wchar_t Pattern_Default_Bin[];
	static const wchar_t Pattern_Default_TXT[];
	static const wchar_t Main_Image_Folder[];
	static const wchar_t Incompatible_Char_Type[];
	static const wchar_t Image_Folder[];
	static const wchar_t Image_Format[];
	static const wchar_t Main_Menu_Title_Name[];
	static const wchar_t Sub_Menu_Title[];
	static const wchar_t Sub_Menu_User_Input_Title[];
	static const wchar_t *Menu_Main_Buttons_Text_Eng[Menu_Main_Button_Count];
	static const wchar_t *Menu_Main_Buttons_Text_Rus[Menu_Main_Button_Count];
	static const wchar_t *Patterns_Config[];
	static const wchar_t *Battons_Page_Name[];
	static const wchar_t *Season_Case_Up[];
	static const wchar_t *Season_Case_Low[];
	static const wchar_t *Text_Program_Names[];
	static const wchar_t *Protocols[];

	static const int Settings_Size;
	static const int Max_Line;
	static const int Max_Loadstring = 100;
	static const int Global_Scale = 3;
	static const int Ch_W = 8;
	static const int Ch_H = 16;
	static const int Menu_Main_Title_Length = 50;
	static constexpr int Window_Width = (int)( (double)960 * 1.5);
	static constexpr int Window_Height = (int)((double)540 * 1.5);
	static constexpr int Main_Image_Width = 424;
	static constexpr int Main_Image_Height = 818;
	static constexpr int User_Input_Buffer = Window_Width / Ch_W;
	static const unsigned long long LL_Max;
	static const unsigned long long ULL_Max;
	static const unsigned long long ULL_Length;
	static const unsigned long long ULL_Index_Length;
	
	static const COLORREF Color_White;
	static const COLORREF Color_Green;
	static const COLORREF Color_Text_Green;
	static const COLORREF Color_Dark;
	static const COLORREF Color_Yellow;
	static const COLORREF Color_Red;
	static const COLORREF Color_Backgrount_Text;

	static const AFont Title_Font, Logo_Pop_Font, Game_Over_Font;

	static const HBRUSH Brush_White;
	static const HBRUSH Brush_Gray;
	static const HBRUSH Brush_Green;
	static const HBRUSH Brush_Green_Dark;
	static const HBRUSH Brush_Background_Dark;
	static const HBRUSH Brush_Background_Button_Update;

	static RECT Window_Rect;
};
//------------------------------------------------------------------------------------------------------------

