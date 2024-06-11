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

const HBRUSH AsConfig::Brush_White  = CreateSolidBrush(RGB(255, 255, 255) );
const HBRUSH AsConfig::Brush_Gray  = CreateSolidBrush(RGB(125, 125, 125) );
const HBRUSH AsConfig::Brush_Green = CreateSolidBrush(RGB(0, 255, 0) );
const HBRUSH AsConfig::Brush_Green_Dark = CreateSolidBrush(RGB(80, 146, 56) );
const HBRUSH AsConfig::Brush_Background = CreateSolidBrush(RGB(30, 30, 30) );
//------------------------------------------------------------------------------------------------------------
AsConfig::~AsConfig()
{
	DeleteObject(Brush_White);
	DeleteObject(Brush_Gray);
	DeleteObject(Brush_Green);
	DeleteObject(Brush_Green_Dark);
	DeleteObject(Brush_Background);
}
//------------------------------------------------------------------------------------------------------------
AsConfig::AsConfig()
{
	//Load_From_Config();

	Window_Rect.left = 0;
	Window_Rect.top = 0;
	Window_Rect.right = Window_Width;
	Window_Rect.bottom = Window_Height;
}
//------------------------------------------------------------------------------------------------------------
void AsConfig::Load_From_Config()
{
   //EConfig_Params config_params = EConfig_Params::ECP_Standart_Background;
   //char ch;
   //int i = 0;
   //int* brush = new int[3] {};
   //char arr[Settings_Size][64]{};
   //int yy = 0;
   //std::ifstream load_from_file("Config.txt");
   //if (!load_from_file.is_open())
   //   return;

   //while (load_from_file.get(ch))
   //{
   //   if (ch >= '0' && ch <= '9')
   //   {
   //      yy *= 10;
   //      yy += ch - '0';
   //   }
   //   else if (ch == ',' || ch == ';')
   //   {
   //      brush[i++] = yy;
   //      yy = 0;
   //   }

   //   if (ch == ';')
   //   {
   //      i = 0;

   //      switch (config_params)
   //      {
   //      case EConfig_Params::ECP_Standart_Background:
   //         Standart_Background = CreateSolidBrush(RGB(brush[0], brush[1], brush[2]));
   //         config_params = EConfig_Params::ECP_Coordinates;
   //         break;

   //      case EConfig_Params::ECP_Coordinates:
   //         Standart_Background = CreateSolidBrush(RGB(brush[0], brush[1], brush[2]));
   //         config_params = EConfig_Params::ECP_Something_Else;
   //         break;

   //      case EConfig_Params::ECP_Something_Else:
   //         break;

   //      default:
   //         int yy = 0;
   //         break;
   //      }
   //   }
   //}
   //delete[] brush;
}
//------------------------------------------------------------------------------------------------------------
