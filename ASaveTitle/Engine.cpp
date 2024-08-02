#include "Engine.h"

// ACurl_Client
ACurl_Client::~ACurl_Client()
{
	if (Patterns_Array != 0)
		for (int i = 0; i < (int)EPatterns_Site::Last_Index; ++i)
			delete[] Patterns_Array[i];

	delete[] Patterns_Array;
	delete[] Title_Site;
	delete[] ID_Content_Array;
	delete[] ID_Content_Folder;

	delete Content_W;
}
//------------------------------------------------------------------------------------------------------------
ACurl_Client::ACurl_Client(const EProgram &program, wchar_t *&user_input)
  : Title_Site(0), ID_Content_Folder(0), Patterns_Array{}, ID_Content_Array(0), ID_Content_Size(0), Content_W(0)
{// Setup
	
	AsTools::Format_Url_Sub_WString(user_input, L"//", L"/", Title_Site);  // Receive Domain --- animevost.org ---
	Content_W = new std::wstring(L"Data/") ;
	*Content_W += Title_Site;  // get path Data/animevost.org to create folder

	switch (program)
	{
	case EProgram::Invalid:
		break;
	case EProgram::ASaver:
		CURL_Handler(user_input);  // Init
		break;
	case EProgram::ABook_Reader:
		break;
	case EProgram::End:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Handler(wchar_t *&user_input_url)
{
	// 1.0. Create Pattern example to folder in program
	if(!std::filesystem::exists(*Content_W) )
		return Pattern_File_Create(user_input_url);

	// 2.0. Get title name, season, series + image if can
	Pattern_File_Read(*Content_W, Patterns_Array);  // Loaded from file if handler is a path
	Download_URL(user_input_url);  // use url to download content to file || CURL
	Read_From_File();  // read from file to wstring
	Find_Title_From_File(user_input_url);
	Download_Image();
	/*
	
	Get_Patterns
	Get_URL_Data
	Get_Contents
	Get_Title
	Get_Image

	File_Upload
	File_Read
	File_Title_Get
	File_Image_Get
	*/
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Content_ID_Load()
{
	delete[] ID_Content_Array;  // !!!

	std::ifstream infile(ID_Content_Folder, std::ios::in | std::ios::binary);
	if (infile)
	{
		infile.seekg(0, std::ios::end);
		const unsigned short how_much_g = (unsigned short)infile.tellg();

		ID_Content_Size = how_much_g / sizeof(unsigned short);

		infile.seekg(0, std::ios::beg);
		ID_Content_Array = new unsigned short [ID_Content_Size + 1] {};
		infile.read(reinterpret_cast<char*>(ID_Content_Array), how_much_g);
		infile.close();
	}
	else
		ID_Content_Array = new unsigned short[ID_Content_Size + 1] {};  // If file don`t exist or can`t open
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Content_ID_Get(const wchar_t *url)
{
	unsigned short id_content;
	size_t len;
	size_t converted_chars;
	std::string handler_str;
	std::vector<char> chars_buffer;

	// 1.0. Init Params
	len = std::wcslen(url) + 1; // + char for \0
	converted_chars = 0;
	chars_buffer.resize(len);

	// 1.1. Convert from wchar_t to char
	wcstombs_s(&converted_chars, chars_buffer.data(), len, url, len - 1);
	handler_str = chars_buffer.data();

	// 1.1. Get id content and return if already exists
	CURL_Content_Pattern_Find_From_To(handler_str, "/content/", "/");  // !!! Receive ID Content
	id_content = std::stoi(handler_str);
	for (int i = 0; i <= ID_Content_Size; ++i)  // Check the same value, if find exit from component
	{
		if (ID_Content_Array[i] == id_content)
			return;
	}
	ID_Content_Array[ID_Content_Size] = id_content;
	CURL_Content_ID_Emplace();  // Save all to files && Resize buffer
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Content_ID_Emplace()
{
	std::ofstream outfile(ID_Content_Folder, std::ios::out | std::ios::binary | std::ios::trunc);
	if (!outfile)
		return;

	if (ID_Content_Size != 65535)
		for (int i = 0; i <= ID_Content_Size; ++i)
			outfile.write(reinterpret_cast<const char*>(&ID_Content_Array[i]), sizeof(ID_Content_Array[i]) );

	outfile.close();
	CURL_Content_ID_Load();  // Load to increment buffer for content
}
//------------------------------------------------------------------------------------------------------------
bool ACurl_Client::CURL_Content_ID_Erase(const int &if_not_last_id_content)
{
	--ID_Content_Size;  // Get Array Size
	ID_Content_Array[if_not_last_id_content] = ID_Content_Array[ID_Content_Size--];  // set last to first short
	//CURL_Content_ID_Emplace();
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool ACurl_Client::CURL_Content_Url_Get(wchar_t *result, const int &id_content_index)
{
	std::wstring url;

	if (id_content_index >= ID_Content_Size)
		return false;
	url = L"https://anime-bit.ru/content/";

	if (ID_Content_Array[id_content_index] < 100)
		url += L"00" + std::to_wstring(ID_Content_Array[id_content_index]) + L"/";
	else if (ID_Content_Array[id_content_index] < 1000)
		url += L"0" + std::to_wstring(ID_Content_Array[id_content_index]) + L"/";
	else
		url += std::to_wstring(ID_Content_Array[id_content_index]) + L"/";

	const wchar_t *c_url = url.c_str();
	wcsncpy_s(result, wcslen(c_url) + 1, c_url, wcslen(c_url) );

	return true;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Content_Pattern_Find_From_To(std::string &url, const char *start, const char *end)  // ~80 000
{
	size_t start_pos;
	size_t end_pos;

	start_pos = url.find(start);  // Find index at str pattern
	if ( !(start_pos != std::string::npos) )  // Return if not found
		return;

	start_pos += std::strlen(start);  // Add to index str pattern length
	end_pos = url.find(end, start_pos);  // find end pattern from str pattern position
	if (end_pos != std::string::npos)
		url = url.substr(start_pos, end_pos - start_pos);  // get string from bgn and end patterns
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Pattern_File_Create(wchar_t *user_input)
{
	char *c_char;
	const wchar_t source_pattern[] = L"title_bgn = laquo;\ntitle_end = &raquo\ntitle_num_bgn = Серии: [\ntitle_num_end =  \nimage_bgn = <img src='\nimage_end = ' width\n";

	if (!std::filesystem::create_directories(*Content_W) )
		return;

	*Content_W += L"/PatternFindConfig.txt";
	std::ofstream outFile(*Content_W, std::ios::binary);
	if (!outFile)
		return;

	AsTools::Format_Wide_Char_To_Char(source_pattern, c_char);
	outFile.write(c_char, strlen(c_char) );
	outFile.close();

	delete[] c_char;
	user_input[0] = '\0';
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Pattern_File_Read(std::wstring &path, wchar_t **&result)
{// !!! Need refactoring

	int size_needed;
	int index;
	size_t bgn;
	size_t end;
	const int pattern_length = (int)EPatterns_Site::Last_Index;
	std::wstring wstring_t;
	std::string string_from_file;

	result = new wchar_t *[pattern_length] {};
	index = 0;
	*Content_W += L"/PatternFindConfig.txt";

	// 1.0. Read from file to string
	std::ifstream file(path, std::ios::binary);
	if (!file != 0)
		return;
	string_from_file = std::string( (std::istreambuf_iterator<char>(file) ), std::istreambuf_iterator<char>() );
	file.close();

	// 1.1. Convert string to wstring
	size_needed = MultiByteToWideChar(CP_UTF8, 0, string_from_file.c_str(), (int)string_from_file.size(), 0, 0);
	path.clear();
	path.resize(size_needed);
	MultiByteToWideChar(CP_UTF8, 0, string_from_file.c_str(), (int)string_from_file.size(), &path[0], size_needed);

	// 1.2. Copy from file to array user input, used like pattern to parsing sites
	size_needed--;  // don`t count last \n
	while (index < pattern_length)
	{
		bgn = path.find(AsConfig::Patterns_Config[index]) + wcslen(AsConfig::Patterns_Config[index]);
		end = path.find(AsConfig::Patterns_Config[index + 1], bgn);

		if (!(size_needed == end))
			end -= 1;  // if last symbol don`t erase 1 char \n

		wstring_t = path.substr(bgn, end - bgn);
		result[index] = new wchar_t[wcslen(wstring_t.c_str() ) + 1] {};
		wcsncpy_s(result[index], wcslen(wstring_t.c_str() ) + 1, wstring_t.c_str(), wcslen(wstring_t.c_str() ) );
		index++;
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Find_Title_From_File(wchar_t *&result)
{
	int title_name_length;
	int title_nums_len;
	const wchar_t *bgn_title = Patterns_Array[(int)EPatterns_Site::Title_Bgn];
	const wchar_t *end_title = Patterns_Array[(int)EPatterns_Site::Title_End];
	const wchar_t *bgn_nums = Patterns_Array[(int)EPatterns_Site::Title_Num_Bgn];
	const wchar_t *end_nums = Patterns_Array[(int)EPatterns_Site::Title_Num_End];

	// 1.0. Find Title Name bgn end
	const wchar_t *title_name_bgn = wcsstr(Content_W->c_str(), bgn_title) + wcslen(bgn_title);
	if (!title_name_bgn != 0)
		return;
	const wchar_t *title_name_end = wcsstr(title_name_bgn, end_title);  // Get title end ptr
	if (!title_name_end != 0)
		return;

	// 1.1. Find Title series bgn end
	const wchar_t *title_num_bgn = wcsstr(Content_W->c_str(), bgn_nums) + wcslen(bgn_nums);
	if (!title_num_bgn != 0)
		return;
	const wchar_t *title_num_end = wcsstr(title_num_bgn, end_nums);
	if (!title_num_end != 0)
		return;

	// 2.0. Set Title and season to result
	title_name_length = (int)(title_name_end - title_name_bgn) + 1;  // Get title name and season length
	title_nums_len = (int)(title_num_end - title_num_bgn) + 1;  // Get title numbers length(series)
	wcsncpy_s(result, title_name_length, title_name_bgn, (rsize_t)title_name_length - 1);

	// 2.1. Set Title num(series) to result
	result += title_name_length - 1;  // find where we need to start put nums
	result[0] = L' ';  // remove '\0' to space
	result++;  // to next index
	wcsncpy_s(result, title_nums_len, title_num_bgn, (rsize_t)title_nums_len - 1);
	result -= title_name_length;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Download_URL(const wchar_t *w_user_input_url)
{
	char *url;
	int size = 0;
	CURL *curl;
	CURLcode res;
	FILE *file;

	AsTools::Format_Wide_Char_To_Char(w_user_input_url, url);

	curl = curl_easy_init();  // Init
	fopen_s(&file, AsConfig::Temporary_File_Name[0], "wb");

	curl_easy_setopt(curl, CURLOPT_URL, url);  // Options
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURL_Content_Write_Data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	res = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);  // Cleaning
	if (file != 0)
		fclose(file);
	delete[] url;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Read_From_File()
{
	int index;
	size_t current_line;
	std::string line;
	std::string content_data_str;

	index = 25;  // Content line from start needed
	current_line = 0;
	delete Content_W;

	std::ifstream file(AsConfig::Temporary_File_Name[0], std::ios::binary);
	if (!file)
		return;

	// 1.0. Find pattern where from start add to string
	while (std::getline(file, line) )
		if ( (current_line = line.find("<h1>") ) != std::string::npos)
			break;

	// 1.1. Add(Write) needed content to string
	do content_data_str += line + "\n";
	while (std::getline(file, line) && index--);

	file.close();
	std::filesystem::remove(AsConfig::Temporary_File_Name[0]);  // Remove, don`t need anymore
	if (content_data_str.empty() )
		return;

	// !!! 1.2. Conver from string to wstring and return like result || Make AsTools
	index = MultiByteToWideChar(CP_UTF8, 0, &content_data_str[0], (int)content_data_str.size(), 0, 0);
	Content_W = new std::wstring(index, 0);
	MultiByteToWideChar(CP_UTF8, 0, &content_data_str[0], (int)content_data_str.size(), &(*Content_W)[0], index);
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Download_Image()
{
	char *c_url;
	wchar_t *w_url;
	const wchar_t *bgn = Patterns_Array[(int)EPatterns_Site::Image_Bgn];
	const wchar_t *end = Patterns_Array[(int)EPatterns_Site::Image_End];
	FILE *file;
	CURL *url_easy;
	CURLcode response;
	std::wstring w_str_url;
	
	// 1.2. Create image url full with doment and convert to char
	AsTools::Format_Url_Sub_WString(Content_W->c_str(), bgn, end, w_url);
	w_str_url = std::wstring(L"https://") + Title_Site + w_url;
	AsTools::Format_Wide_Char_To_Char(w_str_url.c_str(), c_url);

	// 1.3. Init and Download
	curl_global_init(CURL_GLOBAL_DEFAULT);
	url_easy = curl_easy_init();

	fopen_s(&file, "TemporaryName.png", "wb");
	curl_easy_setopt(url_easy, CURLOPT_URL, c_url);  // save img
	curl_easy_setopt(url_easy, CURLOPT_WRITEFUNCTION, CURL_Content_Write_Data);
	curl_easy_setopt(url_easy, CURLOPT_WRITEDATA, file);
	response = curl_easy_perform(url_easy);  // Download image to file
	
	if (file != 0)
		fclose(file);
	curl_easy_cleanup(url_easy);
	delete[] c_url;
	delete[] w_url;
}
//------------------------------------------------------------------------------------------------------------
size_t ACurl_Client::CURL_Content_Write_Data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
//------------------------------------------------------------------------------------------------------------




// AsUI_Builder
int AsUI_Builder::User_Input_Len = 0;
int AsUI_Builder::Context_Button_Length = 5;
const wchar_t AsUI_Builder::Main_Menu_Title_Name[] = L"Title Saver";
const wchar_t *AsUI_Builder::Sub_Menu_Title = L"Enter text here... or URL your site";
const std::wstring AsUI_Builder::Button_Text_List[] = { L"Watch", L"Watched", L"Paused", L"Add to wishlist", L"Errase from Array", L"Exit" };
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::~AsUI_Builder()
{
	// 1.2 Save covered image sys
	if (Hdc_Memory != 0)
		DeleteDC(Hdc_Memory);
	if (H_Bitmap != 0)
		DeleteObject(H_Bitmap);

	// 1.3 Save map to Data/...
	User_Map_Main_Save(Active_Menu = EAM_Exit);  // Exit from Program | if exit save all map

	// 1.5 Free memory
	delete[] Rect_User_Input_Change;
	delete[] Rect_Buttons_Context;
	delete[] Rect_Menu_List;
	delete[] User_Input_Rect;
}
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::AsUI_Builder(HDC hdc, const WPARAM &w_param, const LPARAM &l_param)
: Active_Menu(EAM_Main), Ptr_Hdc(hdc), Rect_Menu_List{}, User_Input{}, Rect_Menu_List_Length(0), Rect_Sub_Menu_Length(0), Sub_Menu_Curr_Page(0), Prev_Main_Menu_Button(0),
  Prev_Button(99), Main_Menu_Titles_Length_Max(50), Sub_Menu_Max_Line(31), User_Array_Max_Size(0), Active_Button(EActive_Button::EAB_Main_Menu),
  Active_Page(EActive_Page::EAP_None), User_Input_Rect{}, Rect_User_Input_Change{}, Rect_Buttons_Context{}, Prev_Context_Menu_Cords{},
  Rect_Pages{}, Input_Button_Rect{}, Hdc_Memory(0), H_Bitmap(0), Saved_Object(0), User_Input_Data{}
{
	// !!! Bad Load map from Data/...
	User_Map_Main_Load(User_Array_Map, "Data/Watching.bin");
	User_Map_Main_Load(User_Library_Map, "Data/Library.bin");
	User_Map_Main_Load(User_Paused_Map, "Data/Paused.bin");
	User_Map_Main_Load(User_Wishlist_Map, "Data/Wishlist.bin");

	Builder_Handler(hdc, EUI_Builder_Handler::Draw_Menu_Main, w_param, l_param);
	Rect_User_Input_Change = new RECT[2]{};
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Builder_Handler(HDC ptr_hdc, const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam)
{
	Ptr_Hdc = ptr_hdc;

	switch (builder_handler)
	{
	case EUI_Builder_Handler::Draw_Full_Window:
		Draw_Menu_Main();  // draw after maximazed window
		Draw_Menu_Sub();
		break;
	case EUI_Builder_Handler::Draw_Menu_Main:
		Draw_Menu_Main();
		break;
	case EUI_Builder_Handler::Draw_Menu_Sub:
		User_Input_Handle();  // Enter
		break;
	case EUI_Builder_Handler::Draw_User_Input_Button:
		Handle_User_Input(static_cast<wchar_t>(wParam) );
		Draw_User_Input_Button();  // Buttons
		break;
	case EUI_Builder_Handler::Handle_Mouse_LButton:
		Handle_LM_Button(lParam);
		break;
	case EUI_Builder_Handler::Handle_Mouse_RButton:
		Handle_RM_Button(lParam);
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Menu_Main()
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
void AsUI_Builder::Draw_Menu_Sub(const EActive_Menu &active_menu)
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
void AsUI_Builder::Draw_User_Input_Button() const
{
	RECT button = Input_Button_Rect;

	SelectObject(Ptr_Hdc, AsConfig::Brush_Background);
	Rectangle(Ptr_Hdc, button.left, button.top, button.right, button.bottom);
	
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);
	SetTextColor(Ptr_Hdc, AsConfig::Color_Yellow);

	if (User_Input[0] != 0)
		TextOutW(Ptr_Hdc, button.left + AsConfig::Global_Scale, button.top + AsConfig::Global_Scale, User_Input, (int)wcslen(User_Input) );  // activ_button  text ( x its text out in middle
	else
		TextOutW(Ptr_Hdc, button.left + AsConfig::Global_Scale, button.top + AsConfig::Global_Scale, Sub_Menu_Title, (int)wcslen(Sub_Menu_Title) );  // activ_button  text ( x its text out in middle
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_User_Input(const wchar_t &user_text)
{
	if (user_text == '\r')  // if not enter
		return;

	if (Active_Menu == EActive_Menu::EAM_Main)
		return;

	if (user_text == '\b')  // Backspace handle | delete last added user_text
	{
		if (User_Input_Len > 0)
			User_Input[--User_Input_Len] = 0;  // delete prev user_text
		return;
	}

	if (User_Input_Len < AsConfig::User_Input_Buffer)
		User_Input[User_Input_Len++] = user_text;
	else
		User_Input_Len = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_RM_Button(const LPARAM &lParam)
{
	int i = 0;
	RECT intersect_rect{};
	RECT mouse_cord{};

	int x = lParam & 0xffff;
	int y = (int)(lParam >> 16);
	mouse_cord.left = x - 1;
	mouse_cord.top = y;
	mouse_cord.right = x;
	mouse_cord.bottom = y + 1;

	// 1.Restore Image covered by context menu
	if (!IsRectEmpty(&Prev_Context_Menu_Cords) )  // если контекстное меню есть
		Context_Image_Restore(Prev_Context_Menu_Cords);


	// 2. While clk on Main menu button redraw it and draw context menu
	for (int i = 0; i < Rect_Menu_List_Length; i++)
	{
		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Menu_List[i]) )
		{
			Active_Menu = EAM_Main;  // Don`t touch
			Handle_Active_Button( (EActive_Button)i);  // Change button color
			Draw_Menu_Sub( (EActive_Menu)i);

			Context_Menu_Draw(mouse_cord.right, mouse_cord.top);
			return;
		}
	}


	// 3. If sub menu wasn`t be created don`t check arrays
	if (User_Input_Rect == 0)
		return;


	// 4. Draw Context_Menu_Draw if at buttons
	for (i = Sub_Menu_Curr_Page * Sub_Menu_Max_Line; i < Rect_Sub_Menu_Length; i++)
	{
		if (IntersectRect(&intersect_rect, &mouse_cord, &User_Input_Rect[i]) )
		{
			Handle_Active_Button( (EActive_Button)i);
			Context_Menu_Draw(mouse_cord.right, mouse_cord.top);
			return;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_LM_Button(const LPARAM &lParam)
{
	RECT intersect_rect{};
	RECT mouse_cord{};

	int x = lParam & 0xffff;
	int y = (int)(lParam >> 16);
	mouse_cord.left = x - 1;
	mouse_cord.top = y;
	mouse_cord.right = x;
	mouse_cord.bottom = y + 1;

	if (!IsRectEmpty(Rect_Pages) )
	{//Rect_Pages buttons Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Prev]) )
		{
			if (Sub_Menu_Curr_Page < 1)
				return;
			else
				Sub_Menu_Curr_Page--;

			Draw_Menu_Sub(Active_Menu);
			return;
		}
		else if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Next]) )
		{
			Sub_Menu_Curr_Page++;
			Draw_Menu_Sub(Active_Menu);
			
			Active_Menu = EAM_Main;
			if (Active_Button != (EActive_Button)-1)
				Draw_User_Input_Request();
			
			Active_Menu = EAM_Watching;

			return;
		} else if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Pages[EActive_Page::EAP_Update]) )  // Update Button
		{
			Handle_Update_Button_Beta();  // While press Update Page
			//Handle_Update_Button();  // While press Update Page
			return;
		}
	}


	if (!IsRectEmpty(&Rect_User_Input_Change[0]) )
	{// Reguest Handle

		for (int i = 0; i < 2; i++)
			if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_User_Input_Change[i]) )
			{
				User_Input_Value_Is_Changed(i);
				Handle_Active_Button( (EActive_Button)Prev_Button);
				return;
			}
	}


	if (!IsRectEmpty(&Input_Button_Rect) )  // !!! Refactoring
	{// User_Input Handle || Double click on User_Input ||

		if (IntersectRect(&intersect_rect, &mouse_cord, &Input_Button_Rect) )
		{
			if (User_Input[0] != 0)
				User_Input_Handle();  // !!! Handle URL || What if thread?
			else
				User_Input_Set_To_Clipboard();

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
				Context_Image_Restore(Prev_Context_Menu_Cords);

				if (Active_Menu != EAM_Main)
				{
					Active_Menu = EAM_Main;
					Draw_User_Input_Request();
					Active_Menu = prev_active_mune;
				}

				switch ( (EActive_Menu)i)
				{
				case EAM_Watching:
				{
					wchar_t *title_name_num;
					int title_name_num_length;
					
					title_name_num_length = (int)wcslen(It_Current_User->second.Title_Name_Num.c_str() + 1);
					title_name_num = new wchar_t[title_name_num_length];
					
					wcsncpy_s(title_name_num, title_name_num_length, It_Current_User->second.Title_Name_Num.c_str(), wcslen(It_Current_User->second.Title_Name_Num.c_str() ) );
					User_Map_Emplace(User_Array_Map, title_name_num);
					User_Map_Erase();
					Draw_Menu_Sub(Active_Menu);
				}
				break;

				case EAM_Library_Menu:
				{
					wchar_t *title_name_num;
					int title_name_num_length;
					
					title_name_num_length = (int)wcslen(It_Current_User->second.Title_Name_Num.c_str() ) + 1;
					title_name_num = new wchar_t[title_name_num_length]{};
					
					wcsncpy_s(title_name_num, title_name_num_length, It_Current_User->second.Title_Name_Num.c_str(), wcslen(It_Current_User->second.Title_Name_Num.c_str() ) );
					User_Map_Emplace(User_Library_Map, title_name_num);
					User_Map_Erase();
					Draw_Menu_Sub(Active_Menu);
				}
				break;

				case EAM_Paused_Menu:
				{
					wchar_t *title_name_num;
					int title_name_num_length;
					
					title_name_num_length = (int)wcslen(It_Current_User->second.Title_Name_Num.c_str() + 1);
					title_name_num = new wchar_t[title_name_num_length];
					
					wcsncpy_s(title_name_num, title_name_num_length, It_Current_User->second.Title_Name_Num.c_str(), wcslen(It_Current_User->second.Title_Name_Num.c_str() ) );
					User_Map_Emplace(User_Paused_Map, title_name_num);
					User_Map_Erase();
					Draw_Menu_Sub(Active_Menu);
				}
				break;

				case EAM_Wishlist:
				{
					wchar_t *title_name_num;
					int title_name_num_length;
					
					title_name_num_length = (int)wcslen(It_Current_User->second.Title_Name_Num.c_str() + 1);
					title_name_num = new wchar_t[title_name_num_length];
					
					wcsncpy_s(title_name_num, title_name_num_length, It_Current_User->second.Title_Name_Num.c_str(), wcslen(It_Current_User->second.Title_Name_Num.c_str() ) );
					User_Map_Emplace(User_Wishlist_Map, title_name_num);
					User_Map_Erase();
					Draw_Menu_Sub(Active_Menu);
				}
					break;

				case EAM_Erase:
					User_Map_Erase();
					Draw_Menu_Sub(Active_Menu);
					break;
				}
				return;
			}
		}
		Context_Image_Restore(Prev_Context_Menu_Cords);
	}


	for (int i = 0; i < Rect_Menu_List_Length; i++)
	{// Main Menu Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &Rect_Menu_List[i]) )
		{
			if (Active_Menu != EAM_Main)
			{
				Active_Menu = EAM_Main;
				Draw_User_Input_Request();
			}

			Active_Menu = EAM_Main;  // if enter here we clk on main menu border
			Handle_Active_Button( (EActive_Button)i);
			Draw_Menu_Sub( (EActive_Menu)i);
			return;
		}
	}


	for (int i = Sub_Menu_Curr_Page * Sub_Menu_Max_Line; i < Rect_Sub_Menu_Length; i++)
	{// Sub Menu Handle

		if (IntersectRect(&intersect_rect, &mouse_cord, &User_Input_Rect[i] ) )
		{
			Active_Button = (EActive_Button)i;
			Handle_Active_Button_Advence();
			return;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Update_Button_Beta()
{
	int yy = 0;
	// TASKS
	/*
		- How to make this in threads, and work fine?
			- Get ID Content from file
				- Save ID Content while handle URL
			- Make from this id url
			- Use url to get title with num
			- Convert title with num to struct
			- Use struct to find same title, and check num
			- if different higlight new title to watch
	*/
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Active_Button_Advence()
{
	if (Active_Button == EActive_Button::EAB_Main_Menu)
		return;

	Handle_Active_Button(Active_Button);  // Draw Active button
	Draw_User_Input_Request();  // Draw Requests and clear prev requests

	std::wstring image_path = AsConfig::Image_Folder + It_Current_User->second.Title_Name_Key + L".png";
	Draw_User_Title_Image(image_path.c_str());  // Initialize Title Image Folder
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Active_Button(const EActive_Button &active_button)
{
	switch (Active_Menu)
	{
	case EAM_Main:
	case EAM_Watching:
		Draw_Active_Button(active_button, User_Array_Map);
		break;

	case EAM_Library_Menu:
		Draw_Active_Button(active_button, User_Library_Map);
		break;

	case EAM_Paused_Menu:
		Draw_Active_Button(active_button, User_Paused_Map);
		break;

	case EAM_Wishlist:
		Draw_Active_Button(active_button, User_Wishlist_Map);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Active_Button(const EActive_Button &active_button, std::map<std::wstring, SUser_Input_Data> &user_array)
{
	int title_name_length;

	title_name_length = 0;

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
			Draw_Button_Text(AsConfig::Brush_Green_Dark, AsConfig::Color_Text_Green, AsConfig::Color_Dark, User_Input_Rect[(int)active_button], It_Current_User->second.Title_Name_Num.c_str());
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
void AsUI_Builder::Draw_User_Input_Request()
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

	// Change Background
	SelectObject(Ptr_Hdc, AsConfig::Brush_Green_Dark);

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

	const DirectX::Image *img = image_title.GetImage(0, 0, 0);
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
void AsUI_Builder::Draw_Button_Text(const HBRUSH &background, const COLORREF &color_bk, const COLORREF &color_tx, const RECT &rect, const wchar_t *str) const
{
	SelectObject(Ptr_Hdc, background);
	SetBkColor(Ptr_Hdc, color_bk);
	SetTextColor(Ptr_Hdc, color_tx);

	Rectangle(Ptr_Hdc, rect.left, rect.top, rect.right, rect.bottom);  // draw rect
	TextOutW(Ptr_Hdc, rect.left + AsConfig::Global_Scale, rect.top + AsConfig::Global_Scale, str, (int)wcslen(str) );  // button_prev text ( x its text out in middle
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
void AsUI_Builder::Context_Menu_Draw(const int &x, const int &y)
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
	Context_Image_Save(context_rect);  // Save image to buffer while we can restore with condition

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
void AsUI_Builder::Context_Image_Save(const RECT &rect)
{
	Hdc_Memory = CreateCompatibleDC(Ptr_Hdc);
	if (!Hdc_Memory != 0)
		return;

	H_Bitmap = CreateCompatibleBitmap(Ptr_Hdc, rect.right - rect.left, rect.bottom - rect.top);
	if (!H_Bitmap != 0)
		return;

	Saved_Object = SelectObject(Hdc_Memory, H_Bitmap);
	BitBlt(Hdc_Memory, 0, 0, rect.right - rect.left, rect.bottom - rect.top, Ptr_Hdc, rect.left, rect.top, SRCCOPY);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Context_Image_Restore(RECT &rect)
{
	if (Hdc_Memory != 0 && H_Bitmap != 0 && Saved_Object != 0)
	{
		BitBlt(Ptr_Hdc, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, Hdc_Memory, 0, 0, SRCCOPY);
		SelectObject(Hdc_Memory, Saved_Object);
		DeleteObject(H_Bitmap);
		DeleteDC(Hdc_Memory);
		Hdc_Memory = 0;
		H_Bitmap = 0;
		Saved_Object = 0;
	}
	rect = {};  // обнуляем
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Handle()
{
	wchar_t *url_content;
	int length;

	if (wcsstr(User_Input, L"http://") != 0 || wcsstr(User_Input, L"https://") != 0)
	{// If it`s url use ACurl_Client to Get ID_Content, Get Title + Num + Season

		if (!std::filesystem::exists(AsConfig::Image_Folder) )
			std::filesystem::create_directories(AsConfig::Image_Folder);

		// If not threaded uselles sections except curl
		length = (int)wcslen(User_Input) + 1;
		url_content = new wchar_t[length]{};
		wcsncpy_s(url_content, length, User_Input, static_cast<rsize_t>(length) - 1);

		ACurl_Client client_url(EProgram::ASaver, url_content);  // Get content from url

		length = (int)wcslen(url_content) + 1;
		wcsncpy_s(User_Input, length, url_content, static_cast<rsize_t>(length) - 1);
		delete[] url_content;
	}

	// 2.0. Add User_Input to opened SubMenu
	switch (Active_Menu)  // With add to
	{
	case EAM_Watching:
		User_Map_Emplace(User_Array_Map, User_Input);
		break;

	case EAM_Library_Menu:
		User_Map_Emplace(User_Library_Map, User_Input);
		break;

	case EAM_Paused_Menu:
		User_Map_Emplace(User_Paused_Map, User_Input);
		break;

	case EAM_Wishlist:
		User_Map_Emplace(User_Wishlist_Map, User_Input);
		break;
	}

	// 3.0. Redraw All and Save
	Draw_Menu_Sub(Active_Menu);  // Redraw All Sub Menu
	Draw_User_Input_Button();  // Redrow User Input
	Handle_Active_Button_Advence();  // Show Active Button
	User_Map_Main_Save(Active_Menu);  // Save current map in used sub menu
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Value_Is_Changed(const bool is_increment)
{
	if (is_increment)
		It_Current_User->second.Title_Num++;
	else
		It_Current_User->second.Title_Num--;

	It_Current_User->second.Title_Name_Num.erase();

	if (It_Current_User->second.Title_Season != 0)
		It_Current_User->second.Title_Name_Num = It_Current_User->second.Title_Name_Key + L" " + AsConfig::Season_Case_Up[It_Current_User->second.Title_Season - 1] + L" " + std::to_wstring(It_Current_User->second.Title_Num);
	else
		It_Current_User->second.Title_Name_Num = It_Current_User->second.Title_Name_Key + L" " + std::to_wstring(It_Current_User->second.Title_Num);
}
//------------------------------------------------------------------------------------------------------------
bool AsUI_Builder::User_Input_Set_To_Clipboard()
{
	WCHAR *psz_text;
	if (!OpenClipboard(0) )
		return false;
	HANDLE handle_data = GetClipboardData(CF_UNICODETEXT);
	if (!handle_data != 0)
		return false;

	if (!(psz_text = static_cast<WCHAR*>(GlobalLock(handle_data) ) ) )
		return false;

	if (wcsstr(psz_text, L"http://") != 0 || wcsstr(psz_text, L"https://") != 0)  // If it`s url check it
	{
		if (!std::filesystem::exists(AsConfig::Image_Folder) )
			std::filesystem::create_directories(AsConfig::Image_Folder);

		if (psz_text != 0)
			wcsncpy_s(User_Input, wcslen(psz_text) + 1, psz_text, wcslen(psz_text) );
	}
	else
		EmptyClipboard();

	GlobalUnlock(handle_data);
	CloseClipboard();
	Draw_User_Input_Button();
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool AsUI_Builder::User_Input_Get_From_Clipboard(wchar_t *to_clipboard)
{// Need refactoring, can delete url

	size_t size_in_bytes;
	HGLOBAL global_handle;
	LPVOID global_ptr;

	if (!OpenClipboard(0) )
		return false;

	if (!EmptyClipboard() )  // clean, if cant close and return
		return CloseClipboard();

	size_in_bytes = (wcslen(to_clipboard) + 1) * sizeof(wchar_t);  // Вычисление размера памяти с учетом правильных скобок
	global_handle = GlobalAlloc(GMEM_MOVEABLE, size_in_bytes);

	if (!global_handle != 0)
		return CloseClipboard();

	global_ptr = GlobalLock(global_handle);
	if (!global_ptr)
		return CloseClipboard();

	memcpy(global_ptr, to_clipboard, size_in_bytes);  // Скопировать текст в выделенную память
	GlobalUnlock(global_handle);  // Разблокировать глобальную память
	wcsncpy_s(User_Input, wcslen(to_clipboard) + 1, to_clipboard, wcslen(to_clipboard));

	if (!SetClipboardData(CF_UNICODETEXT, global_handle))  // Поместить текст в буфер обмена
		GlobalFree(global_handle);  // Освободить глобальную память в случае ошибки

	CloseClipboard();
	Draw_User_Input_Button();
	return true;
}
//------------------------------------------------------------------------------------------------------------
unsigned long long AsUI_Builder::User_Map_Load_Convert(unsigned long long &ch)
{
	// 1.1 Russian
	if (ch >= 10 && ch <= 42)  // Rus Symb
	{
		if (ch >= 10 && ch <= 41)  // а - я
			return ch = (ch + 1000) + 30 + 32;  // a = 10 | я = 41
		else  // ё
			return ch = (ch + 1000) + 30 + 32 + 1;  // ё = 42
	}

	// 1.2. English
	if (ch >= 72 && ch <= 98)  // Eng Symb
	{
		if (ch >= 73 && ch <= 98)  // а = 97 || z = 122
			return ch = ch + 24;  // a = 73 | z = 98
		else  // `
			return ch = (ch + 24);  // ` = 72
	}

	// 1.3. Symbols
	if (ch >= 43 && ch <= 71)  // Standart Help symbols
	{
		if (ch >= 43 && ch <= 54)
			return ch = (ch + 5);  // 0 = 43 | 9 = 52 | : = 53 | ; = 54

		if (ch == 55)  // ?
			return ch = (ch + 8);  // ? = 55

		if (ch >= 56 && ch <= 71)  // space || ! " # ( ) * + , - . / $ %
			return ch = (ch - 24);  // space = 56 || / = 71
	}

	return 0LL;  // Bad Reserved 99
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Main_Load(std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path)
{
	bool is_add_to_user_array = false;
	wchar_t *user_input = new wchar_t[100]{};
	//wchar_t ch = 0;
	int how_much_g = 0;
	int block_sum_ull = 0;
	int block_sum_index = 0, str = 0;
	unsigned long long *ull_array_blocks = 0;
	unsigned long long ull_char = 0;
	unsigned long long ull_index = 0;
	unsigned long long ull_number = 0;
	
	std::ifstream infile(file_path, std::ios::binary);
	if (!infile)
		return;
	infile.seekg(0, std::ios::end);  // Go to last char in file
	how_much_g = (int)infile.tellg();    // How many char in file || Need use seekg
	block_sum_ull = how_much_g / sizeof(unsigned long long);  // (long long) 8 / size = how manny ULL in file
	infile.seekg(0, std::ios::beg);  // Go to first char in file
	ull_array_blocks = new unsigned long long[block_sum_ull];  // Get memory to cast 
	infile.read(reinterpret_cast<char *>(ull_array_blocks), how_much_g);  // reinterpret file size to ull by char

	while (block_sum_index < block_sum_ull)
	{
		ull_number = ull_array_blocks[block_sum_index];
		ull_index = AsConfig::ULL_Index_Length;

		while (ull_index != 0)
		{
			ull_char = ull_number / ull_index;
			ull_index /= 100;
			ull_char %= 100;

			while (ull_char == 0)
			{// If invalid ull_char need to find valid

				if (ull_index == 0)
					break;
				ull_char = ull_number / ull_index;
				ull_char %= 100;
				ull_index /= 100;
			}

			if (ull_char >= 43 && ull_char <= 52)  // 43 == 0 52 == 9
				is_add_to_user_array = true;

			if (is_add_to_user_array && ull_char > 52 || is_add_to_user_array && ull_char < 43)  // If after ull_number end
			{
				user_input[str] = L'\0';  // Title end here
				user_input[0] = user_input[0] - 32;  // Upper Case first char
				User_Map_Emplace(user_arr, user_input);  // Add to array
				is_add_to_user_array = false;  // look next numbers
				str = 0;
			}
			user_input[str++] = (wchar_t)User_Map_Load_Convert(ull_char);  // Convert to norm wchar_t and add to user_input
		}
		
		block_sum_index++;  // go to next index
		if (block_sum_index == block_sum_ull && user_input[0] != L'\0')
		{// If block last and user_input not empty save unsaved

			user_input[str] = L'\0';  // say it`s end
			user_input[0] = user_input[0] - 32;  // Set Upper Case first symbol
			User_Map_Emplace(user_arr, user_input);  // Emplace to map
		}
	}
	infile.close();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Main_Save(const EActive_Menu &active_menu)
{
	switch (active_menu)
	{
	case EAM_Watching:
		Thread_First = std::thread([&]() { User_Map_Init_Buffer(User_Array_Map, AsConfig::Folders_Save[active_menu]); });
		Thread_First.join();
		break;

	case EAM_Library_Menu:
		Thread_Second = std::thread([&]() { User_Map_Init_Buffer(User_Library_Map, AsConfig::Folders_Save[active_menu]); });
		Thread_Second.join();
		break;

	case EAM_Paused_Menu:
		Thread_Third = std::thread([&]() { User_Map_Init_Buffer(User_Paused_Map, AsConfig::Folders_Save[active_menu]); });
		Thread_Third.join();
		break;

	case EAM_Wishlist:
		Thread_Fourth = std::thread([&]() { User_Map_Init_Buffer(User_Wishlist_Map, AsConfig::Folders_Save[active_menu]); });
		Thread_Fourth.join();
		break;

	case EAM_Exit:
		Thread_First = std::thread([&]() { User_Map_Init_Buffer(User_Array_Map, AsConfig::Folders_Save[EActive_Menu::EAM_Watching]); });
		Thread_Second = std::thread([&]() { User_Map_Init_Buffer(User_Library_Map, AsConfig::Folders_Save[EActive_Menu::EAM_Library_Menu]); });
		Thread_Third = std::thread([&]() { User_Map_Init_Buffer(User_Paused_Map, AsConfig::Folders_Save[EActive_Menu::EAM_Paused_Menu]); });
		Thread_Fourth = std::thread([&]() { User_Map_Init_Buffer(User_Wishlist_Map, AsConfig::Folders_Save[EActive_Menu::EAM_Wishlist]); });

		Thread_First.join();
		Thread_Second.join();
		Thread_Third.join();
		Thread_Fourth.join();
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Init_Buffer(const std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path)
{
	int i = 0, len = 0;
	int user_array_size;
	wchar_t **user_array;
	std::string user_input_saved_folder = AsConfig::Path_Saves_Folder;

	if (!std::filesystem::exists(user_input_saved_folder) )  // if in Data
		std::filesystem::create_directory(user_input_saved_folder);

	user_input_saved_folder += file_path;  // add specific name
	if (user_arr.size() == 0)  // If array empty don`t need to save it
		if (std::filesystem::exists(user_input_saved_folder) )
			if(std::filesystem::remove(user_input_saved_folder) )  // If folder exist remove
				return;
			else
				return;

	user_array_size = (int)user_arr.size();  // How many pointers
	user_array = new wchar_t *[user_array_size]{};  // Create pointers

	for (auto &it : user_arr)
	{// Cpy Title with nums to user_array by pointers

		const std::wstring &key = it.second.Title_Name_Num;
		len = (int)key.length() + 1;
		if (i < user_array_size)
		{
			user_array[i] = new wchar_t[len]{};
			wcscpy_s(user_array[i], len, key.c_str() );
			i++;
		}
	}

	User_Map_Save_Array(user_input_saved_folder.c_str(), user_array, user_array_size);  // save data to disk
	
	for (int i = 0; i < user_array_size; i++)  // free memory
		delete[] user_array[i];
	delete[] user_array;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Save_Array(const char *file_path, wchar_t **user_array, int user_input_counter)
{
	int title_index = 0, title_index_length = 0;
	int number_index = 0;
	unsigned short ch_i = 0;
	unsigned long long numbers = 0;

	std::ofstream outfile(file_path, std::ios::out | std::ios::binary);  // Создаем новые данные
	if (!outfile)
		return;

	while (user_input_counter != 0)
	{// Titles counter

		while (user_array[title_index][title_index_length] != L'\0')
		{// Title length

			ch_i = User_Map_Save_Convert( (unsigned short)user_array[title_index][title_index_length++]);
			if (++number_index % 9 == 0)
			{
				numbers += ch_i;
				outfile.write(reinterpret_cast<const char*>(&numbers), sizeof(numbers) );
				numbers = 0;
			}
			else
				numbers = (numbers + ch_i) * 100;
		}
		title_index++;
		title_index_length = 0;
		user_input_counter--;
	}
	if (number_index % 9 == 0)
		outfile.close();
	else
		outfile.write(reinterpret_cast<const char*>(&numbers), sizeof(numbers) );
}
//------------------------------------------------------------------------------------------------------------
unsigned short AsUI_Builder::User_Map_Save_Convert(unsigned short ch)
{
	// 1.1 Russian || Other Languages
	if (ch >= 1025 && ch <= 1105)
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
	
	// 1.2. English
	if (ch >= 65 && ch <= 122)
	{
		// 3.0 English Symbols
		if (ch >= 97 && ch <= 122)  // а = 97 || z = 122
			return ch = ch - 24;  // a = 73 | z = 98

		if (ch >= 65 && ch <= 90)  // A = 65 Z = 90  | 25 
			return ch = ch + 8;  // A = 73 | Z = 98

		if (ch == 96)  // `
			return ch = (ch - 24);  // ` = 72
	}
	
	// 1.3. Symbols
	if (ch >= 32 && ch <= 63)
	{
		if (ch >= 32 && ch <= 47)  // space || ! " # ( ) * + , - . / $ %
			return ch = (ch + 24);  // space = 56 || / = 71

		if (ch >= 48 && ch <= 59)  // 0 - 9 - :
			return ch = (ch - 5);  // 0 = 43 | 9 = 52 | : = 53 | ; = 54

		if (ch == 63)  // ?
			return ch = (ch - 8);  // ? = 55
	}
	
	return 0;  // Reserved 99
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Emplace(std::map<std::wstring, SUser_Input_Data> &user_arr, wchar_t *user_input)
{
	int curr_it;
	SUser_Input_Data converted_data;

	curr_it = 0;
	converted_data = {};
	if (user_input[0] == L'\0')  // If pressed Enter while User_Input empty
		return;

	// 1.2  Check user_input orphography
	while (user_input[wcslen(user_input) - 1] == L' ')  // If last ch = space delete
		user_input[--User_Input_Len] = L'\0';

	// 1.3 Init_Data before set to map
	User_Input_Convert_Data(converted_data, user_input);

	// 1.4. Check if containts the same key if not save to map
	if (user_arr.contains(converted_data.Title_Name_Key) )
		user_arr[converted_data.Title_Name_Key] = converted_data;
	else
		user_arr.emplace(converted_data.Title_Name_Key, converted_data);  // if not add new title

	// 1.5. If from ACurl, saved picture rename, can`t save if invalid file path
	if (std::filesystem::exists("TemporaryName.png") )
	{
		try
		{
			std::filesystem::rename("TemporaryName.png", (std::wstring(AsConfig::Image_Folder) + converted_data.Title_Name_Key + std::wstring(L".png") ) );
		}
		catch (const std::exception &)
		{
			std::filesystem::remove("TemporaryName.png");  // !!! Save ID Content like name to picture, or rename text || Or find invalid, if has check
		}
	}

	// 1.6. Show and select new added title to window
	if (Active_Menu != -1)
	{
		It_Current_User = user_arr.find(user_input);
		Active_Button = (EActive_Button)std::distance(user_arr.begin(), It_Current_User);
		Sub_Menu_Curr_Page = ( (int)Active_Button - 1) / Sub_Menu_Max_Line;  // Find Page
	}

	// 1.7. Reset user_input
	User_Input_Len = 0;
	user_input[User_Input_Len] = L'\0';
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Erase()
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
	User_Map_Main_Save(Active_Menu);  // Save by Erase
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Convert_Data(SUser_Input_Data &ui_data, wchar_t *user_input)
{
	unsigned short current_ch;
	wchar_t *pattern_season;
	int current_user_input_length;
	int season_counter_index;

	pattern_season = 0;
	User_Input_Len = (int)wcslen(user_input) - 1;
	current_user_input_length = User_Input_Len;
	current_ch = (unsigned short)user_input[current_user_input_length];  // 48 0 57 9
	season_counter_index = 0;

	while (current_ch == L' ' || current_ch >= 48 && current_ch <= 57)
	{//Find spaces if current ch numeric

		if (current_ch == L' ')
		{// If space str to int

			if (ui_data.Title_Num == 0)
				ui_data.Title_Num = std::stoi(user_input + current_user_input_length + 1);  // if first ' ' get num
			else
				ui_data.Title_Season = std::stoi(user_input + current_user_input_length + 1);  // if second ' ' get seasons

			user_input[current_user_input_length] = L'\0';  // Hide nums
		}
		current_ch = (unsigned short)user_input[--current_user_input_length];
		while (user_input[current_user_input_length] == L'X' || user_input[current_user_input_length] == L'I' || user_input[current_user_input_length] == L'V')
		{
			season_counter_index++;
			user_input[current_user_input_length--] += L' ';
		}
		current_user_input_length += season_counter_index;
		while (user_input[current_user_input_length] == L'x' || user_input[current_user_input_length] == L'i' || user_input[current_user_input_length] == L'v')  // Find Seasons =) 
		{// Find next season character

			current_user_input_length--;
			if (user_input[current_user_input_length] == L' ')
			{// Find next space

				pattern_season = user_input + current_user_input_length + 1;
				for (int i = 0; i < 10; i++)
				{// Find Pattern in Config and set to data

					if (wcscmp(pattern_season, AsConfig::Season_Case_Low[i]) == 0)
					{
						ui_data.Title_Season = i + 1;
						break;
					}
				}
				user_input[current_user_input_length] = L'\0';  // erase season from user_input
			}
		}
	}
	
	// Initialize TITLE_NAME_NUM
	if (ui_data.Title_Num == 0)
		ui_data.Title_Num = 1;

	ui_data.Title_Name_Key = user_input;  // Init key
	ui_data.Title_Name_Num += ui_data.Title_Name_Key;
	ui_data.Title_Name_Num.append(L" ");
	if (ui_data.Title_Season != 0)
	{
		ui_data.Title_Name_Num += AsConfig::Season_Case_Up[ui_data.Title_Season - 1];
		ui_data.Title_Name_Num.append(L" ");
	}
	
	ui_data.Title_Name_Num += std::to_wstring(ui_data.Title_Num);
	ui_data.Title_Data;
}
//------------------------------------------------------------------------------------------------------------




// AsUI_Book_Reader
AsUI_Book_Reader::AsUI_Book_Reader(HDC hdc)
 : Ptr_Hdc(hdc)
{

}
//------------------------------------------------------------------------------------------------------------
//void AsUI_Book_Reader::Handle_Input(EKey_Type &key_type) const
//{
//	switch (key_type)
//	{
//	case EKT_None:
//		break;
//
//
//	case EKT_Draw_Main_Menu:
//	{
//		wchar_t temp_01[] = L"Welcome to the book reader, choose your option";
//
//		TextOutW(Ptr_Hdc, 15, 15, temp_01, (int)wcslen(temp_01) );
//	}
//	break;
//
//
//	case EKT_Enter:
//		break;
//
//
//	case EKT_Space:
//		break;
//
//
//	case EKT_LM_Down:
//		Rectangle(Ptr_Hdc, 66, 66, 99, 99);
//		break;
//
//
//	case EKT_RM_Down:
//		break;
//
//
//	case EKT_Redraw_User_Input:
//		break;
//
//
//	default:
//		break;
//	}
//}
////------------------------------------------------------------------------------------------------------------




// AsEngine
AsEngine::~AsEngine()
{
	delete UI_Builder;
}
//------------------------------------------------------------------------------------------------------------
AsEngine::AsEngine()
 : Is_After_Maximazied(true), UI_Builder(0), UI_Book_Reader(0), W_Param(0), L_Param(0),
	EBuilder_Handler(EUI_Builder_Handler::Draw_Menu_Main), Ptr_Hwnd(0), Ptr_Hdc(0), Paint_Struct{}
{
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame_ASaver(HWND hwnd)
{
	Ptr_Hwnd = hwnd;
	Ptr_Hdc = BeginPaint(Ptr_Hwnd, &Paint_Struct);

	if (UI_Builder != 0)
		UI_Builder->Builder_Handler(Ptr_Hdc, EBuilder_Handler, W_Param, L_Param);
	else
		UI_Builder = new AsUI_Builder(Ptr_Hdc, W_Param, L_Param);

	if (!Is_After_Maximazied)
	{
		Is_After_Maximazied = !Is_After_Maximazied;
		UI_Builder->Builder_Handler(Ptr_Hdc, EUI_Builder_Handler::Draw_Full_Window, W_Param, L_Param);

		return;
	}

	if (UI_Builder->Active_Menu == EAM_Exit)
		AsEngine::~AsEngine();
	EndPaint(Ptr_Hwnd, &Paint_Struct);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Redraw_Frame(const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam)
{
	W_Param = wParam;
	L_Param = lParam;

	InvalidateRect(Ptr_Hwnd, 0, FALSE);  // если во 2-м параметре указать RECT рисувать можно будет только в нем | TRUE если нужно стереть всё
	EBuilder_Handler = builder_handler;
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Get_Clipboard_From_Else()
{
	int index = 0;

	//if (OpenClipboard(0) )  // !!! Re-Made
	//{
	//	if (HANDLE hData = GetClipboardData(CF_UNICODETEXT) )
	//	{
	//		if (WCHAR *psz_text = static_cast<WCHAR*>(GlobalLock(hData) ) )
	//			while (psz_text[index] != '\0')
	//				UI_Builder->Handle_User_Input(psz_text[index++]);  // Set Text from clipboard
	//		
	//		GlobalUnlock(hData);
	//	}
	//	CloseClipboard();
	//}
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Get_Current_Data_Time()
{
	time_t curr_time = time(&curr_time);  // curr_time <- time
	tm s_local_time;  // crt struct 

	localtime_s(&s_local_time, &curr_time);  // s_local_time <- localtime_s <- curr_time

	int day = s_local_time.tm_mday;
	int month = s_local_time.tm_mon + 1; // Считаем месяцы с 0, поэтому добавляем 1
	int year = s_local_time.tm_year + 1900; // Добавляем 1900, чтобы получить текущий год
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

	EndPaint(Ptr_Hwnd, &Paint_Struct);
}
//------------------------------------------------------------------------------------------------------------
int AsEngine::Draw_Frame_Chooser_Main_Mane(HWND hwnd)
{
	//int i = 0;
	//const int round = 75;
	//const int x = 100;
	//const int width = 450;
	//const int height = 750;
	//const RECT rect_programs{ x, x, width, height };
	//RECT rect_intersecte{};
	//PAINTSTRUCT paint_struct;
	//HDC hdc;

	//const int cord_mouse_x = Mouse_Cord_X;
	//const RECT rect_mouse{ cord_mouse_x - 1, cord_mouse_x - 1, cord_mouse_x + 1, cord_mouse_x + 1 };
	//rect_intersecte = rect_programs;

	//for (i = 0; i < (int)EProgram::End; i++)
	//{
	//	const int width_offset = i * width;
	//				
	//	rect_intersecte.left += width_offset;
	//	rect_intersecte.right += width_offset;

	//	if (IntersectRect(&rect_intersecte, &rect_intersecte, &rect_mouse) )
	//		return i;
	//}
	//		
	//hdc = BeginPaint(hwnd, &paint_struct);  // Draw Chooser Main Menu
	//		
	//for (i = 0; i < (int)EProgram::End; i++)
	//{
	//	RoundRect(hdc, rect_programs.left + (i * width), rect_programs.top, rect_programs.right + (i * width), rect_programs.bottom, round, round);
	//	TextOutW(hdc, rect_programs.left + (i * width) + (width / 3), rect_programs.top + 70, AsConfig::Text_Program_Names[i], (int)wcslen(AsConfig::Text_Program_Names[i]) );
	//}

	//EndPaint(hwnd, &paint_struct);

	return -1;
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
