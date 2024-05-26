#include "Main.h"

AsMain *AsMain::Main_Window = 0;

// API_ENTRY
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hi_prev, _In_ LPWSTR ptr_cmd, _In_ int cmd_int)
{
	AsMain::Main_Window = AsMain::Get_Instance(hinstance);
	return AsMain::Main_Window->Get_WParam();
}
//------------------------------------------------------------------------------------------------------------




// AsMain
bool AsMain::Temp = false;
AsMain::AsMain(HINSTANCE handle_instance)
	:HInstance(handle_instance)
{
	AsConfig::Load_From_Config();  // !!! Load background and else from config

	LoadStringW(HInstance, IDS_APP_TITLE, SZ_Title, AsConfig::Max_Loadstring);  // from IDS_APP_TITLE get string and set to SZ_TITLE if < MAX_LOADSTRING
	LoadStringW(HInstance, IDC_ASAVETITLE, SZ_Window, AsConfig::Max_Loadstring);
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
AsMain *AsMain::Get_Instance(HINSTANCE handle_instance)
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

	hWnd = CreateWindowW(SZ_Window, SZ_Title, WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, window_size.left, window_size.top, window_size.right, window_size.bottom, 0, 0, HInstance, 0);
	if (!hWnd)
		return false;

	AdjustWindowRect(&AsConfig::Window_Rect, WS_OVERLAPPEDWINDOW, TRUE);
	ShowWindow(hWnd, SW_NORMAL);
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
		break;

	case WM_PAINT:
	{
		if (AsMain::Main_Window->Temp)  // Need to create hWnd
			AsMain::Main_Window->Engine.Draw_Frame(hWnd);
		else
			AsMain::Main_Window->Temp = true;
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
		AsMain::Main_Window->Engine.UI_Builder->Set_User_Input(text);  // if press Enter(true) Redraw_Border || it`s for sort in future, but if don`t need we can just draw from next line
	}
	break;


	case WM_KEYDOWN:
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
