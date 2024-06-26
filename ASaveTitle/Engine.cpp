﻿#include "Engine.h"

// ACurl_Client
std::wstring ACurl_Client::Response_Buffer;
//------------------------------------------------------------------------------------------------------------
ACurl_Client::~ACurl_Client()
{//Tear Dowm

	// Last Cleanuping & EASY
	curl_easy_cleanup(Url_Easy);
	curl_global_cleanup();
}
//------------------------------------------------------------------------------------------------------------
ACurl_Client::ACurl_Client(const EPrograms &program, wchar_t *user_input)
 : Url_Easy(0), Response{}
{// Setup

	switch (program)
	{
	case EPrograms::Invalid:
		break;
	case EPrograms::ASaver:
	{
		if (user_input != 0)
			Handle_Saver_URL(user_input);  // Anime-bit
		else
			Saver_Update();
	}
		break;

	case EPrograms::ABook_Reader:
		break;

	case EPrograms::End:
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Handle_Saver_URL(wchar_t *user_input)
{
	char *url;
	int size;
	wchar_t **url_pair;
	void *user_input_void;
	FILE *file;

	url_pair = new wchar_t*[2];
	user_input_void = static_cast<void*>(url_pair);
	size = WideCharToMultiByte(CP_UTF8, 0, user_input, -1, 0, 0, 0, 0);
	url = new char[size];
	WideCharToMultiByte(CP_UTF8, 0, user_input, -1, url, size, 0, 0);

	// INIT CURL
	curl_global_init(CURL_GLOBAL_DEFAULT);
	Url_Easy = curl_easy_init();

	// 1.  First Responds set Options | Maybe Go to function
	/*
	Need handle options what to do sticky
	- SETOPT | CURLOPT:	
		Independent || Order - Independent | Options set copy data | CURLOPT_URL is MANDATORY OPT | Download is default actions |
		Timeouts | name resolve alternatives | connectivity | protocol | TLS | authentication | proxies | receive | 
	*/

	curl_easy_setopt(Url_Easy, CURLOPT_LOW_SPEED_TIME, 60L);  // 60 sec
	curl_easy_setopt(Url_Easy, CURLOPT_LOW_SPEED_LIMIT, 30L);  // Abort if slower that 30 bytes/sec per seconds above
	curl_easy_setopt(Url_Easy, CURLOPT_URL, url);
	curl_easy_setopt(Url_Easy, CURLOPT_VERBOSE, 1L);  // 1L means a one long long type
	curl_easy_setopt(Url_Easy, CURLOPT_WRITEFUNCTION, Write_Callback);
	curl_easy_setopt(Url_Easy, CURLOPT_WRITEDATA, user_input_void);

	Response = curl_easy_perform(Url_Easy);

	// 2. Second responds save to Folder picture Temporary
	delete[] url;
	errno_t err = fopen_s(&file, "Pictures/Temporary.png", "wb");  // save as Temporary.png
	if (err == 0 && file != 0)
	{
		size = WideCharToMultiByte(CP_UTF8, 0, url_pair[0], -1, 0, 0, 0, 0);
		url = new char[size];
		WideCharToMultiByte(CP_UTF8, 0, url_pair[0], -1, url, size, 0, 0);

		curl_easy_setopt(Url_Easy, CURLOPT_URL, url);  // save img
		curl_easy_setopt(Url_Easy, CURLOPT_WRITEFUNCTION, Save_Img);
		curl_easy_setopt(Url_Easy, CURLOPT_WRITEDATA, file);

		Response = curl_easy_perform(Url_Easy);  // Download image to file

		delete[] url;
		fclose(file);
	}
	
	wcscpy_s(user_input, wcslen(url_pair[1]) + 1, url_pair[1]);
	delete url_pair[0];
	delete url_pair[1];
	delete[] url_pair;

}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Saver_Update()
{
	char url[] = "https://anime-bit.ru/";
	wchar_t **url_pair;
	void *void_ptr;

	url_pair = new wchar_t *[2] { };
	void_ptr = static_cast<void *>(url_pair);

	// INIT CURL
	curl_global_init(CURL_GLOBAL_DEFAULT);
	Url_Easy = curl_easy_init();

	// SETTINGS
	curl_easy_setopt(Url_Easy, CURLOPT_LOW_SPEED_TIME, 60L);  // 60 sec
	curl_easy_setopt(Url_Easy, CURLOPT_LOW_SPEED_LIMIT, 30L);  // Abort if slower that 30 bytes/sec per seconds above
	curl_easy_setopt(Url_Easy, CURLOPT_URL, url);

	// HOW TO RESPONSE
	curl_easy_setopt(Url_Easy, CURLOPT_WRITEFUNCTION, Write_Callback_Update);
	curl_easy_setopt(Url_Easy, CURLOPT_WRITEDATA, void_ptr);

	// FINAL
	Response = curl_easy_perform(Url_Easy);
}
//------------------------------------------------------------------------------------------------------------
size_t ACurl_Client::Write_Callback_Update(void *contents, size_t size, size_t nmemb, void *void_ptr)
{// Get a part of bytes we can handle(write to file)
	const wchar_t *pattern_title_name_begin = L"<img alt='&laquo;";
	const wchar_t *pattern_title_name_end = L"&raquo";
	const wchar_t *pattern_title_last_add = L"<div>Добавленно:";
	wchar_t **user_input = static_cast<wchar_t**>(void_ptr);
	int wideStringLength;
	size_t total_size;

	// 1. Get Content from URL to Responce Buffer
	total_size = size * nmemb;  // !!! Doesn`t work All down code
	wideStringLength = MultiByteToWideChar(CP_UTF8, 0, (char*)contents, -1, 0, 0);  // Вычисляем размер буфера, необходимый для конвертации

	if (wideStringLength == 0)  // Обработка ошибки, если MultiByteToWideChar вернула 0
		return 0;
	else
		Response_Buffer.resize(wideStringLength);  //

	if (MultiByteToWideChar(CP_UTF8, 0, (char*)contents, -1, &Response_Buffer[0], wideStringLength) == 0)  // Conver ch -> wchar_t
		return 0;  // Обработка ошибки, если MultiByteToWideChar вернула 0
	else
		Response_Buffer.pop_back();  // Удаляем завершающий нулевой символ
	
	// 1.1 Find new Add Title with seasons
	if (const wchar_t* pattern_begining = wcsstr(Response_Buffer.c_str(), pattern_title_name_begin))
	{
		const wchar_t* ptr_end = wcsstr(pattern_begining, pattern_title_name_end);
		const wchar_t* ptr_beg = pattern_begining + wcslen(pattern_title_name_begin);
		const size_t pattern_title_length = ptr_end - ptr_beg;

		user_input[0] = new wchar_t[64];  // !!!
		wcsncpy_s(user_input[0], 63, ptr_beg, pattern_title_length);  // cpy title name
	}

	// 1.2 Find last added date
	if (const wchar_t *pattern_begining = wcsstr(Response_Buffer.c_str(), pattern_title_last_add) )
	{
		const wchar_t *ptr_end = wcsstr(pattern_begining, L"</div>");
		const wchar_t *ptr_beg = pattern_begining + wcslen(pattern_title_last_add);
		const __int64 length = ptr_end - ptr_beg;

		wcsncpy_s(user_input[0] + wcslen(user_input[0]), 63, ptr_beg, (int)length);  // cpy data added

		if (!ptr_end != 0)
			return 0;

		const wchar_t *ptr_beg_01 = wcsstr(ptr_end, L" Серии: [");
		const wchar_t *ptr_end_01 = wcsstr(ptr_end, L"]</div>") + 1;
		__int64 length_01 = ptr_end_01 - ptr_beg_01;

		if (ptr_beg_01 != 0)
			wcsncpy_s(user_input[0] + wcslen(user_input[0]), 63, ptr_beg_01, (int)length_01); // cpy series
	}
	return total_size;  // return bytes we have deal with i
}
//-----------------------------------------------------------------------------------------------------------
size_t ACurl_Client::Write_Callback(void *contents, size_t size, size_t nmemb, void *userp)
{// Get a part of bytes we can handle(write to file)
	bool is_image;
	const wchar_t *pattern_img = L"image_src";
	const wchar_t *pattern_htt = L"https";
	const wchar_t *pattern_ttl = L"<h1>&laquo;";
	const wchar_t *pattern_num = L": [";
	wchar_t **user_input = static_cast<wchar_t**>(userp);
	int wideStringLength;
	size_t total_size;
	unsigned long long ll_ = 17999999999999999999ULL;

	is_image = false;
	total_size = size * nmemb;
	wideStringLength = MultiByteToWideChar(CP_UTF8, 0, (char*)contents, -1, 0, 0);  // Вычисляем размер буфера, необходимый для конвертации

	if (wideStringLength == 0)  // Обработка ошибки, если MultiByteToWideChar вернула 0
		return 0;
	else
		Response_Buffer.resize(wideStringLength);  //

	if (MultiByteToWideChar(CP_UTF8, 0, (char*)contents, -1, &Response_Buffer[0], wideStringLength) == 0)  // Conver ch -> wchar_t
		return 0;  // Обработка ошибки, если MultiByteToWideChar вернула 0
	else
		Response_Buffer.pop_back();  // Удаляем завершающий нулевой символ
	
	// 1. Получить httl картинки и сохранить её
	if (const wchar_t *title_img_ptr = wcsstr(Response_Buffer.c_str(), pattern_img) )
		AsTools::Format_Text_Using_Patterns(title_img_ptr, pattern_htt, L"\"", &user_input[0]);

	// 2. Get title + seasons + nums + find & change invalid chars
	const wchar_t *title_num_ptr = wcsstr(Response_Buffer.c_str(), pattern_ttl);  // get ptr at pattern_ttl begin
	if (title_num_ptr != 0 && *(title_num_ptr + 1) != L'\0')
	{// !!! Refactoring waiting this moment
		wchar_t *title_name;
		wchar_t curr_char;  // Write title | write seasons
		int pattern_length;
		int i;
		int buffer_for_nums;
		int invalid_char_len;

		pattern_length = (int)wcslen(pattern_ttl);
		i = pattern_length;
		curr_char = title_num_ptr[i];
		buffer_for_nums = 6;
		invalid_char_len = AsConfig::Invalid_Chars_Len;

		// 2.0. Find last title element and cpy to user_input
		while (curr_char != L'&')
			curr_char = title_num_ptr[i++];

		i = i - pattern_length + buffer_for_nums;
		user_input[1] = new wchar_t[--i] {};
		buffer_for_nums = i - buffer_for_nums;
		

		AsTools::Format_Text_Using_Patterns(title_num_ptr, title_num_ptr + pattern_length, L"&raquo;", &title_name);
		wcsncpy_s(user_input[1], wcslen(title_name) + 1, title_name, wcslen(title_name) );
		delete title_name;

		// 2.1. Change Invalid chars
		while (--invalid_char_len != -1)
		{
			if (const wchar_t *find_invalid_char = wcsstr(user_input[1], AsConfig::Invalid_Chars[invalid_char_len] ) )
				user_input[1][(int)(find_invalid_char - user_input[1] ) ] = AsConfig::Invalid_Chars_Valid[invalid_char_len];
		}

		// 2.2. Change Nums to dots, need to save load correctly
		while (++invalid_char_len < buffer_for_nums - 2)  // - 2 need for seasons
		{
			curr_char = user_input[1][invalid_char_len];

			if (curr_char == L'«')
				user_input[1][invalid_char_len] = L' ';

			if (curr_char <= L'9' && curr_char >= L'0')
				user_input[1][invalid_char_len] = L' ';

			if (invalid_char_len > 1 && curr_char < L'а' && curr_char >= L'А')
				user_input[1][invalid_char_len] = curr_char += L' ';
		}

		// 2.3. Get Numbers to title
		if (const wchar_t *title_num = wcsstr(title_num_ptr, pattern_num) )
		{
			int pattern_len = (int)wcslen(L": [");
			int index = pattern_len;
			int title_length = (int)wcslen(user_input[1]);

			user_input[1][title_length] = L' ';

			while (title_num[index] != L' ')
				user_input[1][++title_length] = title_num[index++];

			user_input[1][++title_length] = L'\0';
			return 0;
		}
	}

	if (!is_image)
		return total_size;  // return bytes we have deal with i
	else
		return fwrite(contents, size, nmemb, (FILE*)userp);   // if write img to file
}
//-----------------------------------------------------------------------------------------------------------
size_t ACurl_Client::Save_Img(void *contents, size_t size, size_t nmemb, FILE *userp)
{
	size_t written = fwrite(contents, size, nmemb, userp);

	return written;
}
//-----------------------------------------------------------------------------------------------------------




// ACurl_Component
ACurl_Component::~ACurl_Component()
{
	delete[] ID_Content_Array;
}
//------------------------------------------------------------------------------------------------------------
ACurl_Component::ACurl_Component()
 : ID_Content(0), ID_Content_Size(0), ID_Content_Array(0), Site{}
{
	Load_ID_Content();
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Component::Set_W_Url(const wchar_t *url)
{
	size_t len;
	size_t converted_chars;

	len = std::wcslen(url) + 1; // +1 для завершающего нулевого символа
	converted_chars = 0;
	std::vector<char> chars_buffer(len);

	wcstombs_s(&converted_chars, chars_buffer.data(), len, url, len - 1);  // Преобразуем широкую строку в многобайтовую строку

	Set_Content_ID(chars_buffer.data() );  // Передаем результат в Set_Url
}
//------------------------------------------------------------------------------------------------------------
bool ACurl_Component::Get_Url(wchar_t *user_input, const int &id_content_index)
{
	std::wstring url;
	
	if (id_content_index >= ID_Content_Size)
		return false;
	
	url = L"https://anime-bit.ru/content/" + std::to_wstring(ID_Content_Array[id_content_index]) + L"/";  // !!! Constant chang to dynamic
	const wchar_t *c_url = url.c_str();

	wcsncpy_s(user_input, wcslen(c_url) + 1, c_url, wcslen(c_url) );

	return true;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Component::Find_From_Patern(std::string &url, const char *start, const char *end)  // ~80 000
{
	size_t start_pos = url.find(start);
	size_t end_pos;
	if (start_pos != std::string::npos)
	{
		start_pos += std::strlen(start);
		end_pos = url.find(end, start_pos);
		if (end_pos != std::string::npos)
			url = url.substr(start_pos, end_pos - start_pos);
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Component::Set_Content_ID(const char* url)
{
	// 1.0 Receive Data from URL
	Find_From_Patern(Site = url, "/content/", "/");
	ID_Content = std::stoi(Site);

	Find_From_Patern(Site = url, "https://", ".ru/");

	// 1.1. Create Directory based on url 
	if (!std::filesystem::exists("Data/" + Site) )
		std::filesystem::create_directories("Data/" + Site);

	Site = "Data/" + Site + "/" + Site + ".bin";

	// 1.2 Add ID_Content
	Save_ID_Content();
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Component::Save_ID_Content()
{
	for (int i = 0; i < ID_Content_Size; ++i)  // Check the same value, if find exit from component
		if (ID_Content_Array[i] == ID_Content)
			return;  // !!! Don`t add if already exist

	ID_Content_Array[ID_Content_Size++] = ID_Content;

	std::ofstream outfile(Site, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile)
		return;

	for (int i = 0; i < ID_Content_Size; ++i)
		outfile.write(reinterpret_cast<const char*>(&ID_Content_Array[i]), sizeof(ID_Content_Array[i]));

	outfile.close();
	Load_ID_Content();
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Component::Load_ID_Content()
{
	int how_much_g;
	
	how_much_g = 0;
	delete[] ID_Content_Array;

	std::ifstream infile("Data/anime-bit/anime-bit.bin", std::ios::in | std::ios::binary);  // !!! change from const when first time
	if (infile)
	{
		infile.seekg(0, std::ios::end);
		how_much_g = (int)infile.tellg();

		ID_Content_Size = how_much_g / sizeof(int);

		infile.seekg(0, std::ios::beg);
		ID_Content_Array = new int [ID_Content_Size + 1] {};
		infile.read(reinterpret_cast<char*>(ID_Content_Array), how_much_g);
		infile.close();
	}
	else
		ID_Content_Array = new int[ID_Content_Size + 1] {};
}
//------------------------------------------------------------------------------------------------------------




// AsUI_Builder
int AsUI_Builder::User_Input_Len = 0;
int AsUI_Builder::Context_Button_Length = 5;
const wchar_t AsUI_Builder::Main_Menu_Title_Name[] = L"Сохранить запись";
const std::wstring AsUI_Builder::Button_Text_List[] = { L"Просмотреть", L"Просмотренные", L"Преостановленные", L"Добавить в желаемое", L"Удалить из списка", L"Выход из программы" };
const wchar_t *AsUI_Builder::Sub_Menu_Title = L"Вводите текст сюда... Или URL вашего Тайтла";
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::~AsUI_Builder()
{
	// 1.2 Save covered image sys
	if (Hdc_Memory != 0)
		DeleteDC(Hdc_Memory);
	if (H_Bitmap != 0)
		DeleteObject(H_Bitmap);

	// 1.3 Save map to Data/...
	Save_All_To_Data(Active_Menu = EAM_Exit);  // Exit from Program

	// 1.5 Free memory
	delete Curl_Component;

	delete[] Rect_User_Input_Change;
	delete[] Rect_Buttons_Context;
	delete[] Rect_Menu_List;
	delete[] User_Input_Rect;
}
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::AsUI_Builder(HDC hdc)
 : Active_Menu(EAM_Main), Active_Button(EActive_Button::EAB_Main_Menu), Rect_Menu_List{}, User_Input{}, Rect_Menu_List_Length(0), Rect_Sub_Menu_Length(0),
	Main_Menu_Titles_Length_Max(50), Curl_Component(0), Active_Page(EActive_Page::EAP_None), Sub_Menu_Curr_Page(0), Prev_Main_Menu_Button(0), Prev_Button(99), User_Input_Rect{}, Prev_Context_Menu_Cords{},
	Rect_Buttons_Context{}, Input_Button_Rect{}, Rect_Pages{}, Hdc_Memory(0), H_Bitmap(0), Saved_Object(0), Rect_User_Input_Change{}, Ptr_Hdc(hdc)
{
	// Load map from Data/...
	User_Input_Load(User_Array_Map, "Data/Watching.bin");
	User_Input_Load(User_Library_Map, "Data/Library.bin");
	User_Input_Load(User_Paused_Map, "Data/Paused.bin");
	User_Input_Load(User_Wishlist_Map, "Data/Wishlist.bin");

	Curl_Component = new ACurl_Component;

	Rect_User_Input_Change = new RECT[2]{};
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Init()
{
	int x, y;
	int str_length;
	int titles_length;
	RECT border_rect = {};
	
	Rect_Menu_List_Length = sizeof(Button_Text_List) / sizeof(Button_Text_List[0]);

	// 1. Init design
	SelectObject(Ptr_Hdc, AsConfig::Brush_Background);  // Dark Background
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);  // Dark Text Background
	SetTextColor(Ptr_Hdc, AsConfig::Color_Text_Green);  // Text Color

	// 1.1. Find longest titles
	titles_length = (int)wcslen(Main_Menu_Title_Name);

	// 1.2. Draw Border
	border_rect = Add_Border(0);

	// 2. Set Title in the middle of border
	str_length = AsConfig::Ch_W  *titles_length;  // ch width = 8 pixels * titles length in ch
	x = (border_rect.right - str_length) / 2;  // Get the center of first middle border and half of title
	y = border_rect.top + AsConfig::Global_Scale;  // Get offset from border top
	TextOutW(Ptr_Hdc, x, y, Main_Menu_Title_Name, titles_length);  // x = LT + Midle - h_title
	border_rect.top = y + AsConfig::Ch_H + AsConfig::Global_Scale;  // go to next line

	// 3. Set Buttons in border and save they`re cords in Rect_Menu_List
	Rect_Menu_List = new RECT[Rect_Menu_List_Length];

	for (int i = 0; i < Rect_Menu_List_Length; i++)
		Rect_Menu_List[i] = Add_Button(border_rect, Button_Text_List[i]);

	Draw_User_Title_Image(AsConfig::Main_Image_Folder);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Sub_Menu(const EActive_Menu &active_menu)
{
	int border_offset;
	RECT border_rect;
	std::wstring str_title = L"                                TITLE NAMES																								";

	// 1.1.Border Settings
	border_offset = (Main_Menu_Titles_Length_Max + AsConfig::Global_Scale) * AsConfig::Ch_W;  // where to draw next border
	border_rect = Add_Border(border_offset);  // draw border
	border_rect.top += AsConfig::Global_Scale;  // without title? i can fix but it`s look good enough
	Prev_Button = 99;  // Need to switch between arrays

	// 1.2. Draw Title
	SelectObject(Ptr_Hdc, AsConfig::Brush_Background);
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);
	SetTextColor(Ptr_Hdc, AsConfig::Color_Text_Green);
	Add_Button(border_rect, str_title);  // Draw Title

	// 1.3. Data Output to submenu
	Input_Button_Rect = Add_Button(border_rect, Sub_Menu_Title);

	if (active_menu != EActive_Menu::EAM_Main)
		Active_Menu = active_menu;

	switch (Active_Menu)
	{
	case EAM_Watching:
		Draw_User_Map(border_rect, User_Array_Map);
		break;
	
	
	case EAM_Library_Menu:
		Draw_User_Map(border_rect, User_Library_Map);
		break;


	case EAM_Paused_Menu:
		Draw_User_Map(border_rect, User_Paused_Map);
		break;


	case EAM_Wishlist:
		Draw_User_Map(border_rect, User_Wishlist_Map);
		break;


	case EAM_Exit:
		PostQuitMessage(0);
		break;


	case EAM_Main:
	case EAM_Erase:
	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Button_Advence(const EActive_Button &active_button)
{
	switch (Active_Menu)
	{
	case EAM_Main:
	case EAM_Watching:
		Redraw_Button(active_button, User_Array_Map);
		break;

	case EAM_Library_Menu:
		Redraw_Button(active_button, User_Library_Map);
		break;

	case EAM_Paused_Menu:
		Redraw_Button(active_button, User_Paused_Map);
		break;

	case EAM_Wishlist:
		Redraw_Button(active_button, User_Wishlist_Map);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Redraw_Button() const
{
	RECT button = Input_Button_Rect;

	SelectObject(Ptr_Hdc, AsConfig::Brush_Background);
	Rectangle(Ptr_Hdc, button.left, button.top, button.right, button.bottom);
	
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);
	SetTextColor(Ptr_Hdc, AsConfig::Color_Yellow);

	if (User_Input[0] != 0)
		TextOutW(Ptr_Hdc, button.left + AsConfig::Global_Scale, button.top + AsConfig::Global_Scale, User_Input, (int)wcslen(User_Input) );  // activ_button  text ( x its text out in middle
	else
		TextOutW(Ptr_Hdc, button.left + AsConfig::Global_Scale, button.top + AsConfig::Global_Scale, L"Enter Text Here...", 19);  // activ_button  text ( x its text out in middle
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Adjust(const bool is_increment)
{
	if (is_increment)
		It_Current_User->second.Title_Num++;
	else
		It_Current_User->second.Title_Num--;

	It_Current_User->second.Title_Name_Num.erase();

	if (It_Current_User->second.Title_Season != 0)
		It_Current_User->second.Title_Name_Num = It_Current_User->second.Title_Name_Key + L" " + AsConfig::Season[It_Current_User->second.Title_Season - 1] + L" " + std::to_wstring(It_Current_User->second.Title_Num);
	else
		It_Current_User->second.Title_Name_Num = It_Current_User->second.Title_Name_Key + L" " + std::to_wstring(It_Current_User->second.Title_Num);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Request()
{
	int button_offset;
	int box_size;
	int scale;
	int half_box;
	RECT ui_rect_offset {};

	button_offset = 9;
	box_size = 21;
	half_box = (int)( (float)box_size / 2.0f);
	scale = AsConfig::Global_Scale;

	// 1. Get and set current button and drow near sub option to increase or dercrese last char
	if (Active_Menu == EAM_Main)
	{// clear prev request
		FillRect(Ptr_Hdc, &Rect_User_Input_Change[0], AsConfig::Brush_Background);
		FillRect(Ptr_Hdc, &Rect_User_Input_Change[1], AsConfig::Brush_Background);

		Rect_User_Input_Change[0] = ui_rect_offset;
		Rect_User_Input_Change[1] = ui_rect_offset;
		return;
	}
	
	ui_rect_offset = User_Input_Rect[Prev_Button];
	FillRect(Ptr_Hdc, &Rect_User_Input_Change[0], AsConfig::Brush_Background);
	FillRect(Ptr_Hdc, &Rect_User_Input_Change[1], AsConfig::Brush_Background);

	// 1.1 Draw first Rectangle decrease 
	ui_rect_offset.left = ui_rect_offset.right + button_offset;
	ui_rect_offset.top = ui_rect_offset.top;
	ui_rect_offset.right = ui_rect_offset.right + button_offset + box_size;
	ui_rect_offset.bottom = ui_rect_offset.top + box_size;
	if (Rect_User_Input_Change != 0)
		Rect_User_Input_Change[0] = ui_rect_offset;
	Rectangle(Ptr_Hdc, ui_rect_offset.left, ui_rect_offset.top, ui_rect_offset.right, ui_rect_offset.bottom);
	MoveToEx(Ptr_Hdc, ui_rect_offset.left + scale, ui_rect_offset.top + half_box, 0);
	LineTo(Ptr_Hdc, ui_rect_offset.right - scale, ui_rect_offset.top + half_box);

	// 1.2 Draw second Rectangle increment
	ui_rect_offset.left = ui_rect_offset.left + box_size + scale;
	ui_rect_offset.top = ui_rect_offset.top;
	ui_rect_offset.right = ui_rect_offset.right + box_size + scale;
	ui_rect_offset.bottom = ui_rect_offset.bottom;
	if (Rect_User_Input_Change != 0)
		Rect_User_Input_Change[1] = ui_rect_offset;
	Rectangle(Ptr_Hdc, ui_rect_offset.left, ui_rect_offset.top, ui_rect_offset.right, ui_rect_offset.bottom);
	MoveToEx(Ptr_Hdc, ui_rect_offset.left + scale, ui_rect_offset.top + half_box, 0);
	LineTo(Ptr_Hdc, ui_rect_offset.right - scale, ui_rect_offset.top + half_box);
	MoveToEx(Ptr_Hdc, ui_rect_offset.left + half_box, ui_rect_offset.top + scale,0);
	LineTo(Ptr_Hdc, ui_rect_offset.left + half_box, ui_rect_offset.bottom - scale);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Reset()
{
	if (wcsstr(User_Input, L"http://") != 0 || wcsstr(User_Input, L"https://") != 0)
	{
		if (!std::filesystem::exists(AsConfig::Image_Folder) )
			std::filesystem::create_directories(AsConfig::Image_Folder);

		// !!! THREAD
		Curl_Component->Set_W_Url(User_Input);
		ACurl_Client client_url(EPrograms::ASaver, User_Input);
	}

	switch (Active_Menu)
	{
	case EAM_Watching:
		Add_To_User_Array(User_Array_Map, User_Input);
		break;

	case EAM_Library_Menu:
		Add_To_User_Array(User_Library_Map, User_Input);
		break;

	case EAM_Paused_Menu:
		Add_To_User_Array(User_Paused_Map, User_Input);
		break;

	case EAM_Wishlist:
		Add_To_User_Array(User_Wishlist_Map, User_Input);
		break;
	}
	Draw_Sub_Menu(Active_Menu);
	Draw_Active_Button();
	Save_All_To_Data(Active_Menu);  // Save by Add
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Set_RM_Cord(const RECT &mouse_cord)
{
	int i = 0;
	RECT intersect_rect {};

	// 1.Restore Image covered by context menu
	if (!IsRectEmpty(&Prev_Context_Menu_Cords) )  // если контекстное меню есть
		Restore_Image(Prev_Context_Menu_Cords);


	// 2. While clk on Main menu button redraw it and draw context menu
	for (int i = 0; i < Rect_Menu_List_Length; i++)
	{
		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Menu_List[i]) )
		{
			Active_Menu = EAM_Main;  // Don`t touch
			Redraw_Button_Advence( (EActive_Button)i);  // Change button color
			Draw_Sub_Menu( (EActive_Menu)i);

			Init_Context_Menu(mouse_cord.right, mouse_cord.top);
			return;
		}
	}


	// 3. If sub menu wasn`t be created don`t check arrays
	if (User_Input_Rect == 0)
		return;


	// 4. Draw Init_Context_Menu if at buttons
	for (i = Sub_Menu_Curr_Page * Sub_Menu_Max_Line; i < Rect_Sub_Menu_Length; i++)
	{
		if (IntersectRect(&intersect_rect, &mouse_cord, &User_Input_Rect[i]) )
		{
			Redraw_Button_Advence( (EActive_Button)i);
			Init_Context_Menu(mouse_cord.right, mouse_cord.top);
			return;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Set_LM_Cord(const RECT &mouse_cord)
{
	RECT intersect_rect {};

	if (!IsRectEmpty(Rect_Pages) )
	{//Rect_Pages buttons Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Prev]) )
		{
			if (Sub_Menu_Curr_Page < 1)
				return;
			else
				Sub_Menu_Curr_Page--;

			Draw_Sub_Menu(Active_Menu);
			return;
		}
		else if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Next]) )
		{
			Sub_Menu_Curr_Page++;
			Draw_Sub_Menu(Active_Menu);
			
			Active_Menu = EAM_Main;
			if (Active_Button != (EActive_Button)-1)
				User_Input_Request();
			
			Active_Menu = EAM_Watching;

			return;
		} else if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Update]) )  // Update Button
		{
			Update_ID_Content();  // While press Update Page
			return;
		}
	}


	if (!IsRectEmpty(&Rect_User_Input_Change[0]) )
	{// Reguest Handle

		for (int i = 0; i < 2; i++)
			if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_User_Input_Change[i]) )
			{
				User_Input_Adjust(i);
				Redraw_Button_Advence( (EActive_Button)Prev_Button);
				return;
			}
	}


	if (!IsRectEmpty(&Input_Button_Rect) )
	{// User_Input Handle || doubl click on User_Input ||

		if (IntersectRect(&intersect_rect, &mouse_cord, &Input_Button_Rect) )
		{
			if (User_Input[0] != 0)
			{
				User_Input_Reset();
				return;
			}
			else if (OpenClipboard(0) )
			{
				HANDLE handle_data = GetClipboardData(CF_UNICODETEXT);

				if (handle_data != 0)
				{
					WCHAR *psz_text = static_cast<WCHAR*>(GlobalLock(handle_data) );  // Text from clipboard

					int clipboard_length;
					int index;

					index = 0;
					if (psz_text != 0)
					{
						clipboard_length = (int)wcslen(psz_text);
						while (psz_text[index] != '\0')
						{
							Set_User_Input(psz_text[index]);
							index++;
						}
					}

					GlobalUnlock(handle_data);
				}

				CloseClipboard();
			}
			User_Input_Redraw_Button();
			return;
		}

	}


	if (!IsRectEmpty(&Prev_Context_Menu_Cords) )
	{// Context Menu Handle

		for (int i = 0; i < Context_Button_Length; i++)
		{// check clk on context menu

			if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Buttons_Context[i]) )
			{
				EActive_Menu prev_active_mune = Active_Menu;
				Restore_Image(Prev_Context_Menu_Cords);

				if (Active_Menu != EAM_Main)
				{
					Active_Menu = EAM_Main;
					User_Input_Request();
					Active_Menu = prev_active_mune;
				}

				switch ( (EActive_Menu)i)
				{
				case EAM_Watching:
					Add_To_User_Array(User_Array_Map, It_Current_User->second.Title_Name_Num.c_str() );
					Erase_From_User_Array();
					Draw_Sub_Menu(Active_Menu);
					break;

				case EAM_Library_Menu:
					Add_To_User_Array(User_Library_Map, It_Current_User->second.Title_Name_Num.c_str() );
					Erase_From_User_Array();
					Draw_Sub_Menu(Active_Menu);
					break;

				case EAM_Paused_Menu:
					Add_To_User_Array(User_Paused_Map, It_Current_User->second.Title_Name_Num.c_str() );
					Erase_From_User_Array();
					Draw_Sub_Menu(Active_Menu);
					break;

				case EAM_Wishlist:
					Add_To_User_Array(User_Wishlist_Map, It_Current_User->second.Title_Name_Num.c_str());
					Erase_From_User_Array();
					Draw_Sub_Menu(Active_Menu);
					break;

				case EAM_Erase:
					Erase_From_User_Array();
					Draw_Sub_Menu(Active_Menu);
					break;
				}
				Save_All_To_Data( (EActive_Menu)i);
				return;
			}
		}
		Restore_Image(Prev_Context_Menu_Cords);
	}


	for (int i = 0; i < Rect_Menu_List_Length; i++)
	{// Main Menu Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Menu_List[i]) )
		{
			if (Active_Menu != EAM_Main)
			{
				Active_Menu = EAM_Main;
				User_Input_Request();
			}

			Active_Menu = EAM_Main;  // if enter here we clk on main menu border
			Redraw_Button_Advence( (EActive_Button)i);
			Draw_Sub_Menu( (EActive_Menu)i);
			return;
		}
	}


	for (int i = Sub_Menu_Curr_Page * Sub_Menu_Max_Line; i < Rect_Sub_Menu_Length; i++)
	{// Sub Menu Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &User_Input_Rect[i] ) )
		{
			Active_Button = (EActive_Button)i;
			Draw_Active_Button(); 

			return;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Set_Sub_Menu_Curr_Page(const int &set_page_to)
{
	int curr_page = 0, max_page = 0, max_page_line = 0;

	curr_page = Sub_Menu_Curr_Page + 1;
	max_page_line = 31;
	max_page = (int)User_Array_Map.size() / max_page_line;

	if (curr_page > max_page)
		curr_page = 0;

	Sub_Menu_Curr_Page = curr_page;
}
//------------------------------------------------------------------------------------------------------------
bool AsUI_Builder::Set_User_Input(const wchar_t &user_text)
{
	if (user_text == '\b')  // Backspace handle | delete last added user_text
	{
		User_Input[--User_Input_Len] = 0;  // delete prev user_text
		return false;
	}

	if (User_Input_Len < AsConfig::User_Input_Buffer)
		User_Input[User_Input_Len++] = user_text;
	else
		User_Input_Len = 0;

	return false;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Update_ID_Content()
{
	int index;
	int id_content_index;
	SUser_Input_Data new_ui_data;

	index = 0;
	id_content_index = 0;

	while (Curl_Component->Get_Url(User_Input, id_content_index) )
	{// While we have saved ID_Content

		// 1.1 Send reguest to url
		ACurl_Client reguest(EPrograms::ASaver, User_Input);
		new_ui_data = Init_UI_Data();  // User_Input convert to data

		// 1.2 If cant find return
		It_Current_User = User_Array_Map.find(User_Input);  // !!!
		//if (!(It_Current_User != User_Array_Map.end() ) )
		//	return;  // Can delete id_content_index with data if cant find || watched or deleted by user

		if (new_ui_data.Title_Num > It_Current_User->second.Title_Num)
		{// If have new series draw button if different color

			for (auto iter = User_Array_Map.begin(); iter != It_Current_User; ++iter)
				++index;

			// 2.1 Setting to redraw button
			Active_Button = (EActive_Button)index;
			Active_Page = EAP_Update;
			Draw_Active_Button();

			// 2.2 Get url at clipboard and redraw
			Curl_Component->Get_Url(User_Input, id_content_index);
			Add_To_Clipboard();  // Set to Clipboard and get url from User_Input
			User_Input_Redraw_Button();  // redraw
			return;
		}
		id_content_index++;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Button(const EActive_Button &active_button, std::map<std::wstring, SUser_Input_Data> &user_array)
{
	if (Active_Menu != EAM_Main)
	{
		if (Prev_Button == 99)
			Prev_Button = (int)active_button;

		It_Current_User = user_array.begin();  // Prev_Button
		std::advance(It_Current_User, (int)Prev_Button);
		Draw_Button_Text(AsConfig::Brush_Background, AsConfig::Color_Dark, AsConfig::Color_Text_Green, User_Input_Rect[Prev_Button], It_Current_User->second.Title_Name_Num.c_str() );
		
		It_Current_User = user_array.begin();  // Active Button
		std::advance(It_Current_User, (int)active_button);

		if (Active_Page != EAP_Update)  // if from update button change color
		{
			Draw_Button_Text(AsConfig::Brush_Green_Dark, AsConfig::Color_Text_Green, AsConfig::Color_Dark, User_Input_Rect[(int)active_button], It_Current_User->second.Title_Name_Num.c_str());
			Add_To_Clipboard_Name_Key();
		}
		else
		{
			Active_Page = EAP_None;
			Draw_Button_Text(AsConfig::Brush_Background_Button_Update, AsConfig::Color_Backgrount_Text, AsConfig::Color_Dark, User_Input_Rect[(int)active_button], It_Current_User->second.Title_Name_Num.c_str());
		}
		Prev_Button = (int)active_button;
	}
	else
	{
		Draw_Button_Text(AsConfig::Brush_Background, AsConfig::Color_Dark, AsConfig::Color_Text_Green, Rect_Menu_List[Prev_Main_Menu_Button], Button_Text_List[Prev_Main_Menu_Button].c_str() );
		Draw_Button_Text(AsConfig::Brush_Green_Dark, AsConfig::Color_Text_Green, AsConfig::Color_Dark, Rect_Menu_List[(int)active_button], Button_Text_List[(int)active_button].c_str() );

		Prev_Main_Menu_Button = (int)active_button;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button_Text(const HBRUSH& background, const COLORREF& color_bk, const COLORREF& color_tx, const RECT& rect, const wchar_t *str) const
{
	SelectObject(Ptr_Hdc, background);
	SetBkColor(Ptr_Hdc, color_bk);
	SetTextColor(Ptr_Hdc, color_tx);

	Rectangle(Ptr_Hdc, rect.left, rect.top, rect.right, rect.bottom);  // draw rect
	TextOutW(Ptr_Hdc, rect.left + AsConfig::Global_Scale, rect.top + AsConfig::Global_Scale, str, (int)wcslen(str));  // button_prev text ( x its text out in middle
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_User_Title_Image(const wchar_t *image_path) const
{
	int width = 0, height = 0, bpp = 0;
	DirectX::ScratchImage image_title;
	BITMAPINFO bmi = {};
	RECT img_cords{};

	if (!std::filesystem::exists(image_path) )
		DirectX::LoadFromWICFile(AsConfig::Main_Image_Folder, DirectX::WIC_FLAGS_NONE, 0, image_title);
	else
		DirectX::LoadFromWICFile(image_path, DirectX::WIC_FLAGS_NONE, 0, image_title);

	const DirectX::Image* img = image_title.GetImage(0, 0, 0);
	if (img)
	{
		DXGI_FORMAT format = img->format;
		bpp = (int)DirectX::BitsPerPixel(format);
		width = (int)img->width;
		height = (int)img->height;

		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height; // отрицательное значение для вертикального растеризации сверху вниз
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = bpp;
		bmi.bmiHeader.biCompression = BI_RGB;

		img_cords.left = 9;
		img_cords.top = 182;
		img_cords.right = AsConfig::Main_Image_Width;  // 415
		img_cords.bottom = AsConfig::Main_Image_Height;  // 636
		FillRect(Ptr_Hdc, &img_cords, AsConfig::Brush_Background);

		img_cords.right = width;  // 415
		img_cords.bottom = height;  // 636


		StretchDIBits(Ptr_Hdc, img_cords.left, img_cords.top, img_cords.right, img_cords.bottom, 0, 0, width, height, img->pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);
	}
	image_title.Release();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_User_Map(RECT &border_rect, std::map<std::wstring, SUser_Input_Data> &map)
{
	int curr_line;
	int curr_page_max_line;
	
	delete[] User_Input_Rect;

	Rect_Sub_Menu_Length = (int)map.size();
	User_Input_Rect = new RECT[Rect_Sub_Menu_Length];

	It_Current_User = map.begin();
	if (map.size() < Sub_Menu_Max_Line)
		Sub_Menu_Curr_Page = 0;
	
	curr_page_max_line = Sub_Menu_Max_Line * (Sub_Menu_Curr_Page + 1);
	curr_line = Sub_Menu_Curr_Page * Sub_Menu_Max_Line;
	std::advance(It_Current_User, curr_line);

	for (; It_Current_User != map.end(); ++It_Current_User)
	{
		if (curr_line >= Rect_Sub_Menu_Length)
			break;
		else
		{
			User_Input_Rect[curr_line] = Add_Button(border_rect, It_Current_User->second.Title_Name_Num);
			curr_line++;
		}

		if (curr_line > curr_page_max_line - 1)
		{
			Add_Button_Next_Page();
			return;
		}
	}
	Add_Button_Next_Page();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Active_Button()
{
	Redraw_Button_Advence(Active_Button);  // Draw Active button
	User_Input_Request();  // Draw Requests and clear prev requests

	std::wstring image_path = AsConfig::Image_Folder;
	image_path += It_Current_User->second.Title_Name_Key;
	image_path += L".png";
	Draw_User_Title_Image(image_path.c_str());  // Initialize Title Image Folder
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Init_User_Array_Load(const std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path)
{
	int i = 0, len = 0;
	int user_array_size;
	wchar_t **user_array;

	user_array_size = (int)user_arr.size();
	user_array = new wchar_t *[user_array_size];

	// init array for save
	for (auto &it : user_arr)
	{
		const std::wstring &key = it.second.Title_Name_Num;
		len = (int)key.length() + 1;
		if (i < user_array_size)
		{
			user_array[i] = new wchar_t[len];
			wcscpy_s(user_array[i], len, key.c_str() );
			i++;
		}
	}

	// save data to disk
	User_Input_Save(file_path, user_array, user_array_size);

	// free memory
	for (int i = 0; i < user_array_size; i++)
		delete[] user_array[i];  // Удаление данных на которых указивают указателеи

	delete[] user_array;  // Удаление указателей
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Init_Context_Menu(const int& x, const int& y)
{
	int button_text_len;
	int scale;
	int context_offset;
	int user_input_len;
	int button_heigh;
	RECT context_rect;

	button_text_len = (int)Button_Text_List[3].length();
	scale = AsConfig::Global_Scale;
	context_offset = 6;
	user_input_len = button_text_len * AsConfig::Ch_W + context_offset;
	button_heigh = AsConfig::Ch_H * Context_Button_Length + context_offset;
	context_rect = {};

	// 1. Start point where RMB pressed, then we take longest word and add his len like width
	context_rect.left = x;
	context_rect.top = y;
	context_rect.right = context_rect.left + user_input_len;
	context_rect.bottom = context_rect.top + button_heigh;

	// 2. Delete all prev Button Context Rect and create new
	delete[] Rect_Buttons_Context;
	Rect_Buttons_Context = new RECT[Context_Button_Length];

	// 3. Before draw Context Menu save Image where we draw it to reload
	Save_Image(context_rect);  // Save image to buffer while we can restore with condition

	// 4. Draw Context Menu
	Rectangle(Ptr_Hdc, context_rect.left, context_rect.top, context_rect.right, context_rect.bottom);

	for (int i = 0; i < Context_Button_Length; i++)
	{
		button_text_len = (int)Button_Text_List[i].length();
		std::wstring w_title(Button_Text_List[i].begin(), Button_Text_List[i].end() );
		TextOutW(Ptr_Hdc, context_rect.left + scale + 1, context_rect.top + AsConfig::Ch_H * i + scale + 1, w_title.c_str(), button_text_len);

		// 4.1. Save Context Buttons Rects || handle when LMB pressed
		Rect_Buttons_Context[i].left = context_rect.left;
		Rect_Buttons_Context[i].top = context_rect.top;
		Rect_Buttons_Context[i].right = context_rect.right;
		Rect_Buttons_Context[i].bottom = context_rect.top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H;

		if (!(i < Context_Button_Length - 1) )
			break;

		MoveToEx(Ptr_Hdc, context_rect.left + scale, context_rect.top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H, NULL);
		LineTo(Ptr_Hdc, context_rect.left + scale + button_text_len * 8, context_rect.top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H);
	}
	Prev_Context_Menu_Cords = context_rect;
}
//------------------------------------------------------------------------------------------------------------
int AsUI_Builder::Init_Seasons(int curr_it) const
{
	int ch;
	int season = 0;
	int ch_space = 32;
	int ch_rus = 1000;  // russian chars
	int i = 73, v = 86, x = 88;  // maybe make enumeration, it`s means I V X numbers in chars

	ch = User_Input[++curr_it];

	while (ch != L' ')
	{
		if (ch > ch_rus && season < 1)
			return 1;  // 73 its I
		else if (ch > ch_rus && season > 1)
			return season;
		else if (ch == '\0')
			return season;

		if (ch == i)
			season = season + 1;
		else if (ch == v && season < 1)
			season = season + 5;
		else if (ch == x && season < 1)
			season = season + 10;
		else if (ch == i && season >= 5)
			season = season - 1;
		else if (ch == v && season > 0)
			season = 5 - season;
		else if (ch == x && season > 0)
			season = 10 - season;

		ch = User_Input[++curr_it];
	}
	return season;
}
//------------------------------------------------------------------------------------------------------------
RECT AsUI_Builder::Add_Border(const int &x_cord) const
{
	bool is_sub_menu = false;
	int scale;
	int border_width;
	int border_height;
	RECT border_rect = {};

	scale = AsConfig::Global_Scale;

	if (x_cord != 0)  // if not main menu we must reset setting
	{// Draw Sub Menu
		border_width = (AsConfig::Window_Width - x_cord) - 23 * 2 - 9;  // window_width - main menu width
		border_height = AsConfig::Window_Height + 30 - scale;
		is_sub_menu = true;
		SelectObject(Ptr_Hdc, AsConfig::Brush_Green);
	}
	else
	{// Draw Main Menu
		border_width = (Main_Menu_Titles_Length_Max + scale) * AsConfig::Ch_W;  // 424
		border_height = (Rect_Menu_List_Length + 2) * (AsConfig::Ch_H + 6) + scale + 1;
	}

	// 1.2. Set cords for first border
	border_rect.left = x_cord + scale;
	border_rect.top = scale;
	border_rect.right = x_cord + border_width;
	border_rect.bottom = border_height;
	Rectangle(Ptr_Hdc, border_rect.left, border_rect.top, border_rect.right, border_rect.bottom);  // Draw First Border

	// 1.2. Set cords for second border
	border_rect.left = border_rect.left + scale;
	border_rect.top = border_rect.top + scale;
	border_rect.right = border_rect.right - scale;
	border_rect.bottom = border_rect.bottom - scale;
	Rectangle(Ptr_Hdc, border_rect.left, border_rect.top, border_rect.right, border_rect.bottom);  // Second Border

	// 2.1. Fill Rect
	if (is_sub_menu)
	{// Fill sub_menu to grey menu
		border_rect.left = border_rect.left + 1;
		border_rect.top = border_rect.top + 1;
		border_rect.right = border_rect.right - 1;
		border_rect.bottom = border_rect.bottom - 1;

		FillRect(Ptr_Hdc, &border_rect, AsConfig::Brush_Background);
		is_sub_menu = false;
	}

	return border_rect;
}
//------------------------------------------------------------------------------------------------------------
RECT AsUI_Builder::Add_Button(RECT &border_rect, const std::wstring &title) const
{
	int scale = AsConfig::Global_Scale;
	int button_w, button_h;
	RECT button = {};

	button_w = border_rect.right - border_rect.left - scale;  // button width
	button_h = border_rect.top + AsConfig::Ch_H + scale * 2;  // button height

	button.left = border_rect.left + scale;  // x = 3px from curr border
	button.top = border_rect.top;
	button.right = button.left + button_w - scale;  // border width - offset to draw button)
	button.bottom = button_h;

	Rectangle(Ptr_Hdc, button.left, button.top, button.right, button.bottom);  // draw button
	TextOutW(Ptr_Hdc, button.left + scale, button.top + scale, title.c_str(), (int)title.length() );

	border_rect.top = button.bottom + scale;  // cuting border for next button

	return button;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Add_Button_Next_Page()
{
	int scale = AsConfig::Global_Scale;
	RECT button_update = { 1140, 12, 1228, 30 };  // Update Page
	RECT button_prev = { 1231, 12, 1303, 30 };  // Next Page
	RECT button_next= { 1305, 12, 1375, 30 };  // Prev Page

	Rectangle(Ptr_Hdc, button_update.left, button_update.top, button_update.right, button_update.bottom);
	TextOutW(Ptr_Hdc, button_update.left + 1, button_update.top + 1, L"Update Page", 11);
	Rectangle(Ptr_Hdc, button_prev.left, button_prev.top, button_prev.right, button_prev.bottom);
	TextOutW(Ptr_Hdc, button_prev.left + 1, button_prev.top + 1, L"Prev Page", 9);
	Rectangle(Ptr_Hdc, button_next.left, button_next.top, button_next.right, button_next.bottom);
	TextOutW(Ptr_Hdc, button_next.left + 1, button_next.top + 1, L"Next Page", 9);

	Rect_Pages[EActive_Page::EAP_Update] = button_update;
	Rect_Pages[EActive_Page::EAP_Prev] = button_prev;
	Rect_Pages[EActive_Page::EAP_Next] = button_next;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Add_To_User_Array(std::map<std::wstring, SUser_Input_Data> &user_arr, const wchar_t *user_input)
{
	int curr_it = 0;
	SUser_Input_Data ui_data = {};

	if (user_input[0] == L'\0')
		return;

	// 1. If User_Input not empty init if we can || Use while load from file
	if (User_Input[0] == L'\0')
	{
		if (user_input != 0)
		{
			while (user_input[curr_it] != '\0')
				Set_User_Input(user_input[curr_it++]);
		}
	}

	// 1.2  Check User_Input orphography
	while (User_Input[wcslen(User_Input) - 1] == L' ')  // if last ch = space delete
		User_Input[--User_Input_Len] = L'\0';

	// 1.3 Init_Data before set to map
	ui_data = Init_UI_Data();

	// 1.4. Check if containts the same key if not save to map
	if (user_arr.contains(ui_data.Title_Name_Key) )
		user_arr[ui_data.Title_Name_Key] = ui_data;
	else
		user_arr.emplace(ui_data.Title_Name_Key, ui_data);  // if not add new title

	if (std::filesystem::exists("Pictures/Temporary.png") )
		std::filesystem::rename("Pictures/Temporary.png", (std::wstring(AsConfig::Image_Folder) + ui_data.Title_Name_Key + std::wstring(L".png") ) );

	if (Active_Menu != -1)
	{
		It_Current_User = user_arr.find(User_Input);
		Active_Button = (EActive_Button)std::distance(user_arr.begin(), It_Current_User);
		Sub_Menu_Curr_Page = ((int)Active_Button - 1) / Sub_Menu_Max_Line;  // Find Page
	}

	// Reset User_Input
	for (int i = 0; i < User_Input_Len; i++)
		User_Input[i] = 0;

	User_Input_Len = 0;
}
//------------------------------------------------------------------------------------------------------------
 void AsUI_Builder::Add_To_Clipboard()
{// Need refactoring, can delete url

	wchar_t *url;
	size_t size_in_bytes;
	HGLOBAL global_handle;
	LPVOID global_ptr;

	if (!OpenClipboard(0) )
		return;

	if (!EmptyClipboard() )  // clean, if cant close and return
	{
		CloseClipboard();
		return;
	}

	url = new wchar_t[wcslen(User_Input) + 1];  // Выделение памяти для url с учетом завершающего нуля
	wcsncpy_s(url , wcslen(User_Input) + 1, User_Input, wcslen(User_Input));  // Копирование строки User_Input в url 
	size_in_bytes = (wcslen(User_Input) + 1) * sizeof(wchar_t);  // Вычисление размера памяти с учетом правильных скобок
	global_handle = GlobalAlloc(GMEM_MOVEABLE, size_in_bytes);

	if (!global_handle)
		CloseClipboard();
	else
	{
		global_ptr = GlobalLock(global_handle);

		if (!global_ptr)
			return;

		memcpy(global_ptr, url, size_in_bytes);  // Скопировать текст в выделенную память
		GlobalUnlock(global_handle);  // Разблокировать глобальную память

		if (SetClipboardData(CF_UNICODETEXT, global_handle) )  // Поместить текст в буфер обмена
			CloseClipboard();  // Закрыть буфер обмена
		else
		{
			GlobalFree(global_handle);  // Освободить глобальную память в случае ошибки
			CloseClipboard();
		}
	}
	delete []url;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Add_To_Clipboard_Name_Key()
{
	HGLOBAL global_handle;
	LPVOID global_ptr;

	if (!OpenClipboard(0) )
		return;
	
	if (!EmptyClipboard() )  // clean, if cant close and return
	{
		CloseClipboard();
		return;
	}

	if (global_handle = GlobalAlloc(GMEM_MOVEABLE, (It_Current_User->second.Title_Name_Key.size() + 1) * sizeof(wchar_t) ) )  // Выделить глобальную память для текста
	{
		if (global_ptr = GlobalLock(global_handle) )  // Заблокировать глобальную память и получить указатель на нее
		{
			memcpy(global_ptr, It_Current_User->second.Title_Name_Key.c_str(), (It_Current_User->second.Title_Name_Key.size() + 1) * sizeof(wchar_t) );  // Скопировать текст в выделенную память
			GlobalUnlock(global_handle);  // Разблокировать глобальную память

			if (SetClipboardData(CF_UNICODETEXT, global_handle) )  // Поместить текст в буфер обмена
				CloseClipboard();  // Закрыть буфер обмена
		}
	}
	else
	{
		GlobalFree(global_handle);
		CloseClipboard();
		return;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Erase_From_User_Array()
{
	if (Prev_Button == 99)
		return;

	switch (Active_Menu)
	{
	case EAM_Watching:
		User_Array_Map.erase(It_Current_User->first);
		break;

	case EAM_Library_Menu:
		User_Library_Map.erase(It_Current_User->first);
		break;

	case EAM_Paused_Menu:
		User_Paused_Map.erase(It_Current_User->first);
		break;

	case EAM_Wishlist:
		User_Wishlist_Map.erase(It_Current_User->first);
		break;
	}

	Prev_Button = 99;  // set to no prev_button
	Save_All_To_Data(Active_Menu);  // Save by Erase
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Load(std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path)
{
	bool is_add_to_user_array = false;
	wchar_t *user_input;
	wchar_t ch;
	int how_much_g;
	int block_sum;
	int ptr = 0, str = 0;
	unsigned long long *block_array;
	unsigned long long number;
	unsigned long long ch_long;
	unsigned long long index;
	
	user_input = new wchar_t[100];
	ch = 0;
	how_much_g = 0;
	block_sum = 0;
	number = 0;
	ch_long = 0;
	index = 0;

	std::ifstream infile(file_path, std::ios::binary);  // Откриваем файл по названию
	if (!infile)
		return;

	infile.seekg(0, std::ios::end);  // Вычисляем количество чисел в файле тем самим переходя в конец файла
	how_much_g = (int)infile.tellg();

	block_sum = how_much_g / sizeof(unsigned long long);  // (long long) 8 / size = how manny in unsigned long long data

	infile.seekg(0, std::ios::beg);  // Переходим в начало файла
	block_array = new unsigned long long[block_sum];  // Выделяем память для чтения чисел из файла
	infile.read(reinterpret_cast<char*>(block_array), how_much_g);  // Читаем и записиваем числа из файла в массив!

	while (ptr < block_sum)
	{
		number = block_array[ptr];
		index = 10000000000000000LL;

		while (index != 0)
		{
			ch_long = number / index;
			ch_long %= 100;
			index /= 100;

			while (ch_long == 0)
			{
				ch_long = number / index;
				ch_long %= 100;
				index /= 100;

				if (index == 0)
					break;
			}

			// Конвертируем в рус символы
			int converted_char = (int)ch_long;

			if (converted_char <= 64 && converted_char > 55)
				is_add_to_user_array = true;

			if (is_add_to_user_array && converted_char > 64 || is_add_to_user_array && converted_char < 55)
			{
				user_input[str] = L'\0';
				str = str + 1;

				user_input[0] = user_input[0] - 32;  // Делаем заглавной
				Add_To_User_Array(user_arr, user_input);
				is_add_to_user_array = false;
				str = 0;
			}

			Convert(converted_char, false);

			wchar_t ch = (wchar_t)converted_char;
			user_input[str] = ch;
			str++;
		}
		ptr++;

		if (ptr == block_sum)
		{
			user_input[str] = L'\0';
			str = str + 1;

			user_input[0] = user_input[0] - 32;  // Делаем заглавной
			Add_To_User_Array(user_arr, user_input);
		}

		if (!infile)
			return;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Save(const char *file_path, wchar_t **user_array, int user_input_counter)
{
	int ptr = 0, str = 0, counter_write = 0;
	unsigned long long numbers = 0;
	std::string user_input_saved_folder = "Data/";

	if (!std::filesystem::exists(user_input_saved_folder) )
		std::filesystem::create_directory(user_input_saved_folder);
	
	user_input_saved_folder += file_path;

	std::ofstream outfile(user_input_saved_folder, std::ios::out | std::ios::binary);  // Создаем новые данные

	if (!outfile)
		return;

	while (user_input_counter != 0)
	{
		while (user_array[ptr][str] != L'\0')  // Пока не достигнем конца обрабативаем строку
		{
			wchar_t ch;
			int ch_int;

			ch = user_array[ptr][str];
			ch_int = (int)ch;  // 1105

			Convert(ch_int, true);

			if (counter_write % 9 == 0 && counter_write != 0)  // Заходим каждый 9 раз
			{
				outfile.write(reinterpret_cast<const char*>(&numbers), sizeof(numbers) );
				numbers = 0;
			}

			if (numbers == 0)
				numbers = ch_int;
			else
				numbers = numbers * 100 + ch_int;  // 100 - смещение на 2 числа

			counter_write++;
			str++;
		}

		user_input_counter--;
		ptr++;  // переходим на следующую строку
		str = 0;  // переходим в начало строки

		bool is_nine = counter_write % 9 != 0;

		if (user_input_counter == 0 && is_nine)  // если в конце массива то записиваем остаток символов
			outfile.write(reinterpret_cast<const char*>(&numbers), sizeof(numbers) );
		else if (!is_nine && user_input_counter == 0)
			outfile.write(reinterpret_cast<const char*>(&numbers), sizeof(numbers) );
	}

	if (!outfile)
		return;

	outfile.close();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Save_Image(const RECT &rect)
{
	Hdc_Memory = CreateCompatibleDC(Ptr_Hdc);
	if (Hdc_Memory != 0)
	{
		H_Bitmap = CreateCompatibleBitmap(Ptr_Hdc, rect.right - rect.left, rect.bottom - rect.top);
		if (H_Bitmap != NULL)
		{
			Saved_Object = SelectObject(Hdc_Memory, H_Bitmap);
			BitBlt(Hdc_Memory, 0, 0, rect.right - rect.left, rect.bottom - rect.top, Ptr_Hdc, rect.left, rect.top, SRCCOPY);
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Restore_Image(RECT& rect)
{
	if (Hdc_Memory != NULL && H_Bitmap != NULL && Saved_Object != NULL) {
		BitBlt(Ptr_Hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, Hdc_Memory, 0, 0, SRCCOPY);
		SelectObject(Hdc_Memory, Saved_Object);
		DeleteObject(H_Bitmap);
		DeleteDC(Hdc_Memory);
		// Сброс сохраненных значений
		Hdc_Memory = 0;
		H_Bitmap = 0;
		Saved_Object = NULL;
	}
	rect = {};  // обнуляем
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Convert(int &ch, const bool &is_in_file)
{
	if (!is_in_file)
	{// If load
		if (ch == 38)
		{
			ch = 88;
			return;
		}
		if (ch == 39)
		{
			ch = 86;
			return;
		}
		if (ch >= 40 && ch <= 65)
		{
			ch = ch - 7;
			return;
		}

		if (ch == 66)
		{
			ch = ch - 3;
			return;
		}

		if (ch == 67)  // Обработка символов I & V на латиници
		{
			ch = ch + 6;
			return;
		}

		if (ch >= 68)
			ch = ((ch + 1000) - 28) + 32;  // low case to top case 

	}
	else
	{// If Save
		if (ch == 63)
		{
			ch = ch + 3;
			return;
		}

		if (ch == 88 || ch == 120)
		{
			ch = 38;
			return;
		}

		if (ch == 73)
		{
			ch = ch - 6;
			return;
		}

		if (ch >= 33 && ch <= 58)
		{
			ch = ch + 7;
			return;
		}

		if (ch == 86)
		{
			ch = 39;
			return;
		}

		if (ch == 1105)  // ё to е
			ch = 1077;

		if (ch >= 1072)  // low case to top case
			ch = ch - 32;

		if (ch > 1000)
			ch = (ch - 1000) + 28;

		if (ch == 0)
			ch = 36;
	}
}
//------------------------------------------------------------------------------------------------------------
SUser_Input_Data AsUI_Builder::Init_UI_Data()
{
	wchar_t *num;
	int is_space = 0;
	int curr_it;
	SUser_Input_Data ui_data;
	User_Input_Len = (int)wcslen(User_Input);

	// Initialize & Find were key starts
	curr_it = User_Input_Len;
	while (User_Input[curr_it] < 1000)
	{
		curr_it--;
		if (User_Input[curr_it] == L' ')  // if space
		{
			is_space++;
			switch (is_space)
			{
			case 1:
				num = User_Input + curr_it;
				ui_data.Title_Num = std::stoi(num);  // Initialize TITLE_NUM
				break;


			case 2:
				User_Input[curr_it + 3 + 1] = L'\0';
				num = User_Input + curr_it + 1;

				// Initialize TITLE_SEASON
				ui_data.Title_Season = Init_Seasons(curr_it);
				if (ui_data.Title_Season == 0)
					ui_data.Title_Season = (int)std::stoi(num);
				break;


			default:
				break;
			}
		}
	}

	// Initialize TITLE_NAME_KEY
	User_Input[++curr_it] = L'\0';
	ui_data.Title_Name_Key = User_Input;

	// Initialize TITLE_NAME_NUM
	ui_data.Title_Name_Num += ui_data.Title_Name_Key;
	ui_data.Title_Name_Num.append(L" ");
	if (ui_data.Title_Season != 0)
	{
		ui_data.Title_Name_Num += AsConfig::Season[ui_data.Title_Season - 1];
		ui_data.Title_Name_Num.append(L" ");
	}
	
	ui_data.Title_Name_Num += std::to_wstring(ui_data.Title_Num);
	ui_data.Title_Data;

	return ui_data;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Save_All_To_Data(const EActive_Menu& menu)
{
	std::thread th_user_arra;
	std::thread th_libr_arra;
	std::thread th_paus_arra;
	std::thread th_wish_arra;

	switch (menu)
	{
	case EAM_Watching:
		th_user_arra = std::thread([&]() { Init_User_Array_Load(User_Array_Map, "Watching.bin"); });
		th_user_arra.join();
		break;

	case EAM_Library_Menu:
		th_libr_arra = std::thread([&]() { Init_User_Array_Load(User_Library_Map, "Library.bin"); });
		th_libr_arra.join();
		break;

	case EAM_Paused_Menu:
		th_paus_arra = std::thread([&]() { Init_User_Array_Load(User_Paused_Map, "Paused.bin"); });
		th_paus_arra.join();
		break;

	case EAM_Wishlist:
		th_wish_arra = std::thread([&]() { Init_User_Array_Load(User_Wishlist_Map, "Wishlist.bin"); });
		th_wish_arra.join();
		break;

	case EAM_Exit:
		th_user_arra = std::thread([&]() { Init_User_Array_Load(User_Array_Map, "Watching.bin"); });
		th_libr_arra = std::thread([&]() { Init_User_Array_Load(User_Library_Map, "Library.bin"); });
		th_paus_arra = std::thread([&]() { Init_User_Array_Load(User_Paused_Map, "Paused.bin"); });
		th_wish_arra = std::thread([&]() { Init_User_Array_Load(User_Wishlist_Map, "Wishlist.bin"); });

		th_user_arra.join();
		th_libr_arra.join();
		th_paus_arra.join();
		th_wish_arra.join();
		break;

	default:
		break;
	}

}
//------------------------------------------------------------------------------------------------------------




// AsUI_Book_Reader
AsUI_Book_Reader::AsUI_Book_Reader(HDC hdc)
 : Ptr_Hdc(hdc)
{

}
//------------------------------------------------------------------------------------------------------------
void AsUI_Book_Reader::Handle_Input(EKey_Type &key_type) const
{
	switch (key_type)
	{
	case EKT_None:
		break;


	case EKT_Draw_Main_Menu:
	{
		wchar_t temp_01[] = L"Welcome to the book reader, choose your option";

		TextOutW(Ptr_Hdc, 15, 15, temp_01, (int)wcslen(temp_01) );
	}
	break;


	case EKT_Enter:
		break;


	case EKT_Space:
		break;


	case EKT_LM_Down:
		Rectangle(Ptr_Hdc, 66, 66, 99, 99);
		break;


	case EKT_RM_Down:
		break;


	case EKT_Redraw_User_Input:
		break;


	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------




// AsEngine
AsEngine::~AsEngine()
{
	delete UI_Builder;
}
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
 : Is_After_Maximazied(true), Key_Type(EKey_Type::EKT_Draw_Main_Menu), Tools()
{
	Set_Current_Data();
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame(HWND hwnd)
{
	Ptr_Hwnd = hwnd;
	Ptr_Hdc = BeginPaint(Ptr_Hwnd, &Paint_Struct);

	if (UI_Builder == 0)
	{
		UI_Builder = new AsUI_Builder(Ptr_Hdc);
		Key_Type = EKey_Type::EKT_Draw_Main_Menu;
	}
	else
		UI_Builder->Ptr_Hdc = Ptr_Hdc;

	if (!Is_After_Maximazied)
	{
		Is_After_Maximazied = !Is_After_Maximazied;
		UI_Builder->Init();  // draw after maximazed window
		UI_Builder->Draw_Sub_Menu();
		return;
	}

	Handle_Input();
	EndPaint(Ptr_Hwnd, &Paint_Struct);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame_Book_Reader(HWND hwnd)
{
	Ptr_Hwnd = hwnd;
	Ptr_Hdc = BeginPaint(Ptr_Hwnd, &Paint_Struct);

	if (UI_Book_Reader == 0)
		UI_Book_Reader = new AsUI_Book_Reader(Ptr_Hdc);
	else
		UI_Book_Reader->Ptr_Hdc = Ptr_Hdc;

	UI_Book_Reader->Handle_Input(Key_Type);
	EndPaint(Ptr_Hwnd, &Paint_Struct);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Redraw_Frame() const
{
	InvalidateRect(Ptr_Hwnd, 0, FALSE);  // если во 2-м параметре указать RECT рисувать можно будет только в нем | TRUE если нужно стереть всё
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Get_Clipboard_From_Else()
{
	int index = 0;

	if (OpenClipboard(0) )
	{
		if (HANDLE hData = GetClipboardData(CF_UNICODETEXT) )
		{
			if (WCHAR* psz_text = static_cast<WCHAR*>(GlobalLock(hData) ) )
				while (psz_text[index] != '\0')
					UI_Builder->Set_User_Input(psz_text[index++]);  // Set Text from clipboard
			
			GlobalUnlock(hData);
		}
		CloseClipboard();
	}
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::On_Timer()
{
	return 1;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Handle_Input()
{
	switch (Key_Type)
	{
	case EKT_Draw_Main_Menu:
		UI_Builder->Init();
		break;


	case EKT_Redraw_User_Input:
		UI_Builder->User_Input_Redraw_Button();
		break;


	case EKT_LM_Down:
		Mouse_Handler_LM();
		break;


	case EKT_RM_Down:
		Mouse_Handler_RM();
		break;


	case EKT_Enter:
		UI_Builder->User_Input_Reset();
		break;


	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Mouse_Handler_LM()
{
	RECT mouse_rect {};

	mouse_rect.left = LM_Cord_X - 1;
	mouse_rect.top = LM_Cord_Y;
	mouse_rect.right = LM_Cord_X;
	mouse_rect.bottom = LM_Cord_Y + 1;

	UI_Builder->Set_LM_Cord(mouse_rect);

	if (UI_Builder->Active_Menu == EAM_Exit)
		AsEngine::~AsEngine();
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Mouse_Handler_RM()
{
	RECT mouse_rect{};

	mouse_rect.left = LM_Cord_X - 1;
	mouse_rect.top = LM_Cord_Y;
	mouse_rect.right = LM_Cord_X;
	mouse_rect.bottom = LM_Cord_Y + 1;

	UI_Builder->Set_RM_Cord(mouse_rect);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Set_Current_Data()
{
	time_t curr_time = time(&curr_time);  // curr_time <- time
	tm s_local_time;  // crt struct 

	localtime_s(&s_local_time, &curr_time);  // s_local_time <- localtime_s <- curr_time

	int day = s_local_time.tm_mday;
	int month = s_local_time.tm_mon + 1; // Считаем месяцы с 0, поэтому добавляем 1
	int year = s_local_time.tm_year + 1900; // Добавляем 1900, чтобы получить текущий год
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::Connect_To_Server()
{
	WSADATA wsocket_data;
	SOCKET socket_to_server;
	struct sockaddr_in address_server;
	unsigned long long client_buffer;
	unsigned char buffer_to_server[sizeof(unsigned long long)];

	// Settings
	address_server = {};
	client_buffer = 1234567890123456789ULL;  // this we send to server
	memcpy(buffer_to_server, &client_buffer, sizeof(unsigned long long));

	if (WSAStartup(MAKEWORD(2, 2), &wsocket_data) != 0)  // set versions of sockets data
		return 1;

	// 1.1. Init socket and Server address
	socket_to_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (socket_to_server == INVALID_SOCKET) {
		WSACleanup();
		return 1;
	}

	address_server.sin_family = AF_INET;
	address_server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP address
	address_server.sin_port = htons(666);

	// 1.2. Try to connect to server using address server
	if (connect(socket_to_server, (struct sockaddr*)&address_server, sizeof(address_server)) == SOCKET_ERROR)  // wait server
	{
		closesocket(socket_to_server);
		WSACleanup();
		return 1;
	}

	// 1.3. Send buffer to socket server
	send(socket_to_server, (char*)&buffer_to_server, sizeof(buffer_to_server), 0);

	closesocket(socket_to_server);
	WSACleanup();
	return 0;
}
//------------------------------------------------------------------------------------------------------------
