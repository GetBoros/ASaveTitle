#include "Main.h"

#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

AsMain *AsMain::Main_Window = 0;  // Singlton
//------------------------------------------------------------------------------------------------------------




//------------------------------------------------------------------------------------------------------------
void DecodeHTML_qwe()
{
	/*
		- <picture>  // start save file from here
		- image start	= <img src="
		- image end		= " alt>
		- 122 - 180
		// 2 Варианта декодирувать русс, или оставить на английском, или оба?
		- title bgn		= ; / 
		- title end		= </h3>
		- title num bgn= </span>
<span>
		- title num end=  
		- Done
	*/
}
//------------------------------------------------------------------------------------------------------------




// API_ENTRY
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hi_prev, _In_ LPWSTR ptr_cmd, _In_ int cmd_int)
{

	// Tutorial
	/*AsExamples *examples = new AsExamples(EShow_Preview::EP_Show_Html_Decode);*/

	return AsMain::Set_Instance(hinstance)->Get_WParam();  // Bad, but why not
}
//------------------------------------------------------------------------------------------------------------




// AsMain
bool AsMain::Is_Hwnd_Created = false;
EProgram AsMain::Programs = EProgram::Invalid;
WCHAR AsMain::SZ_Title[] = L"ASaver";
WCHAR AsMain::SZ_Window[] = L"Book_Reader";
//------------------------------------------------------------------------------------------------------------
AsMain::AsMain(HINSTANCE handle_instance)
	:HInstance(handle_instance)
{
	setlocale(LC_ALL, "ru_RU.UTF-8");

	LoadStringW(HInstance, IDC_ASAVETITLE, SZ_Window, AsConfig::Max_Loadstring);
	LoadStringW(HInstance, IDS_APP_TITLE, SZ_Title, AsConfig::Max_Loadstring);  // from IDS_APP_TITLE get string and set to SZ_TITLE if < MAX_LOADSTRING
	Register_Class();
	if (!Init_Instance() )
		return;
}
//------------------------------------------------------------------------------------------------------------
int AsMain::Get_WParam() const
{
	MSG msg;
	HACCEL handl_accel;

	handl_accel = LoadAccelerators(HInstance, MAKEINTRESOURCE(IDC_ASAVETITLE) );
	while (GetMessage(&msg, 0, 0, 0) )
	{
		if (!TranslateAccelerator(msg.hwnd, handl_accel, &msg) )
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}
//------------------------------------------------------------------------------------------------------------
AsMain *AsMain::Set_Instance(HINSTANCE handle_instance)
{
	if (Main_Window != 0)
		return Main_Window;
	else
		return Main_Window = new AsMain(handle_instance);
}
//------------------------------------------------------------------------------------------------------------
bool AsMain::Init_Instance() const
{
	RECT window_size{};
	HWND hWnd;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	window_size.left = 0;
	window_size.top = 0;
	window_size.right = screenWidth;
	window_size.bottom = screenHeight - 20;

	hWnd = CreateWindowExW( 0, SZ_Window, 0, WS_OVERLAPPEDWINDOW, window_size.left, window_size.top, window_size.right - window_size.left, window_size.bottom - window_size.top, 0, 0, HInstance, 0);
	if (!hWnd)
		return false;

	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);

	return true;
}
//------------------------------------------------------------------------------------------------------------
ATOM AsMain::Register_Class() const
{
	WNDCLASSEXW wcex {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = &AsMain::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = HInstance;
	wcex.hIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_ASAVETITLE) );
	wcex.hCursor = LoadCursor(0, IDC_ARROW);
	wcex.hbrBackground = AsConfig::Brush_Background_Dark;
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ASAVETITLE);
	wcex.lpszClassName = SZ_Window;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL) );

	return RegisterClassExW(&wcex);
}
//------------------------------------------------------------------------------------------------------------
LRESULT AsMain::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		SetMenu(hWnd, 0);
		break;


	case WM_PAINT:
		if (AsMain::Main_Window != 0)
			AsMain::Main_Window->Engine.Draw_Frame_ASaver(hWnd);
		else
			return !InvalidateRect(hWnd, 0, FALSE);  // if Main Window don`t created
		break;


	case WM_CHAR:
		AsMain::Main_Window->Engine.Redraw_Frame(EUI_Builder_Handler::EBH_UI_Button, wParam, lParam);  // if Enter
		break;


	case WM_LBUTTONDOWN:
		AsMain::Main_Window->Engine.Redraw_Frame(EUI_Builder_Handler::EBH_LM_Button, wParam, lParam);
		break;


	case WM_RBUTTONDOWN:
		AsMain::Main_Window->Engine.Redraw_Frame(EUI_Builder_Handler::EBH_RM_Button, wParam, lParam);
		break;


	case WM_COMMAND:
		switch (const int wmId = LOWORD(wParam) )
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_SETTINGS:  // My, ctrl alt f do delete all
			DialogBox(GetModuleHandle(0), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_SIZE:
		if (wParam == SIZE_MAXIMIZED)
			int yy = 0;
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
//------------------------------------------------------------------------------------------------------------
INT_PTR AsMain::About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
//------------------------------------------------------------------------------------------------------------
