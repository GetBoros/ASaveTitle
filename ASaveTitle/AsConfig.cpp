#include "AsConfig.h"

// AsConfig
const char AsConfig::Dictionary_Main_Folder[] = "Map.bin";
const char AsConfig::Image_Name_File[] = "TemporaryName.png";
const char AsConfig::Path_Saves_Folder[] = "Data/";
const char AsConfig::Pattern_Default[] = "init_load = <h1>\ninit_load_line = 100\nurl_example = cache.dreamerscast.com/releases/\ntitle_bgn = laquo;\ntitle_end = &raquo\ntitle_num_bgn = Серии: [\ntitle_num_end =  \nimage_bgn = <img src='\nimage_end = ' width\n";
const char *AsConfig::Settings[] = { "Standart Background = ;", "Coordinates = ;", "Something Else = ;" };
const char *AsConfig::Folders_Save[] = { "Watching.bin", "Library.bin", "Paused.bin", "Wishlist.bin" };
const char *AsConfig::Saved_Path[] = { "Data/Watching.bin", "Data/Library.bin", "Data/Paused.bin", "Data/Wishlist.bin" };
const char *AsConfig::Temporary_File_Name[] = {"Data/1.bin", "Data/2.bin", "Data/3.bin", "Data/4.bin", "Data/5.bin", "Data/6.bin", "Data/7.bin", "Data/8.bin", };

const wchar_t AsConfig::Pattern_Default_Bin[] = L"Url.bin";
const wchar_t AsConfig::Pattern_Default_TXT[] = L"/PatternFindConfig.txt";
const wchar_t AsConfig::Main_Image_Folder[] = L"Main_Image.png";
const wchar_t AsConfig::Image_Folder[] = L"Pictures/";
const wchar_t AsConfig::Image_Format[] = L".png";
const wchar_t AsConfig::Main_Menu_Title_Name[] = L"Title Saver";
const wchar_t AsConfig::Sub_Menu_Title[] = L"                                TITLE NAMES																								";
const wchar_t AsConfig::Sub_Menu_User_Input_Title[] = L"Enter text here... or URL your site";

const wchar_t *AsConfig::Menu_Main_Buttons_Text_Eng[] = { L"Watch", L"Watched", L"Paused", L"Add to wishlist", L"Errase from Array", L"Exit" };
const wchar_t *AsConfig::Menu_Main_Buttons_Text_Rus[] = { L"Просмотреть", L"Просмотренные", L"В перерыве", L"Добавить в желаемое", L"Удалить", L"Выход" };
const wchar_t *AsConfig::Patterns_Config[] = { L"init_load = ",  L"init_load_line = ",  L"url_example = ",  L"title_bgn = ",  L"title_end = ", L"title_num_bgn = ", L"title_num_end = ", L"image_bgn = ", L"image_end = ", L"\n" };
const wchar_t *AsConfig::Battons_Page_Name[] = { L"Update Page", L"Prev Page", L"Next Page" };
const wchar_t *AsConfig::Season_Case_Up[] = { L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X" };
const wchar_t *AsConfig::Season_Case_Low[] = { L"i", L"ii", L"iii", L"iv", L"v", L"vi", L"vii", L"viii", L"ix", L"x" };
const wchar_t *AsConfig::Text_Program_Names[] = {L"Saver", L"Book Reader", L"Invalid"};
const wchar_t *AsConfig::Protocols[] = { L"https://", L"http://" };
const wchar_t AsConfig::Seasons[] = { 'I', 'V', 'X' };

const int AsConfig::Settings_Size = sizeof(AsConfig::Settings) / sizeof(AsConfig::Settings[0]);
const int AsConfig::Max_Line = 31;
RECT AsConfig::Window_Rect {};

const COLORREF AsConfig::Color_White = RGB(255, 255, 255);
const COLORREF AsConfig::Color_Green = RGB(0, 255, 0);
const COLORREF AsConfig::Color_Text_Green = RGB(80, 146, 56);
const COLORREF AsConfig::Color_Dark = RGB(30, 30, 30);
const COLORREF AsConfig::Color_Yellow = RGB(220, 220, 10);
const COLORREF AsConfig::Color_Red = RGB(255, 32, 37);
const COLORREF AsConfig::Color_Backgrount_Text = RGB(185, 122, 87);

const HBRUSH AsConfig::Brush_White  = CreateSolidBrush(RGB(255, 255, 255) );
const HBRUSH AsConfig::Brush_Gray  = CreateSolidBrush(RGB(125, 125, 125) );
const HBRUSH AsConfig::Brush_Green = CreateSolidBrush(RGB(0, 255, 0) );
const HBRUSH AsConfig::Brush_Green_Dark = CreateSolidBrush(RGB(80, 146, 56) );
const HBRUSH AsConfig::Brush_Background_Dark = CreateSolidBrush(RGB(30, 30, 30) );
const HBRUSH AsConfig::Brush_Background_Button_Update = CreateSolidBrush(RGB(185, 122, 87) );

const unsigned long long AsConfig::LL_Max = 9223372036854775807LL;  // 9 22 33 72 03 68 54 77 58 07 LL || 8
const unsigned long long AsConfig::ULL_Max = 18446744073709551615ULL;  // 18 [44 67 44] [07 37 09] [55 16 15] ULL || 9
const unsigned long long AsConfig::ULL_Length = 1000000000000000000ULL;  // 18 [44 67 44] [07 37 09] [55 16 15] ULL || 9
const unsigned long long AsConfig::ULL_Index_Length = 10000000000000000ULL;  // !!!
//------------------------------------------------------------------------------------------------------------
AsConfig::~AsConfig()
{
	DeleteObject(Brush_White);
	DeleteObject(Brush_Gray);
	DeleteObject(Brush_Green);
	DeleteObject(Brush_Green_Dark);
	DeleteObject(Brush_Background_Dark);
	DeleteObject(Brush_Background_Button_Update);
}
//------------------------------------------------------------------------------------------------------------
AsConfig::AsConfig()
{
	Window_Rect.left = 0;
	Window_Rect.top = 0;
	Window_Rect.right = Window_Width;
	Window_Rect.bottom = Window_Height;
}
//------------------------------------------------------------------------------------------------------------
