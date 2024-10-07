#pragma region Hello
/*
	Hello
*/
#pragma endregion

#include "stdafx.h"
#include "Main.h"

//------------------------------------------------------------------------------------------------------------
void init(SOCKET &socket_to_server, struct sockaddr_in &address_server)
{
	int result;
	WSADATA wsocket_data;

	result = 0;
	address_server = {};  // Инициализируем структуру адреса

	if (WSAStartup(MAKEWORD(2, 2), &wsocket_data) )
		return;
	socket_to_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 1.1. Создаем сокет и задаем адрес сервера
	if (socket_to_server == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	address_server.sin_family = AF_INET;
	address_server.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP-адрес сервера
	address_server.sin_port = htons(666);  // Порт сервера
}
//------------------------------------------------------------------------------------------------------------
int connect_to_server()
{
	SOCKET socket_to_server;
	struct sockaddr_in address_server;
	unsigned long long data_to_send[5] {}; // Данные, которые отправляем на сервер
	unsigned char buffer_to_server[sizeof(data_to_send)] = {};

	for (size_t i = 0; i < 5; ++i) {
		data_to_send[i] = 999999ULL; // Пример заполнения массива
	}

	memcpy(buffer_to_server, data_to_send, sizeof(data_to_send) );  // Копируем данные в буфер для отправки
	init(socket_to_server, address_server);

	// 1.2. Подключаемся к серверу
	if (connect(socket_to_server, (sockaddr *) &address_server, sizeof(address_server) ) == SOCKET_ERROR)  // can`t connect server
	{
		closesocket(socket_to_server);
		WSACleanup();
		return 1;
	}

	// 1.3. Отправляем данные на сервер
	if (send(socket_to_server, (char *) &buffer_to_server, sizeof(buffer_to_server), 0) == SOCKET_ERROR)  // send data to server
	{
		closesocket(socket_to_server);
		WSACleanup();
		return 1;
	}

	closesocket(socket_to_server);
	WSACleanup();

	return 0;
}
//------------------------------------------------------------------------------------------------------------
void func()
{
	wchar_t *found = 0;
	wchar_t str[] = L"Я стал самым сильным с провальным навыком «ненормальное состояние», я разрушу всё 12";
	const wchar_t wrong[] = L"«»:";

	while ( (found = wcspbrk(str, wrong) ) != 0)
		while ( *found != '\0')
			( *found++ = found[1]);  // set next wchar to empty space after found++(it`s last low prior)
}
//------------------------------------------------------------------------------------------------------------




// API_ENTRY
int APIENTRY wWinMain(_In_ HINSTANCE hinstance, _In_opt_ HINSTANCE hi_prev, _In_ LPWSTR ptr_cmd, _In_ int cmd_int)
{
	AsExamples examples(EShow_Preview::EP_Show_Constexpr_Examples);  // send data to server

	if (false)  // Testing
	{
		char *test = 0;
		AsTools::Format_Wide_Char_To_Char(ptr_cmd, test);  // convert to char

		AFFmpeg_Task ffmpeg_task(test);
		ffmpeg_task.Init();
		ffmpeg_task.Show_Info();
		
		delete[] test;
		return 0;
	}
	else
		return AsMain::Set_Instance(hinstance)->Get_WParam();  // Bad, but why not
}
//------------------------------------------------------------------------------------------------------------




// AsMain
AsMain *AsMain::Main_Window = 0;  // Singlton
bool AsMain::Is_Hwnd_Created = false;
EProgram AsMain::Programs = EProgram::Invalid;
WCHAR AsMain::SZ_Title[] = L"ASaver";
WCHAR AsMain::SZ_Window[] = L"Book_Reader";
//------------------------------------------------------------------------------------------------------------
AsMain::AsMain(HINSTANCE handle_instance)
	:HInstance(handle_instance)
{
	Main_Window = this;
	setlocale(LC_ALL, "ru_RU.UTF-8");
	std::lconv *st = std::localeconv();

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

	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);

	window_size.left = 0;
	window_size.top = 0;
	window_size.right = screen_width;
	window_size.bottom = screen_height - 20;

	hWnd = CreateWindowExW( 0, SZ_Window, 0, WS_OVERLAPPEDWINDOW, window_size.left, window_size.top, window_size.right - window_size.left, window_size.bottom - window_size.top, 0, 0, HInstance, 0);
	if (!hWnd)
		return false;

	AsConfig::Hwnd = hWnd;

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
			//AsMain::Main_Window->Engine.Draw_Frame_AEmpty(hWnd);
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
		Main_Window->Engine.~AsEngine();
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
