﻿#include "Main.h"

// Singlton
AsMain *AsMain::Main_Window = 0;
//------------------------------------------------------------------------------------------------------------




enum EConvert  // !!!
{
	EChar_Rus_Beg = 1040,  // 1040(А) - 68(40 + 28)
	EChar_Rus_End = 1071,  // 1071(Я) - 99(71 + 28)
	ENum_Beg = 39,  // 0(48)
	ENum_End = 48,  // 9(57)
	ENum_Test_0 = 49,  // :(58)
	ENum_Test_4 = 50,  // !(33)
	ENum_Test_1 = 51,  // "(34)
	ENum_Test_2 = 52,  // '(39)
	ENum_Test_3_0 = 53,  // ,(44) -(45) .(46)
	ENum_Test_3_3 = 56,  // /(47)
	ENum_Test_5 = 57,  // ?(63)
	ENum_Test_6 = 58  // ?(63)

};
unsigned short Convert_Rus(unsigned short &ch)
{
	if (ch >= 1025 && ch <= 1103)
	{
		// 1.0. Rus Symbols
		if (ch >= 1072 && ch <= 1103)  // а - я
			return ch = (ch - 1000) - 30 - 32;  // if a = 10 | я = 41

		if (ch >= 1040 && ch <= 1071)  // А - Я
			return ch = (ch - 1000) - 30;    // if A = 10 | Я = 41

		if (ch == 1105)  // ё
			return ch = (ch - 1000) - 30 - 32 - 1;  // ё = 42

		if (ch == 1025)  // Ё
			return ch = (ch - 1000) + 17;  // ё = 42
	}
	else if (ch >= 65 && ch <= 122)
	{
		// 3.0 English Symbols
		if (ch >= 97 && ch <= 122)  // а = 97 || z = 122
			return ch = ch - 24;  // a = 73 | z = 98

		if (ch >= 65 && ch <= 90)  // A = 65 Z = 90  | 25 
			return ch = ch + 8;  // A = 73 | Z = 98

		if (ch == 96)  // `
			return ch = (ch - 24);  // ` = 72
	}
	else
	{
		// 2.0. Standart symbols || Les than < 100
		if (ch >= 48 && ch <= 59)  // 0 - 9 - :
			return ch = (ch - 5);  // 0 = 43 | 9 = 52 | : = 53 | ; = 54

		if (ch == 63)  // ?
			return ch = (ch - 8);  // ? = 55

		if (ch >= 32 && ch <= 47)  // space || ! " # ( ) * + , - . / $ %
			return ch = (ch + 24);  // space = 56 || / = 71
	}
	return 0;  // Reserver for 99
}



// API_ENTRY
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hi_prev, _In_ LPWSTR ptr_cmd, _In_ int cmd_int)
{
	//int index = 0;
	//unsigned short yy = 0;
	//wchar_t word[] = L"ФZ3zAZнкарнация безработного: История о приключениях в другом мире 2.2";

	//while (index < 100)
	//{
	//	unsigned short w_ch = (unsigned short)word[index];
	//	yy = Convert_Rus(w_ch);

	//	yy++;
	//	index++;
	//}

	//return 0;
	AsMain::Main_Window = AsMain::Set_Instance(hinstance);
	return AsMain::Main_Window->Get_WParam();
}
//------------------------------------------------------------------------------------------------------------




// AsMain
bool AsMain::Is_Hwnd_Created = false;
EPrograms AsMain::Programs = EPrograms::ASaver;
WCHAR AsMain::SZ_Title[] = L"ASaver";
WCHAR AsMain::SZ_Window[] = L"Book_Reader";
//------------------------------------------------------------------------------------------------------------
AsMain::AsMain(HINSTANCE handle_instance)
	:HInstance(handle_instance)
{
	LoadStringW(HInstance, IDC_ASAVETITLE, SZ_Window, AsConfig::Max_Loadstring);
	LoadStringW(HInstance, IDS_APP_TITLE, SZ_Title, AsConfig::Max_Loadstring);  // from IDS_APP_TITLE get string and set to SZ_TITLE if < MAX_LOADSTRING
	Register_Class();
	if (!Init_Instance() )
		return;
}
//------------------------------------------------------------------------------------------------------------
int AsMain::Get_WParam()
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
	if (Main_Window == 0)
		Main_Window = new AsMain(handle_instance);

	return Main_Window;
}
//------------------------------------------------------------------------------------------------------------
bool AsMain::Init_Instance()
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
ATOM AsMain::Register_Class()
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
	wcex.hbrBackground = AsConfig::Brush_Background;
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
	{
		int i;

		if (AsMain::Main_Window == 0)
			return !InvalidateRect(hWnd, 0, FALSE);  // if Main Window don`t created

		switch (Programs)
		{
		case EPrograms::Invalid:
		{
			const int round = 75;
			const int x = 100;
			const int width = 450;
			const int height = 750;
			const RECT rect_programs{ x, x, width, height };
			RECT rect_intersecte{};
			PAINTSTRUCT paint_struct;
			HDC hdc;

			// if intersect load program
			if (AsMain::Main_Window != 0)
			{
				const int cord_mouse_x = AsMain::Main_Window->Engine.LM_Cord_X;
				const RECT rect_mouse{ cord_mouse_x - 1, cord_mouse_x - 1, cord_mouse_x + 1, cord_mouse_x + 1 };
				rect_intersecte = rect_programs;

				for (i = 0; i < (int)EPrograms::End; i++)
				{
					const int width_offset = i * width;
					
					rect_intersecte.left += width_offset;
					rect_intersecte.right += width_offset;

					if (IntersectRect(&rect_intersecte, &rect_intersecte, &rect_mouse) )
					{
						Programs = (EPrograms)i;
						InvalidateRect(hWnd, 0, TRUE);
						return 0;
					}
				}
			}

			// Draw Main Menu
			hdc = BeginPaint(hWnd, &paint_struct);
			
			for (i = 0; i < (int)EPrograms::End; i++)
			{
				RoundRect(hdc, rect_programs.left + (i * width), rect_programs.top, rect_programs.right + (i * width), rect_programs.bottom, round, round);
				TextOutW(hdc, rect_programs.left + (i * width) + (width / 3), rect_programs.top + 70, AsConfig::Text_Program_Names[i], (int)wcslen(AsConfig::Text_Program_Names[i]) );
			}

			EndPaint(hWnd, &paint_struct);
		}
			break;


		case EPrograms::ASaver:
			AsMain::Main_Window->Engine.Draw_Frame(hWnd);
			break;


		case EPrograms::ABook_Reader:
			AsMain::Main_Window->Engine.Draw_Frame_Book_Reader(hWnd);
			break;


		case EPrograms::End:
			break;


		default:
			break;
		}
	}
	break;

	case WM_CHAR:  // wParam get char
	{
		wchar_t text = static_cast<wchar_t>(wParam);

		if (text == '\r')  // if not enter
			return 0;

		AsMain::Main_Window->Engine.Key_Type = EKT_Redraw_User_Input;
		AsMain::Main_Window->Engine.Redraw_Frame();  // Call WM_PAINT to redraw

		if (GetKeyState(VK_CONTROL) & 0x8000)  // pressed not with ctrl
		{
			if (wParam == 22)  // if pressed ctrl + v get from clipboard
			{
				AsMain::Main_Window->Engine.Get_Clipboard_From_Else();
				break;
			}
		}
		if (AsMain::Main_Window->Engine.UI_Builder != 0)
			AsMain::Main_Window->Engine.UI_Builder->Set_User_Input(text);  // if press Enter(true) Redraw_Border || it`s for sort in future, but if don`t need we can just draw from next line
	}
	break;


	case WM_KEYDOWN:
	{
		switch (wParam)
		{

		case VK_RIGHT:
		case VK_LEFT:
		case VK_UP:
		case VK_DOWN:
		case VK_SPACE:
			return 0;

		case VK_RETURN:
			InvalidateRect(hWnd, 0, FALSE);  // set messege WM_PAINT
			AsMain::Main_Window->Engine.Key_Type = EKT_Enter;
			return 0;

		default:
			break;
		}
	}
	break;


	case WM_LBUTTONDOWN:
	{
		AsMain::Main_Window->Engine.LM_Cord_X = lParam & 0xffff;  // 0x5095
		AsMain::Main_Window->Engine.LM_Cord_Y = (int)(lParam >> 16);
		AsMain::Main_Window->Engine.Key_Type = EKT_LM_Down;

		InvalidateRect(hWnd, 0, FALSE);  // set messege WM_PAINT
	}
	break;


	case WM_RBUTTONDOWN:
	{
		AsMain::Main_Window->Engine.LM_Cord_X = lParam & 0xffff;  // 0x5095
		AsMain::Main_Window->Engine.LM_Cord_Y = (int)(lParam >> 16);
		AsMain::Main_Window->Engine.Key_Type = EKT_RM_Down;

		InvalidateRect(hWnd, 0, FALSE);
	}
	break;


	//case WM_TIMER:  // нужен для постепенного обновления
//{
//	if (wParam == Timer_ID)  // 20 frames per 1 second
//		return AsMain::Main_Window->Engine.On_Timer();
//}
//break;


	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_SETTINGS:  // My, ctrl alt f do delete all
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;


		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	case WM_SIZE:
		if (wParam == SIZE_MAXIMIZED)
			if (AsMain::Main_Window != 0)
				AsMain::Main_Window->Engine.Is_After_Maximazied = false;
			
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
