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

	delete Content_W;
}
//------------------------------------------------------------------------------------------------------------
ACurl_Client::ACurl_Client(const EProgram &program, wchar_t *&user_input)
  : ID_Content_Size(0), ID_Content_Array(0), Patterns_Array{}, Title_Result(user_input), Title_Site(0), Content_W(0)
{
	Init();

	switch (program)
	{
	case EProgram::Invalid:
		break;
	case EProgram::ASaver:
		CURL_Handler();
		break;
	case EProgram::ABook_Reader:
		break;
	case EProgram::End:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Init()
{
	AsTools::Format_Sub_WString(Title_Result, L"//", L"/", Title_Site);  // Receive Domain --- animevost.org ---
	Content_W = new std::wstring(L"Data/") ;
	*Content_W += Title_Site;  // get path Data/animevost.org to create folder
	*Content_W += L"/";
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::CURL_Handler()
{
	if(!std::filesystem::exists(*Content_W) )  // If new site create pattern in file, need handle it personal
		return Add_Pattern_File();

	Get_Patterns();
	Get_URL_Data();
	Get_Contents();
	Get_Title();
	Get_Image();
}
//------------------------------------------------------------------------------------------------------------
bool ACurl_Client::Erase_ID(const int &if_not_last_id_content)
{
	--ID_Content_Size;  // Get Array Size
	ID_Content_Array[if_not_last_id_content] = ID_Content_Array[ID_Content_Size--];  // set last to first short
	//CURL_Content_ID_Emplace();
	return true;
}
//------------------------------------------------------------------------------------------------------------
bool ACurl_Client::Make_URL(wchar_t *result, const int &id_content_index)
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
void ACurl_Client::Add_Pattern_File()
{
	std::filesystem::create_directories(*Content_W);
	*Content_W += AsConfig::Pattern_Default_TXT;

	std::ofstream write_file(*Content_W, std::ios::binary);
	if (!write_file)
		return;

	write_file.write(AsConfig::Pattern_Default_Anime_Bit, strlen(AsConfig::Pattern_Default_Anime_Bit) );
	write_file.close();
	
	wcsncpy_s(Title_Result, wcslen(AsConfig::Patterns_Info) + 1, AsConfig::Patterns_Info, wcslen(AsConfig::Patterns_Info) );  // Say user what need format .txt to set patterns
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_ID()
{
	char *c_id_content_data;
	char *c_id_content;
	unsigned short i;
	unsigned short index_counter;

	i = 0;
	index_counter = 0;
	if (ID_Content_Array != 0)
		delete[] ID_Content_Array;

	// 1.0. Read from file binary & Init ID_Content_Array
	std::ifstream read_file(*Content_W + AsConfig::Pattern_Default_Bin, std::ios::in | std::ios::binary);
	if (read_file)
	{
		read_file.seekg(0, std::ios::end);
		index_counter = (unsigned short)read_file.tellg();
		ID_Content_Size = index_counter / sizeof(unsigned short);
		read_file.seekg(0, std::ios::beg);
		
		ID_Content_Array = new unsigned short [ID_Content_Size + 1] {};
		read_file.read(reinterpret_cast<char *>(ID_Content_Array), index_counter);
		read_file.close();
	}
	else
		ID_Content_Array = new unsigned short[ID_Content_Size + 1] {};  // If file don`t exist or can`t open

	// 1.1. Extract ID_Content // !!! Must be personal for each site
	AsTools::Format_Wide_Char_To_Char(Title_Result, c_id_content_data);
	AsTools::Format_Sub_String(c_id_content_data, "/content/", "/", c_id_content);
	index_counter = std::stoi(c_id_content);  // Get ID_Content

	// 1.2. If find same id content don`t add new
	for (i = 0; i <= ID_Content_Size; ++i)  // while?
		if (ID_Content_Array[i] == index_counter)
			return;
	ID_Content_Array[ID_Content_Size] = index_counter;

	// 1.3. Save to file ID_Content_Array if not max size
	std::ofstream write_file(*Content_W + AsConfig::Pattern_Default_Bin, std::ios::out | std::ios::binary | std::ios::trunc);
	if (write_file)
	{
		if (ID_Content_Size != 65535)
			for (i = 0; i <= ID_Content_Size; ++i)
				write_file.write(reinterpret_cast<const char *>(&ID_Content_Array[i]), sizeof(ID_Content_Array[i]) );
		write_file.close();
	}

	delete[] c_id_content;
	delete[] c_id_content_data;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_Patterns()
{
	const int pattern_length = (int)EPatterns_Site::Last_Index;
	int size_needed;
	int index;
	int w_string_length;
	size_t bgn;
	size_t end;
	std::wstring wstring_t;
	std::string string_from_file;

	Patterns_Array = new wchar_t *[pattern_length] {};
	w_string_length = 0;
	index = 0;
	*Content_W += AsConfig::Pattern_Default_TXT;

	// 1.0. Read from file to string_from_file
	std::ifstream file(*Content_W, std::ios::binary);
	if (!file != 0)
		return;
	string_from_file = std::string( (std::istreambuf_iterator<char>(file) ), std::istreambuf_iterator<char>() );
	file.close();

	// 1.1. Convert string_from_file to wstring_t
	delete Content_W;
	size_needed = MultiByteToWideChar(CP_UTF8, 0, string_from_file.c_str(), (int)string_from_file.size(), 0, 0);
	Content_W = new std::wstring(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, string_from_file.c_str(), (int)string_from_file.size(), &(*Content_W)[0], size_needed);

	// 1.2. Copy from file to user input array, used like pattern to parsing sites
	size_needed--;  // don`t count last \n
	while (index < pattern_length)
	{
		bgn = Content_W->find(AsConfig::Patterns_Config[index]) + wcslen(AsConfig::Patterns_Config[index]);
		end = Content_W->find(AsConfig::Patterns_Config[index + 1], bgn);

		if (!(size_needed == end) )
			end -= 1;  // if last symbol don`t erase 1 char \n

		wstring_t = Content_W->substr(bgn, end - bgn).c_str();
		w_string_length = (int)wcslen(wstring_t.c_str() );
		
		Patterns_Array[index] = new wchar_t[w_string_length + 1] {};
		wcsncpy_s(Patterns_Array[index], (rsize_t)(w_string_length + 1), wstring_t.c_str(), w_string_length);
		index++;
	}
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_URL_Data() const
{
	char *url;
	int size;
	CURL *curl;
	CURLcode res;
	FILE *file;

	size = 0;
	AsTools::Format_Wide_Char_To_Char(Title_Result, url);
	curl = curl_easy_init();  // Init
	fopen_s(&file, AsConfig::Temporary_File_Name[0], "wb");

	curl_easy_setopt(curl, CURLOPT_URL, url);  // Options
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_To_File);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
	res = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);  // Cleaning
	if (file != 0)
		fclose(file);
	delete[] url;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_Contents()
{
	int line_to_save;
	int line_current;
	char *start_point = 0;
	std::string line_from_file;
	std::string content_from_file;

	AsTools::Format_Wide_Char_To_Char(Patterns_Array[(int)EPatterns_Site::Init_Load], start_point);  // set start ptr
	line_to_save = std::stoi(Patterns_Array[(int)EPatterns_Site::Init_Load_Line]);  // how many line need save
	line_current = 0;
	delete Content_W;

	// 1.0. Find pattern where from start add to string
	std::ifstream file(AsConfig::Temporary_File_Name[0], std::ios::binary);
	if (!file)
		return;
	
	while (std::getline(file, line_from_file) )
		if ( (line_current = (int)line_from_file.find(start_point) ) != std::string::npos)
			break;

	// 1.1. Add(Write) needed content to string
	do content_from_file += line_from_file + "\n";
	while (std::getline(file, line_from_file) && line_to_save--);

	file.close();
	std::filesystem::remove(AsConfig::Temporary_File_Name[0]);  // Remove, don`t need anymore
	if (content_from_file.empty() )
		return;

	// !!! 1.2. Conver from string to wstring and return like result || Make AsTools
	line_to_save = MultiByteToWideChar(CP_UTF8, 0, &content_from_file[0], (int)content_from_file.size(), 0, 0);
	Content_W = new std::wstring(line_to_save, 0);
	MultiByteToWideChar(CP_UTF8, 0, &content_from_file[0], (int)content_from_file.size(), &(*Content_W)[0], line_to_save);
	delete[] start_point;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_Title()
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

	// 2.0. Set Title and season to Title_Result
	title_name_length = (int)(title_name_end - title_name_bgn) + 1;  // Get title name and season length
	title_nums_len = (int)(title_num_end - title_num_bgn) + 1;  // Get title numbers length(series)
	wcsncpy_s(Title_Result, title_name_length, title_name_bgn, (rsize_t)title_name_length - 1);

	// 2.1. Set Title num(series) to result
	Title_Result += title_name_length - 1;  // find where we need to start put nums
	Title_Result[0] = L' ';  // remove '\0' to space
	Title_Result++;  // to next index
	wcsncpy_s(Title_Result, title_nums_len, title_num_bgn, (rsize_t)title_nums_len - 1);
	Title_Result -= title_name_length;
}
//------------------------------------------------------------------------------------------------------------
void ACurl_Client::Get_Image()
{
	char *c_url;
	wchar_t *w_url;
	FILE *file;
	CURL *url_easy;
	CURLcode response;
	std::wstring w_str_url;

	// 1.2. Create image url full with domein and convert to char
	AsTools::Format_Sub_WString(Content_W->c_str(), Patterns_Array[(int)EPatterns_Site::Image_Bgn], Patterns_Array[(int)EPatterns_Site::Image_End], w_url);
	w_str_url = std::wstring(AsConfig::Protocols[0]) + Patterns_Array[(int)EPatterns_Site::Init_URL] + w_url;
	AsTools::Format_Wide_Char_To_Char(w_str_url.c_str(), c_url);

	// 1.3. Init and Download
	curl_global_init(CURL_GLOBAL_DEFAULT);
	url_easy = curl_easy_init();

	fopen_s(&file, AsConfig::Image_Name_File, "wb");
	curl_easy_setopt(url_easy, CURLOPT_URL, c_url);  // save img
	curl_easy_setopt(url_easy, CURLOPT_WRITEFUNCTION, Write_To_File);
	curl_easy_setopt(url_easy, CURLOPT_WRITEDATA, file);
	response = curl_easy_perform(url_easy);  // Download image to file
	
	if (file != 0)
		fclose(file);
	curl_easy_cleanup(url_easy);
	delete[] c_url;
	delete[] w_url;
}
//------------------------------------------------------------------------------------------------------------
size_t ACurl_Client::Write_To_File(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}
//------------------------------------------------------------------------------------------------------------




// AsUI_Builder
AsUI_Builder::~AsUI_Builder()
{
	int i = 0;
	std::vector<std::thread> threads_temp;
	
	// 1.2 Save covered image sys
	if (Hdc_Memory != 0)
		DeleteDC(Hdc_Memory);
	if (H_Bitmap != 0)
		DeleteObject(H_Bitmap);

	// 1.3. Save and free memmory
	for (i = 0; i < (int)EUser_Arrays::EUA_Arrays_Count; ++i)
	{
		threads_temp.emplace_back([&, i]()
			{ 
				User_Map_Save(AsConfig::Saved_Path[i], *User_Maps[i]);
				User_Map_Free(*User_Maps[i]);  // Thread 1
			});
	}
	for (std::thread &thread : threads_temp)
		thread.join();

	// 1.4. Free memory
	for (i = 0; i < (int)EPress::Button_Not_Handled; i++)
		delete[] Borders_Rect[i];

	delete[] Borders_Rect;
	delete[] User_Input;
	delete[] User_Maps;
	delete[] Buttons;
}
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::AsUI_Builder(HDC hdc)
: Ptr_Hdc(hdc), Builder_State(EBuilder_State::EBS_Working), Active_Map(EUser_Arrays::EUA_Arrays_Count), Border_Pressed(EPress::Border_Menu_Main), 
  Buttons {}, User_Input(0), Button_User_Offset(0), Borders_Rect{}, Hdc_Memory(0), H_Bitmap(0), Saved_Object(0), User_Maps {}, It_User_Map_Active {}
{
	//const wchar_t test[] = L"https://anime-bit.ru/content/7003/";
	//wchar_t *temp = new wchar_t[90] {};

	//wcsncpy_s(temp, wcslen(test) + 1, test, wcslen(test) );
	//
	//ACurl_Client client_url(EProgram::ASaver, temp);  // Get content from url

	//int yy = 0;
	//delete[] temp;

	std::vector<std::thread> threads;

	User_Maps = new std::map<wchar_t *, STitle_Info *, SCmp_Char> *[(int)EUser_Arrays::EUA_Arrays_Count] {};

	for (int i = 0; i < (int)EUser_Arrays::EUA_Arrays_Count; ++i)
	{
		threads.emplace_back([&, i]()
			{
				User_Maps[i] = new std::map<wchar_t *, STitle_Info *, SCmp_Char> {};
				User_Map_Load(AsConfig::Saved_Path[i], *User_Maps[i]);
			});
	}

	Init();
	Draw_Buttons_Menu_Main();
	for (std::thread &thread : threads)
		thread.detach();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Builder_Handler(HDC ptr_hdc, const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam)
{
	Ptr_Hdc = ptr_hdc;

	switch (builder_handler)
	{
	case EUI_Builder_Handler::EBH_LM_Button:
	case EUI_Builder_Handler::EBH_RM_Button:
		Handle_Button_Bordered(builder_handler, lParam);  // lparam to glob param
		break;
	case EUI_Builder_Handler::EBH_UI_Button:
		Redraw_Button_User_Input(static_cast<wchar_t>(wParam) );
		break;

	case EUI_Builder_Handler::EBH_UI_Menu_Main:
		Active_Map = EUser_Arrays::EUA_Arrays_Count;
		Borders_Rect[(int)EPress::Border_Menu_Main][0] = {};
		Draw_Buttons_Menu_Main();
		Draw_Buttons_Menu_Sub();
		Redraw_Buttons_Menu_Main();
		break;
	case EUI_Builder_Handler::EBH_UI_Menu_Sub:
		Handle_User_Input();
		break;
	case EUI_Builder_Handler::EBH_UI_Maximazed:
		Borders_Rect[(int)EPress::Border_Menu_Main][0] = {};
		Draw_Buttons_Menu_Main();

		if (EUser_Arrays::EUA_Arrays_Count > Active_Map)
		{
			Draw_Buttons_Menu_Sub();
			Redraw_Buttons_Menu_Sub();  // Bad Draw Button even if not pressed
			Draw_Buttons_Request();
			Redraw_Buttons_Menu_Main();
		}
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Init()
{
	if (!std::filesystem::exists(AsConfig::Image_Folder) )
		std::filesystem::create_directories(AsConfig::Image_Folder);

	if (!std::filesystem::exists(AsConfig::Path_Saves_Folder) )
		std::filesystem::create_directories(AsConfig::Path_Saves_Folder);

	Borders_Rect = new RECT *[(int)EPress::Button_Not_Handled] {};  // Exit 7 last border
	Borders_Rect[(int)EPress::Border_Menu_Main] = new RECT {};
	Borders_Rect[(int)EPress::Border_Menu_Sub] = new RECT {};
	Borders_Rect[(int)EPress::Button_User_Input] = new RECT {};
	Borders_Rect[(int)EPress::Button_Reguest] = new RECT[2] {};  // Increase or Decrease Buttons
	Borders_Rect[(int)EPress::Button_Pages] = new RECT[3] {};
	Borders_Rect[(int)EPress::Buttons_User_Input] = new RECT[AsConfig::Max_Line] {};
	Borders_Rect[(int)EPress::Button_Menu_Main] = new RECT[AsConfig::Menu_Main_Button_Count] {};
	Borders_Rect[(int)EPress::Button_Context] = new RECT[AsConfig::Context_Button_Count] {};
	Borders_Rect[(int)EPress::Border_Menu_Context] = new RECT {};
	Borders_Rect[(int)EPress::Mouse_Coordinate] = new RECT {};

	Borders_Rect[(int)EPress::Button_Pages][EPage_Button::EPB_Update] = { 1140, 12, 1228, 30 };
	Borders_Rect[(int)EPress::Button_Pages][EPage_Button::EPB_Prev] = { 1231, 12, 1303, 30 };
	Borders_Rect[(int)EPress::Button_Pages][EPage_Button::EPB_Next] = { 1305, 12, 1375, 30 };

	Buttons = new byte[(int)EActive_Button::EAB_Handlers_Count] {};
	User_Input = new wchar_t[128] {};
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Border()
{
	bool is_sub_menu;
	int scale;
	int x_cord;
	int border_width;
	int border_height;
	RECT *border_rect;

	x_cord = 0;
	is_sub_menu = false;
	scale = AsConfig::Global_Scale;
	border_rect = 0;

	if (!IsRectEmpty(&Borders_Rect[(int)EPress::Border_Menu_Main][0]) )  // if not main menu we must reset setting
	{// Draw Sub Menu
		border_rect = &Borders_Rect[(int)EPress::Border_Menu_Sub][0];
		x_cord = Borders_Rect[(int)EPress::Border_Menu_Main][0].right + AsConfig::Global_Scale;
		border_width = (AsConfig::Window_Width - x_cord) - 23 * 2 - 9;  // window_width - main menu width
		border_height = AsConfig::Window_Height + 30 - scale;
		is_sub_menu = true;
		SelectObject(Ptr_Hdc, AsConfig::Brush_Green);
	}
	else
	{// Draw Main Menu
		border_rect = &Borders_Rect[(int)EPress::Border_Menu_Main][0];
		border_width = (AsConfig::Menu_Main_Title_Length + scale) * AsConfig::Ch_W;  // 424
		border_height = (AsConfig::Menu_Main_Button_Count + 2) * (AsConfig::Ch_H + 6) + scale + 1;
	}

	// 1.2. Draw First Border
	border_rect->left = x_cord + scale;
	border_rect->top = scale;
	border_rect->right = x_cord + border_width;
	border_rect->bottom = border_height;
	Rectangle(Ptr_Hdc, border_rect->left, border_rect->top, border_rect->right, border_rect->bottom);

	// 1.2. Draw Second Border
	border_rect->left = border_rect->left + scale;
	border_rect->top = border_rect->top + scale;
	border_rect->right = border_rect->right - scale;
	border_rect->bottom = border_rect->bottom - scale;
	Rectangle(Ptr_Hdc, border_rect->left, border_rect->top, border_rect->right, border_rect->bottom);
	if (is_sub_menu != true)  // if not submenu return
		return;

	// 2.1. Fill Rect
	border_rect->left = border_rect->left + 1;
	border_rect->top = border_rect->top + 1;
	border_rect->right = border_rect->right - 1;
	border_rect->bottom = border_rect->bottom - 1;
	FillRect(Ptr_Hdc, border_rect, AsConfig::Brush_Background_Dark);  // Fill sub_menu to grey menu
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button(RECT &border_rect, RECT &button, const wchar_t *title_name) const
{
	int scale;
	int button_w, button_h;

	scale = AsConfig::Global_Scale;
	button_w = border_rect.right - border_rect.left - scale;  // button width
	button_h = border_rect.top + AsConfig::Ch_H + scale * 2;  // button height

	button.left = border_rect.left + scale;  // x = 3px from curr border
	button.top = border_rect.top;
	button.right = button.left + button_w - scale;  // border width - offset to draw button)
	button.bottom = button_h;
	Rectangle(Ptr_Hdc, button.left, button.top, button.right, button.bottom);  // draw button
	TextOutW(Ptr_Hdc, button.left + scale, button.top + scale, title_name, (int)wcslen(title_name) );

	border_rect.top = button.bottom + scale;  // cuting border for next button
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button_Text(const bool is_dark, const RECT &rect, const wchar_t *str) const
{
	HBRUSH test = is_dark ? AsConfig::Brush_Background_Dark : AsConfig::Brush_Green_Dark;
	COLORREF bk_color = is_dark ? AsConfig::Color_Dark : AsConfig::Color_Text_Green;
	COLORREF text_color = is_dark ? AsConfig::Color_Text_Green : AsConfig::Color_Dark;

	SelectObject(Ptr_Hdc, test);
	SetBkColor(Ptr_Hdc, bk_color);
	SetTextColor(Ptr_Hdc, text_color);

	Rectangle(Ptr_Hdc, rect.left, rect.top, rect.right, rect.bottom);  // draw rect
	TextOutW(Ptr_Hdc, rect.left + AsConfig::Global_Scale, rect.top + AsConfig::Global_Scale, str, (int)wcslen(str) );  // button_prev text ( x its text out in middle
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Buttons_Request()
{
	int button_offset;
	int box_size;
	int scale;
	int half_box;
	int prev_button;
	RECT *ui_rect_offset = 0;

	button_offset = 9;
	box_size = 21;
	half_box = (int)( (float)box_size / 2.0f);
	scale = AsConfig::Global_Scale;
	prev_button = Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] - Button_User_Offset;
	
	// 1.1 Draw first Rectangle decrease
	SelectObject(Ptr_Hdc, AsConfig::Brush_Green_Dark);  // Select color
	ui_rect_offset = &Borders_Rect[(int)EPress::Button_Reguest][0];
	ui_rect_offset->left = Borders_Rect[(int)EPress::Buttons_User_Input][prev_button].right + button_offset;
	ui_rect_offset->top = Borders_Rect[(int)EPress::Buttons_User_Input][prev_button].top;
	ui_rect_offset->right = Borders_Rect[(int)EPress::Buttons_User_Input][prev_button].right + button_offset + box_size;
	ui_rect_offset->bottom = Borders_Rect[(int)EPress::Buttons_User_Input][prev_button].top + box_size;

	Rectangle(Ptr_Hdc, ui_rect_offset->left, ui_rect_offset->top, ui_rect_offset->right, ui_rect_offset->bottom);
	MoveToEx(Ptr_Hdc, ui_rect_offset->left + scale, ui_rect_offset->top + half_box, 0);
	LineTo(Ptr_Hdc, ui_rect_offset->right - scale, ui_rect_offset->top + half_box);

	// 1.2. Draw Second rect increase
	Borders_Rect[(int)EPress::Button_Reguest][1].left = ui_rect_offset->left + box_size + scale;
	Borders_Rect[(int)EPress::Button_Reguest][1].top = ui_rect_offset->top;
	Borders_Rect[(int)EPress::Button_Reguest][1].right = ui_rect_offset->right + box_size + scale;
	Borders_Rect[(int)EPress::Button_Reguest][1].bottom = ui_rect_offset->bottom;
	ui_rect_offset = &Borders_Rect[(int)EPress::Button_Reguest][1];
	
	Rectangle(Ptr_Hdc, ui_rect_offset->left, ui_rect_offset->top, ui_rect_offset->right, ui_rect_offset->bottom);
	MoveToEx(Ptr_Hdc, ui_rect_offset->left + scale, ui_rect_offset->top + half_box, 0);
	LineTo(Ptr_Hdc, ui_rect_offset->right - scale, ui_rect_offset->top + half_box);
	MoveToEx(Ptr_Hdc, ui_rect_offset->left + half_box, ui_rect_offset->top + scale,0);
	LineTo(Ptr_Hdc, ui_rect_offset->left + half_box, ui_rect_offset->bottom - scale);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Buttons_Menu_Main()
{
	int x, y;
	int str_length;
	int titles_length;
	RECT border_rect;
	
	x = 0, y = 0;
	str_length = 0;
	border_rect = {};
	titles_length = (int)wcslen(AsConfig::Main_Menu_Title_Name);
	str_length = AsConfig::Ch_W * titles_length;  // ch width = 8 pixels * titles length in ch

	// 1.0. Set How to draw text and bg
	SelectObject(Ptr_Hdc, AsConfig::Brush_Background_Dark);  // Background
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);  // Text Background
	SetTextColor(Ptr_Hdc, AsConfig::Color_Text_Green);  // Text Color

	// 1.1. Draw Border and Text title
	Draw_Border();
	border_rect = Borders_Rect[(int)EPress::Border_Menu_Main][0];
	x = (border_rect.right - str_length) / 2;  // Get the center of first middle border and half of title
	y = border_rect.top + AsConfig::Global_Scale;  // Get offset from border top
	TextOutW(Ptr_Hdc, x, y, AsConfig::Main_Menu_Title_Name, titles_length);
	border_rect.top = y + AsConfig::Ch_H + AsConfig::Global_Scale;  // go to next line

	// 1.2. Set Buttons in border and save they`re cords in Borders_Rect[(int)EPress::Button_Menu_Main]
	for (int i = 0; i < AsConfig::Menu_Main_Button_Count; i++)
		Draw_Button(border_rect, Borders_Rect[(int)EPress::Button_Menu_Main][i], AsConfig::Menu_Main_Buttons_Text_Eng[i]);

	// 1.3. Draw Image
	Redraw_Image();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Buttons_Menu_Sub()
{
	int curr_line;
	int curr_page_max_line;
	RECT border_rect;
	std::map<wchar_t *, STitle_Info *, SCmp_Char>::iterator it;

	curr_line = 0;
	curr_page_max_line = 0;
	border_rect = {};

	// 1.1.Draw Border, Set colors, Draw Titles and user input handler
	Draw_Border();  // draw border
	border_rect = Borders_Rect[(int)EPress::Border_Menu_Sub][0];
	border_rect.top += AsConfig::Global_Scale;  // without title? i can fix but it`s look good enough
	
	SelectObject(Ptr_Hdc, AsConfig::Brush_Background_Dark);
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);
	SetTextColor(Ptr_Hdc, AsConfig::Color_Text_Green);
	Draw_Button(border_rect, Borders_Rect[(int)EPress::Button_User_Input][0], AsConfig::Sub_Menu_Title);  // Write Sub menu title
	Draw_Button(border_rect, Borders_Rect[(int)EPress::Button_User_Input][0], AsConfig::Sub_Menu_User_Input_Title);  // Write User Input Handler

	// 1.3. Handle Page setting if more that we can have do nothing
	it = User_Maps[(int)Active_Map]->begin();
	while(Button_User_Offset > User_Maps[(int)Active_Map]->size() )  // if next page don`t have buttons to draw stay at current page
		Button_User_Offset -= AsConfig::Max_Line;
	std::advance(it, Button_User_Offset);

	// 1.4 Draw Button Pages
	for (int i = 0; i < EPage_Button::EPB_Last; i++)
	{
		const RECT *button = &Borders_Rect[(int)EPress::Button_Pages][i];
		Rectangle(Ptr_Hdc, button->left, button->top, button->right, button->bottom);
		TextOutW(Ptr_Hdc, button->left + 1, button->top + 1, AsConfig::Battons_Page_Name[i], i == 0 ? 11 : 9);
	}

	// 1.4. Draw All buttons
	for (curr_line = 0; it != User_Maps[(int)Active_Map]->end(); ++it)
	{
		if (curr_line < AsConfig::Max_Line)
			Draw_Button(border_rect, Borders_Rect[(int)EPress::Buttons_User_Input][curr_line], it->second->Title_Name_Num);
		else
			return;
		
		curr_line++;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button_User_Input() const
{
	RECT *user_input = &Borders_Rect[(int)EPress::Button_User_Input][0];

	SelectObject(Ptr_Hdc, AsConfig::Brush_Background_Dark);
	Rectangle(Ptr_Hdc, user_input->left, user_input->top, user_input->right, user_input->bottom);
	
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);  // Set Text Background
	SetTextColor(Ptr_Hdc, AsConfig::Color_Yellow);  // Set Text color

	const int offset_x = user_input->left + AsConfig::Global_Scale;  // ofset from left
	const int offset_y = user_input->top + AsConfig::Global_Scale;

	if (User_Input[0] != 0)
		TextOutW(Ptr_Hdc, offset_x, offset_y, User_Input, (int)wcslen(User_Input) );  // write text in rect
	else
		TextOutW(Ptr_Hdc, offset_x, offset_y, AsConfig::Sub_Menu_User_Input_Title, (int)wcslen(AsConfig::Sub_Menu_User_Input_Title) );
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Buttons_Menu_Context()
{
	int button_text_len;
	int scale;
	int context_offset;
	int user_input_len;
	int button_heigh;
	RECT *context_rect;
	
	context_rect = &Borders_Rect[(int)EPress::Border_Menu_Context][0];
	button_text_len = (int)wcslen(AsConfig::Menu_Main_Buttons_Text_Eng[3]);
	scale = AsConfig::Global_Scale;
	context_offset = 6;
	user_input_len = button_text_len * AsConfig::Ch_W + context_offset;
	button_heigh = AsConfig::Ch_H * AsConfig::Context_Button_Count + context_offset;

	// 1. Start point where RMB pressed, then we take longest word and add his len like width
	context_rect->left = Borders_Rect[(int)EPress::Mouse_Coordinate]->right;
	context_rect->top = Borders_Rect[(int)EPress::Mouse_Coordinate]->top;
	context_rect->right = context_rect->left + user_input_len;
	context_rect->bottom = context_rect->top + button_heigh;

	// 3. Before draw Context Menu save Image where we draw it to reload
	Hdc_Memory = CreateCompatibleDC(Ptr_Hdc);
	if (!Hdc_Memory != 0)
		return;

	H_Bitmap = CreateCompatibleBitmap(Ptr_Hdc, context_rect->right - context_rect->left, context_rect->bottom - context_rect->top);
	if (!H_Bitmap != 0)
		return;

	Saved_Object = SelectObject(Hdc_Memory, H_Bitmap);
	BitBlt(Hdc_Memory, 0, 0, context_rect->right - context_rect->left, context_rect->bottom - context_rect->top, Ptr_Hdc, context_rect->left, context_rect->top, SRCCOPY);

	// 4. Draw Context Menu
	Rectangle(Ptr_Hdc, context_rect->left, context_rect->top, context_rect->right, context_rect->bottom);

	for (int i = 0; i < AsConfig::Context_Button_Count; i++)
	{
		button_text_len = (int)wcslen(AsConfig::Menu_Main_Buttons_Text_Eng[i]);
		TextOutW(Ptr_Hdc, context_rect->left + scale + 1, context_rect->top + AsConfig::Ch_H * i + scale + 1, AsConfig::Menu_Main_Buttons_Text_Eng[i], button_text_len);

		// 4.1. Save Context Buttons Rects || handle when LMB pressed
		Borders_Rect[(int)EPress::Button_Context][i].left = context_rect->left;
		Borders_Rect[(int)EPress::Button_Context][i].top = context_rect->top;
		Borders_Rect[(int)EPress::Button_Context][i].right = context_rect->right;
		Borders_Rect[(int)EPress::Button_Context][i].bottom = context_rect->top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H;
		if (!(i < AsConfig::Context_Button_Count - 1) )
			break;

		MoveToEx(Ptr_Hdc, context_rect->left + scale, context_rect->top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H, 0);
		LineTo(Ptr_Hdc, context_rect->left + scale + button_text_len * 8, context_rect->top + AsConfig::Ch_H * i + scale + AsConfig::Ch_H);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Buttons_Menu_Context()
{
	RECT &rect = Borders_Rect[(int)EPress::Border_Menu_Context][0];

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
void AsUI_Builder::Redraw_Button_User_Input(const wchar_t &user_text)
{
	wchar_t *to_map;
	const int user_input_lenght = (int)wcslen(User_Input);

	if (user_text == '\r')  // if press enter
	{
		to_map = new wchar_t[wcslen(User_Input) + 1] {};
		wcsncpy_s(to_map, wcslen(User_Input) + 1, User_Input, wcslen(User_Input) );

		Handle_Title_Info(to_map);
		Draw_Buttons_Menu_Sub();

		do *(User_Input++) = '\0';   // Clear User Input to \0
		while (*User_Input != '\0');

		User_Input -= user_input_lenght;
		return;
	}

	if (Active_Map == EUser_Arrays::EUA_Arrays_Count)  // main menu
		return;

	if (user_text != '\b' && user_input_lenght < AsConfig::User_Input_Buffer)
		User_Input[user_input_lenght] = user_text;
	else if (user_input_lenght > 0)
		User_Input[user_input_lenght - 1] = '\0';  // delete prev user_text

	Draw_Button_User_Input();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Button_User_Input_Added()
{
	Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] = (int)std::distance(User_Maps[(int)Active_Map]->begin(), It_User_Map_Active);

	// 1.0. If button must be on next page
	if (Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] > AsConfig::Max_Line)
	{
		Button_User_Offset += AsConfig::Max_Line;
		Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] = Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr];
	}

	// 1.1. If button must be on prev page and not less than 0
	if (Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] < AsConfig::Max_Line && Button_User_Offset > 0)
	{
		Button_User_Offset -= AsConfig::Max_Line;
		Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] = Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr];
	}

	// 1.2. If new title redraw, if old just redraw button?
	if (true)  // is new
		Draw_Buttons_Menu_Sub();  // !!! How to know new or old
	Redraw_Buttons_Menu_Sub();  // Show new added title
	Redraw_Image();  // Show Image
	Redraw_Button_Request();  // Clear old request
	Draw_Buttons_Request();  // Draw new request
	User_Input[0] = L'\0';
	Draw_Button_User_Input();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Image() const
{
	int width = 0, height = 0, bpp = 0;
	DXGI_FORMAT format {};
	RECT img_cords {};
	std::filesystem::path image_path {};
	BITMAPINFO bmi = {};
	DirectX::ScratchImage image_title {};

	//if (Active_Map == EUser_Arrays::EUA_Arrays_Count)
	if (Border_Pressed == EPress::Border_Menu_Main)
		image_path = AsConfig::Main_Image_Folder;
	else
		image_path = AsConfig::Image_Folder + std::wstring(It_User_Map_Active->second->Title_Name_Key) + AsConfig::Image_Format;

	DirectX::LoadFromWICFile(image_path.c_str(), DirectX::WIC_FLAGS_NONE, 0, image_title);

	const DirectX::Image *img = image_title.GetImage(0, 0, 0);
	if (!img != 0)
		return;

	format = img->format;
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
	FillRect(Ptr_Hdc, &img_cords, AsConfig::Brush_Background_Dark);

	img_cords.right = width;  // 415
	img_cords.bottom = height;  // 636

	StretchDIBits(Ptr_Hdc, img_cords.left, img_cords.top, img_cords.right, img_cords.bottom, 0, 0, width, height, img->pixels, &bmi, DIB_RGB_COLORS, SRCCOPY);

	image_title.Release();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Button_Request() const
{
	FillRect(Ptr_Hdc, &Borders_Rect[(int)EPress::Button_Reguest][0], AsConfig::Brush_Background_Dark);
	FillRect(Ptr_Hdc, &Borders_Rect[(int)EPress::Button_Reguest][1], AsConfig::Brush_Background_Dark);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Buttons_Menu_Main()
{
	const RECT &prev_button = Borders_Rect[(int)EPress::Button_Menu_Main][Buttons[(int)EActive_Button::EAB_Menu_Main_Prev]];
	const RECT &curr_button = Borders_Rect[(int)EPress::Button_Menu_Main][Buttons[(int)EActive_Button::EAB_Menu_Main_Curr]];

	Redraw_Button_Request();
	Draw_Button_Text(true, prev_button, AsConfig::Menu_Main_Buttons_Text_Eng[Buttons[(int)EActive_Button::EAB_Menu_Main_Prev]]);
	Draw_Button_Text(false, curr_button, AsConfig::Menu_Main_Buttons_Text_Eng[Buttons[(int)EActive_Button::EAB_Menu_Main_Curr]]);

	Buttons[(int)EActive_Button::EAB_Menu_Main_Prev] = Buttons[(int)EActive_Button::EAB_Menu_Main_Curr];
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Redraw_Buttons_Menu_Sub()
{
	wchar_t *text;
	int buttons_count = 0;
	const RECT &prev_button = Borders_Rect[(int)EPress::Buttons_User_Input][(int)Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] - Button_User_Offset];
	const RECT &curr_button = Borders_Rect[(int)EPress::Buttons_User_Input][(int)Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] - Button_User_Offset];

	while (buttons_count < 2)
	{
		It_User_Map_Active = User_Maps[(int)Active_Map]->begin();  // Clear button
		std::advance(It_User_Map_Active, buttons_count == 0 ? Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] : Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr]);
		text = It_User_Map_Active->second->Title_Name_Num;
		Draw_Button_Text(buttons_count == 0 ? true : false, buttons_count == 0 ? prev_button : curr_button, text);
		buttons_count++;
	}
	Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] = Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr];
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Button_Bordered(const EUI_Builder_Handler &builder_handler, const LPARAM &lParam)
{
	const int x = lParam & 0xffff;
	const int y = (int)(lParam >> 16);
	int index = 0;
	RECT mouse_cord_destination {};

	Borders_Rect[(int)EPress::Mouse_Coordinate]->left = x - 1;
	Borders_Rect[(int)EPress::Mouse_Coordinate]->top = y;
	Borders_Rect[(int)EPress::Mouse_Coordinate]->right = x;
	Borders_Rect[(int)EPress::Mouse_Coordinate]->bottom = y + 1;

	// 1.0. Border Chooser : at which border pressed, get index
	for (index = 0; index < (int)EPress::Non_Bordered; index++)
		if (IntersectRect(&mouse_cord_destination, Borders_Rect[(int)EPress::Mouse_Coordinate], &Borders_Rect[index][0] ) )
			break;

	// 1.1. Context Menu if rect is not empty clear
	if (Borders_Rect[(int)EPress::Border_Menu_Context][0].left != 0)
		Redraw_Buttons_Menu_Context();

	Border_Pressed = (EPress)index;
	Handle_Border_Pressed();
	if (builder_handler == EUI_Builder_Handler::EBH_RM_Button)  // If RM Button
		Draw_Buttons_Menu_Context();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Border_Pressed()
{
	switch (Border_Pressed)
	{
	case EPress::Border_Menu_Main:
		Handle_Menu_Main();
		if (Active_Map == EUser_Arrays::EUA_Quit_Button || Active_Map == EUser_Arrays::EUA_Arrays_Count)
			return;
		Draw_Buttons_Menu_Sub();
		Redraw_Buttons_Menu_Main();  // Redraw pressed button
		break;
	case EPress::Border_Menu_Context:
		Handle_Menu_Context();
		Redraw_Button_Request();
		Draw_Buttons_Menu_Sub();
		break;
	case EPress::Border_Menu_Sub:
		Handle_Menu_Sub();  // Change Border pressed State to User_Input | _Second | Button_Pages | Buttons_User_Input
		Handle_Border_Pressed();  // why this here?
		break;
	case EPress::Non_Bordered:
		Handle_Non_Bordered();
		break;
	case EPress::Button_User_Input:
		Draw_Button_User_Input();
		break;
	case EPress::Button_User_Input_Second:
		Redraw_Button_User_Input_Added();
		break;
	case EPress::Button_Reguest:
		break;
	case EPress::Button_Pages:
		Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] = 0;
		Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] = Button_User_Offset;
		Redraw_Button_Request();
		Draw_Buttons_Menu_Sub();
		break;
	case EPress::Buttons_User_Input:
		Redraw_Buttons_Menu_Sub();
		Redraw_Button_Request();
		Draw_Buttons_Request();
		Redraw_Image();
		break;
	}
	Border_Pressed = EPress::Non_Bordered;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Border_Pressed_Index(const int border_index, const int border_length, int& result) const
{
	RECT mouse_cord_destination{};

	for (result = 0; result < border_length; result++)
		if (IntersectRect(&mouse_cord_destination, Borders_Rect[(int)EPress::Mouse_Coordinate], &Borders_Rect[border_index][result]))
			break;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Button_Request(const bool is_increment)
{
	int title_num_last_index, title_length;
	int *ptr_title_num;
	wchar_t *title_num, *ptr, *ptr_buffer;
	const wchar_t compare_to = is_increment ? L'9' : L'0';
	const int decrease_to = is_increment ? -9 : 9;
	const int increase_to = is_increment ? 1 : -1;

	ptr = 0;
	ptr_buffer = 0;
	ptr_title_num = &It_User_Map_Active->second->Title_Num;
	*ptr_title_num += increase_to;
	title_num = It_User_Map_Active->second->Title_Name_Num;
	title_num_last_index = (int)wcslen(title_num) - 1;
	title_num += title_num_last_index;

	do
	{
		if (*title_num == compare_to)  // if not 9 or 0
			*title_num += decrease_to;  // from 9 to 0 or from 0 to 9
		else
			break;
	} while ( *(title_num -= 1) != L' ');

	*title_num += increase_to;

	if ( *title_num == L'!')  // From 99 to 100
	{// memloc | resize data and add new data
		title_length = (int)wcslen(It_User_Map_Active->second->Title_Name_Num) + 2;
		ptr = new wchar_t[title_length] {};
		title_num_last_index = (int)(title_num - It_User_Map_Active->second->Title_Name_Num);

		wcsncpy_s(ptr, title_length, It_User_Map_Active->second->Title_Name_Num, title_num_last_index);
		ptr[title_length - 1] = L'\0';
		ptr_buffer = ptr + title_num_last_index;
		*(ptr_buffer++) = L' ';
		ptr_buffer[0] = L'1';
		while ( *(++ptr_buffer) != L'\0')
			*ptr_buffer = L'0';

		delete[] It_User_Map_Active->second->Title_Name_Num;
		It_User_Map_Active->second->Title_Name_Num = ptr;
	}

	if (*title_num == L'0')  // From 100+ to 99 format str
		while (*title_num != L'\0')
			*(title_num++) = title_num[1];
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Main()
{
	int button_index = 0;

	Handle_Border_Pressed_Index( (int)EPress::Button_Menu_Main, AsConfig::Menu_Main_Button_Count, button_index);
	if ( !(button_index != AsConfig::Menu_Main_Button_Count) )  // if less buttons we have
		return;

	if ( (Active_Map = (EUser_Arrays)button_index) == EUser_Arrays::EUA_Quit_Button)
	{
		Builder_State = EBuilder_State::EBS_Exit;
		return PostQuitMessage(0);  // If pressed at button Exit
	}
	Buttons[(int)EActive_Button::EAB_Menu_Main_Curr] = button_index;
	It_User_Map_Active = User_Maps[(int)Active_Map]->begin();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Sub()
{
	int i;
	RECT mouse_cord_destination;

	i = 0;
	mouse_cord_destination = {};
	Border_Pressed = EPress::Non_Bordered;

	// 1.1. User Input Button Handler
	if (IntersectRect(&mouse_cord_destination, Borders_Rect[(int)EPress::Mouse_Coordinate], &Borders_Rect[(int)EPress::Button_User_Input][0]) )
		if (User_Input[0] != 0)
			return Handle_User_Input();
		else
			return Handle_Clipboard();

	// 1.2. Next or Prev page Handler
	Handle_Border_Pressed_Index( (int)EPress::Button_Pages, EPage_Button::EPB_Last, i);
	if (i != EPage_Button::EPB_Last)
	{
		if (i == EPage_Button::EPB_Prev)
			if (Button_User_Offset < 0)
				return;
			else
				Button_User_Offset -= AsConfig::Max_Line;

		if (i == EPage_Button::EPB_Next && User_Maps[(int)Active_Map]->size() > (size_t)(AsConfig::Max_Line + Button_User_Offset) )
			Button_User_Offset += AsConfig::Max_Line;
		Border_Pressed = EPress::Button_Pages;
		return;
	}

	// 1.3. Buttons  Sub Title
	Handle_Border_Pressed_Index( (int)EPress::Buttons_User_Input, AsConfig::Max_Line, (i = 0) );
	if (i >= AsConfig::Max_Line)  // if out of border, miss hit on button
		return;
	Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] = i + Button_User_Offset;
	Border_Pressed = EPress::Buttons_User_Input;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Clipboard()
{
	WCHAR *psz_text;
	HANDLE handle_data;

	if (!OpenClipboard(0) )
		return;

	handle_data = GetClipboardData(CF_UNICODETEXT);
	if (!handle_data != 0)
		return;

	if (!(psz_text = static_cast<WCHAR*>(GlobalLock(handle_data) ) ) )
		return;

	if (!wcsstr(psz_text, AsConfig::Protocols[1]) != 0 && !wcsstr(psz_text, AsConfig::Protocols[0]) != 0)  // try to find proto in text
		EmptyClipboard();
	else
		if (psz_text != 0)
			wcsncpy_s(User_Input, wcslen(psz_text) + 1, psz_text, wcslen(psz_text) );

	GlobalUnlock(handle_data);
	CloseClipboard();
	Border_Pressed = EPress::Button_User_Input;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Context()
{
	int i = 0;
	std::map<wchar_t *, STitle_Info *, SCmp_Char> *map = 0;

	Handle_Border_Pressed_Index( (int)EPress::Button_Context, AsConfig::Context_Button_Count, i);  // which array

	if (i == (int)EUser_Arrays::EUA_Arrays_Count)
	{
		delete It_User_Map_Active->second->Title_Name_Key;
		delete It_User_Map_Active->second->Title_Name_Num;
		delete It_User_Map_Active->second;
		User_Maps[(int)Active_Map]->erase(It_User_Map_Active);
	}
	else
	{
		map = User_Maps[(int)(EUser_Arrays)i];  // or just i, know
		map->insert(std::make_pair(It_User_Map_Active->first, std::move(It_User_Map_Active->second) ) );
		User_Maps[(int)Active_Map]->erase(It_User_Map_Active->first);  // Errase from map
	}
	Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev] = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Non_Bordered()
{
	int i = 0;
	const int how_many_buttons = 2;

	Handle_Border_Pressed_Index( (int)EPress::Button_Reguest, how_many_buttons, i);
	if (i == how_many_buttons)  // if cant find any return
		return;

	Buttons[(int)EActive_Button::EAB_Menu_Sub_Curr] = Buttons[(int)EActive_Button::EAB_Menu_Sub_Prev];
	Handle_Button_Request(i);
	Redraw_Buttons_Menu_Sub();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_User_Input()
{
	int length = 0;
	wchar_t *url_content = 0;
	wchar_t *ptr = 0;

	// 1.0. If it`s url use ACurl_Client to Get ID_Content, Get Title + Num + Season
	if (wcsstr(User_Input, AsConfig::Protocols[0]) != 0 || wcsstr(User_Input, AsConfig::Protocols[1]) != 0)
	{
		// 1.1. Try to get url content from ACurl
		length = 128;  // !!! TEMP need to right
		url_content = new wchar_t[length]{};  // need send a lot of memorry to get long title names
		wcsncpy_s(url_content, (size_t)(wcslen(User_Input) + 1), User_Input, (int)wcslen(User_Input) );

		// !!! Refactoring some day
		ACurl_Client client_url(EProgram::ASaver, url_content);  // !!! || Get content from url
		if (wcsstr(url_content, L"Pattern") )  // !!! Rework with ACurl
		{
			wcsncpy_s(User_Input, wcslen(url_content) + 1, url_content, wcslen(url_content) );
			Draw_Button_User_Input();
			User_Input[0] = L'\0';
			Border_Pressed = EPress::Non_Bordered;
			delete[] url_content;
			return;
		}
		// !!! Refactoring END

		if (*url_content != L'\0')  // !!! if pattern added don`t need go here
		{// Covert data and title and Save image

			Handle_Title_Info(url_content);

			length = (int)wcslen(It_User_Map_Active->second->Title_Name_Key);
			wcsncpy_s(url_content, length + 1, It_User_Map_Active->second->Title_Name_Key, length);
			wcsncpy_s(url_content + wcslen(url_content), wcslen(AsConfig::Image_Format) + 1, AsConfig::Image_Format, wcslen(AsConfig::Image_Format) );
			std::filesystem::rename(AsConfig::Image_Name_File, std::filesystem::path(AsConfig::Image_Folder) / url_content);  // that why we have problem here cant name with :
		}
	}

	Border_Pressed = EPress::Button_User_Input_Second;
	delete[] url_content;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Title_Info(wchar_t *ptr)
{
	wchar_t *title_name_num = ptr;
	wchar_t *title_num_space = 0;
	wchar_t *title_season_space = 0;
	wchar_t *title_buffer = 0;
	int title_num = 0;
	int title_season = 0;
	STitle_Info *data = 0;

	Handle_Title_Format(title_name_num);  // Formatting title

	// 1.0. Get Num
	title_num_space = wcsrchr(ptr, L' ');
	title_num = std::wcstol(title_num_space + 1, 0, 10);
	*title_num_space = L'\0';

	// 1.1. Get Season
	title_season_space = wcsrchr(ptr, L' ');
	if (title_season_space)
	{
		title_season = std::wcstol(title_season_space + 1, 0, 10);
		if (title_season != 0)
			*title_season_space = L'\0';
	}

	// 1.2. Get Name Num And Name Key
	It_User_Map_Active = User_Maps[(int)Active_Map]->find(title_name_num);
	
	if (!(It_User_Map_Active != User_Maps[(int)Active_Map]->end() ) )
	{// If doesn`t exist create and add to active map
		data = new STitle_Info();

		// 1.2.a. Title_Name_Key
		data->Title_Name_Key = new wchar_t[wcslen(title_name_num) + 1] {};
		wcsncpy_s(data->Title_Name_Key, wcslen(title_name_num) + 1, title_name_num, wcslen(title_name_num) );
		*title_num_space = L' ';

		if (title_season != 0)
		{
			const wchar_t *season_str = AsConfig::Season_Case_Up[title_season - 1];
			Handle_Title_Name_Num(title_name_num, title_num_space, season_str, data->Title_Name_Num);
		}
		else
		{
			// 1.2.b. Title_Name_Num
			if (title_season != 0 && title_season_space != 0)
				*title_season_space = L' ';
			data->Title_Name_Num = new wchar_t[wcslen(title_name_num) + 1] {};
			wcsncpy_s(data->Title_Name_Num, wcslen(title_name_num) + 1, title_name_num, wcslen(title_name_num) );
		}

		// 1.2.c. Title_Num && Title_Season
		data->Title_Num = title_num;
		data->Title_Season = title_season;  // If season need something to do
		It_User_Map_Active = User_Maps[(int)Active_Map]->emplace(data->Title_Name_Key, data).first;
	}
	else
	{
		It_User_Map_Active->second->Title_Num = title_num;
		const wchar_t *season_str = AsConfig::Season_Case_Up[std::stoi(ptr + 1 + wcslen(title_name_num), 0, 10) - 1];
		if (title_season == 0)
		{// Not work from 9 to 10 or 99 to 100 for now

			title_buffer = It_User_Map_Active->second->Title_Name_Num;
			title_buffer = title_buffer + (title_num_space - title_name_num);
			while (*title_buffer != '\0')
				*(++title_buffer) = *(++title_num_space);
		}
		else
			Handle_Title_Name_Num(title_name_num, title_num_space, season_str, It_User_Map_Active->second->Title_Name_Num);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Title_Info_Beta(wchar_t *user_input, STitle_Info *&data)
{
	unsigned short current_ch;
	int user_input_length;
	int temp = 0;

	user_input_length = (int)wcslen(user_input) - 1;  // length without 0
	current_ch = (unsigned short)user_input[user_input_length];  // get last index char

	if (wcscmp(user_input, L"Я стал самым сильным с провальным навыком ") == 0)
		return;


	while (current_ch == L' ' || current_ch >= 48 && current_ch <= 57)
	{// Get NUM and Seasons Data

		if (current_ch == L' ')
			if (data->Title_Num == 0)
				data->Title_Num = std::stoi(user_input + user_input_length + 1);  // Get NUM Title
			else
				data->Title_Season = std::stoi(user_input + user_input_length + 1);  // Get Season Title

		current_ch = (unsigned short)user_input[--user_input_length];  // go to prev index and get ch
		if (data->Title_Num != 0)
			Handle_Title_Season(user_input + user_input_length, data->Title_Season, temp);
	}
	
	user_input_length -= temp;  // 1 for space
	user_input_length += 1;  // Set to next index |
	data->Title_Name_Key = new wchar_t[user_input_length + 1] {};  // + 1 '\0'
	wcsncpy_s(data->Title_Name_Key, static_cast<rsize_t>(user_input_length + 1), user_input, user_input_length);  // Get Title Name Key
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Title_Season(wchar_t *ptr, int &result, int &season_length)
{
	int i = 0;

	season_length = 0;

	if (*ptr != L'i' && *ptr != L'x' && *ptr != L'v')
		return;

	while (*ptr != ' ')
	{
		*(ptr--) = std::toupper(*ptr);
		season_length++;
	}
	*ptr = L'\0';
	ptr += 1;
	*(ptr + season_length) = L'\0';

	for (i = 0; i < 10; i++)
		if (wcscmp(ptr, AsConfig::Season_Case_Up[i]) == 0)
		{
			result = i + 1;
			break;
		}

	*(ptr + season_length) = L' ';
	*(ptr - 1) = L' ';
	season_length++;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Title_Name_Num(const wchar_t *key, wchar_t *num, const wchar_t *season, wchar_t *&result)
{
	*num = L' ';
	int title_length = (int)(wcslen(num) + wcslen(key) + 1 + wcslen(season) + 1);
	result = new wchar_t[title_length + 1] {};

	wcsncpy_s(result, wcslen(key) + 1, key, wcslen(key) );
	*(result + wcslen(result) ) = L' ';
	wcsncpy_s(result + wcslen(result), wcslen(season) + 1, season, wcslen(season) );
	*(result + wcslen(result) ) = L' ';
	wcsncpy_s(result + wcslen(result), wcslen(num) + 1, num, wcslen(num) );
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Title_Format(wchar_t *&title)
{
	int length = 0;
	wchar_t *found = 0;
	wchar_t *ptr = title;

	while ( (found = wcspbrk(title, AsConfig::Incompatible_Char_Type) ) != 0)
		while ( *found != '\0')
			( *found++ = found[1]);  // set next wchar to empty space after found++(it`s last low prior)

	while (*(ptr++) != L'\0')
		*ptr = std::towlower(*ptr);  // To lower
}
//------------------------------------------------------------------------------------------------------------  1850 - 1555 CURL
void AsUI_Builder::User_Map_Load(const char *file_path, std::map<wchar_t *, STitle_Info *, SCmp_Char> &map)
{
	bool is_add_to_user_array = false;
	wchar_t *to_map = 0;
	wchar_t *user_input = new wchar_t[100]{};
	int how_much_g = 0;
	int block_sum_ull = 0;
	int block_sum_index = 0, str = 0;
	unsigned long long *ull_array_blocks = 0;
	unsigned long long ull_char = 0;
	unsigned long long ull_index = 0;
	unsigned long long ull_number = 0;
	STitle_Info *buffer = {};

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

				buffer = new STitle_Info{};
				buffer->Title_Name_Num = new wchar_t[wcslen(user_input) + 1] {};
				to_map = buffer->Title_Name_Num;
				wcsncpy_s(to_map, wcslen(user_input) + 1, user_input, wcslen(user_input) );
				
				Handle_Title_Info_Beta(buffer->Title_Name_Num, buffer);  // Convert Data
				map.emplace(buffer->Title_Name_Key, buffer);  // Add to map

				is_add_to_user_array = false;  // look next numbers
				str = 0;
			}
			user_input[str++] = (wchar_t)User_Map_Convert_Out(ull_char);  // Convert to norm wchar_t and add to user_input
		}
		
		block_sum_index++;  // go to next index
		if (block_sum_index == block_sum_ull && user_input[0] != L'\0')
		{// If block last and user_input not empty save unsaved

			user_input[str] = L'\0';  // say it`s end
			user_input[0] = user_input[0] - 32;  // Set Upper Case first symbol

			buffer = new STitle_Info{};
			buffer->Title_Name_Num = new wchar_t[wcslen(user_input) + 1] {};
			to_map = buffer->Title_Name_Num;
			wcsncpy_s(to_map, wcslen(user_input) + 1, user_input, wcslen(user_input) );
				
			Handle_Title_Info_Beta(buffer->Title_Name_Num, buffer);// Convert Data
			map.emplace(buffer->Title_Name_Key, buffer);  // Add to map
		}
	}
	infile.close();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Save(const char *file_path, std::map<wchar_t *, STitle_Info *, SCmp_Char> &map)
{
	int title_index = 0, title_index_length = 0;
	int number_index = 0;
	unsigned short ch_i = 0;
	unsigned long long numbers = 0;

	std::ofstream outfile(file_path, std::ios::out | std::ios::binary);
	if (!outfile)
		return;

	for (std::pair<wchar_t *, STitle_Info *> it : map)
	{
		while (it.second->Title_Name_Num[title_index_length] != L'\0')
		{// Title length

			ch_i = User_Map_Convert_In( (unsigned short)it.second->Title_Name_Num[title_index_length++]);
			if (++number_index % 9 == 0)
			{
				numbers += ch_i;
				outfile.write(reinterpret_cast<const char *>( &numbers), sizeof(numbers) );
				numbers = 0;
			}
			else
				numbers = (numbers + ch_i) * 100;
		}
		title_index_length = 0;
	}

	if (number_index % 9 == 0)
		outfile.close();
	else
		outfile.write(reinterpret_cast<const char *>( &numbers), sizeof(numbers) );
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Free(std::map<wchar_t *, STitle_Info *, SCmp_Char> &map)
{
	for (std::map<wchar_t *, STitle_Info *>::iterator it = map.begin(); it != map.end(); )
	{// Free Memorry from all pointers in map

		delete it->second->Title_Name_Key;
		delete it->second->Title_Name_Num;
		delete it->second;

		it = map.erase(it);
	}
	map.clear();
}
//------------------------------------------------------------------------------------------------------------
unsigned short AsUI_Builder::User_Map_Convert_In(unsigned short ch)
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
unsigned long long  AsUI_Builder::User_Map_Convert_Out(unsigned long long &ch)
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
 : UI_Builder(0), UI_Book_Reader(0), W_Param(0), L_Param(0),
	EBuilder_Handler(EUI_Builder_Handler::EBH_UI_Menu_Main), Ptr_Hwnd(0), Ptr_Hdc(0), Paint_Struct{}
{
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame_ASaver(HWND hwnd)
{
	Ptr_Hwnd = hwnd;
	Ptr_Hdc = BeginPaint(Ptr_Hwnd, &Paint_Struct);

	if (UI_Builder != 0)  // Can be better
		UI_Builder->Builder_Handler(Ptr_Hdc, EBuilder_Handler, W_Param, L_Param);
	else
		UI_Builder = new AsUI_Builder(Ptr_Hdc);  // <= 9ms || what if without Init()?

	EBuilder_Handler = EUI_Builder_Handler::EBH_UI_Maximazed;
	if (UI_Builder->Builder_State == EBuilder_State::EBS_Exit)  // !!! TEMP
		AsEngine::~AsEngine();
	EndPaint(Ptr_Hwnd, &Paint_Struct);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Draw_Frame_AEmpty(HWND hwnd)
{
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	Ptr_Hwnd = hwnd;
	Ptr_Hdc = BeginPaint(Ptr_Hwnd, &Paint_Struct);

	SelectObject(Ptr_Hdc, AsConfig::Brush_Green_Dark);  // Background
	Test(1440, 900);
	SelectObject(Ptr_Hdc, AsConfig::Brush_Green);  // Background
	Test_0(1440, 900);

	//POINT triangle[3] {};
	//triangle[0] = { 1440 / 2, 900 / 2 };  // Левая нижняя вершина
	//triangle[1] = { 1440, 900 };          // Правая нижняя вершина
	//triangle[2] = { 1440 / 2, 900 };      // Верхняя вершина

	//Polygon(Ptr_Hdc, triangle, 3);

	EndPaint(Ptr_Hwnd, &Paint_Struct);

		//wchar_t text[] = L"Monday";
	//RECT textRect { 10, 10, 1000, 50 };



	// Select FONT
	//AsConfig::Title_Font.Select(Ptr_Hdc);

	// Where to draw RECT
	//textRect.left = 100;  // Координата X
	//textRect.top = 100;   // Координата Y
	//textRect.right = 300; // Дальняя правая граница (зависит от ширины текста)
	//textRect.bottom = 200; // Дальняя нижняя граница (зависит от высоты текста)
	
	//SetTextColor(Ptr_Hdc, RGB(255, 0, 0) );  // Устанавливаем цвет текста
	//SetBkMode(Ptr_Hdc, AsConfig::Color_Backgrount_Text);  // Устанавливаем фон для текста (прозрачный)
	//DrawTextExW(Ptr_Hdc, text, -1, &textRect, DT_LEFT, 0);  // Выводим текст в указанные координаты

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
void AsEngine::Test(double x, double y)
{
	RECT rect {};  // Don`t need here out of from those func

	rect.left = static_cast<int>(x /= 2.0);
	rect.top = static_cast<int>(y /= 2.0);
	rect.right = static_cast<int>(std::floor(x * 2.0) );
	rect.bottom = static_cast<int>(std::floor(y * 2.0) );
	if (rect.left < 90.0)
		return;

	Rectangle(Ptr_Hdc, 0, 0, rect.left, rect.top);  // UP
	Rectangle(Ptr_Hdc, 0, rect.top, rect.left, rect.bottom);  // DOWN

	Rectangle(Ptr_Hdc, rect.left, rect.top, rect.right, rect.bottom);  // UP
	Rectangle(Ptr_Hdc, rect.left, 0, rect.right, rect.top);  // DOWN
	Test(x, y);
}
//------------------------------------------------------------------------------------------------------------
void AsEngine::Test_0(double x, double y)
{
	RECT rect {};

	rect.left = static_cast<int>(x /= 2.0);
	rect.top = static_cast<int>(y /= 2.0);
	rect.right = static_cast<int>(std::floor(x * 2.0) );
	rect.bottom = static_cast<int>(std::floor(y * 2.0) );
	if (rect.left < 90.0)
		return;

	Rectangle(Ptr_Hdc, 0, rect.top, rect.left, rect.bottom);  // 1 | 0
	Rectangle(Ptr_Hdc, rect.left, 0, rect.right, rect.top);  // 0 | 1
	Test_0(x, y);
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
	memcpy(buffer_to_server, &client_buffer, sizeof(unsigned long long) );

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
	if (connect(socket_to_server, (struct sockaddr*)&address_server, sizeof(address_server) ) == SOCKET_ERROR)  // wait server
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
