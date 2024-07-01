#include "AsConfig.h"

// AsConfig
const char *AsConfig::Settings[] = { "Standart Background = ;", "Coordinates = ;", "Something Else = ;" };
const char AsConfig::Dictionary_Main_Folder[] = "Map.bin";

const wchar_t AsConfig::Seasons[3] = { 'I', 'V', 'X' };
const wchar_t AsConfig::Image_Folder[] = L"Pictures/";
const wchar_t AsConfig::Main_Image_Folder[] = L"Main_Image.png";
const wchar_t *AsConfig::Season[] = { L"I", L"II", L"III", L"IV", L"V", L"VI", L"VII", L"VIII", L"IX", L"X" };
const wchar_t *AsConfig::Invalid_Chars[] = { L":", L"¸", L"¨" };
const wchar_t *AsConfig::Text_Program_Names[] = {L"Saver", L"Book Reader", L"Invalid"};

const wchar_t AsConfig::Invalid_Chars_Valid[] = { L',', L'å', L'å' };

const int AsConfig::Settings_Size = sizeof(AsConfig::Settings) / sizeof(AsConfig::Settings[0]);
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
const HBRUSH AsConfig::Brush_Background = CreateSolidBrush(RGB(30, 30, 30) );
const HBRUSH AsConfig::Brush_Background_Button_Update = CreateSolidBrush(RGB(185, 122, 87) );
//------------------------------------------------------------------------------------------------------------
AsConfig::~AsConfig()
{
	DeleteObject(Brush_White);
	DeleteObject(Brush_Gray);
	DeleteObject(Brush_Green);
	DeleteObject(Brush_Green_Dark);
	DeleteObject(Brush_Background);
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
