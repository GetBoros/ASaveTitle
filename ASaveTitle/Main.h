#pragma once

#include "Dictionary.h"
#include "Engine.h"

// AsMain
class AsMain
{
	AsMain(HINSTANCE handle_instance);

public:
	AsMain(AsMain &main_window) = delete;
	void operator=(const AsMain &) = delete;
	
	int Get_WParam();

	static AsMain *Set_Instance(HINSTANCE handle_instance);
	static AsMain *Main_Window;

private:

	bool Init_Instance();
	ATOM Register_Class();
	
	// TEMP
	RECT *Rect_Programs;

	static WCHAR SZ_Title[AsConfig::Max_Loadstring];
	static WCHAR SZ_Window[AsConfig::Max_Loadstring];
	AsEngine Engine;
	HINSTANCE HInstance;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static bool Is_Hwnd_Created;
	static EProgram Programs;
};
//------------------------------------------------------------------------------------------------------------
