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

	// Get ID Title, Season, Num and download image to file
	Get_ID();
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
	if (!std::filesystem::create_directories(*Content_W) )
		return;

	*Content_W += AsConfig::Pattern_Default_TXT;
	std::ofstream write_file(*Content_W, std::ios::binary);
	if (!write_file)
		return;

	write_file.write(AsConfig::Pattern_Default, strlen(AsConfig::Pattern_Default) );
	write_file.close();

	Title_Result[0] = '\0';  // !!! Need to say user what need format .txt to set patterns
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
	const char *pattern = "<h1>";
	int line_to_save;
	int line_current;
	std::string line_from_file;
	std::string content_from_file;

	line_to_save = 25;  // Content line from start needed
	line_current = 0;
	delete Content_W;

	// 1.0. Find pattern where from start add to string
	std::ifstream file(AsConfig::Temporary_File_Name[0], std::ios::binary);
	if (!file)
		return;
	
	while (std::getline(file, line_from_file) )
		if ( (line_current = (int)line_from_file.find(pattern) ) != std::string::npos)
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
	MultiByteToWideChar(CP_UTF8, 0, &content_from_file[0], (int)content_from_file.size(), &(*Content_W)[0], line_to_save);  // !!! &(*Content_W)[0] Inresting moment
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
	const wchar_t *bgn = Patterns_Array[(int)EPatterns_Site::Image_Bgn];
	const wchar_t *end = Patterns_Array[(int)EPatterns_Site::Image_End];
	FILE *file;
	CURL *url_easy;
	CURLcode response;
	std::wstring w_str_url;
	
	// 1.2. Create image url full with domein and convert to char
	AsTools::Format_Sub_WString(Content_W->c_str(), bgn, end, w_url);
	w_str_url = std::wstring(AsConfig::Protocols[0]) + Title_Site + w_url;
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
int AsUI_Builder::User_Input_Len = 0;
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::~AsUI_Builder()
{
	int i = 0;
	// 1.2 Save covered image sys
	if (Hdc_Memory != 0)
		DeleteDC(Hdc_Memory);
	if (H_Bitmap != 0)
		DeleteObject(H_Bitmap);

	User_Map_Save("99.bin", *User_Map_Ptr);
	User_Map_Save("98.bin", *User_Map_Library);
	Erase_Data(*User_Map_Ptr);
	Erase_Data(*User_Map_Library);

	// 1.4. 
	for (i = 0; i < (int)EPress::Exit; i++)
		delete Borders_Rect[i];

	// 1.5 Free memory
	delete[] Borders_Rect;

	delete Mouse_Cord_Destination;
	delete Mouse_Cord;
}
//------------------------------------------------------------------------------------------------------------
AsUI_Builder::AsUI_Builder(HDC hdc)
: Active_Menu(EAM_Main), Ptr_Hdc(hdc), Borders_Rect(0), Mouse_Cord_Destination(0), Mouse_Cord(0), User_Input{}, Data_From_File {}, Prev_Main_Menu_Button(0),
  Prev_Button(99), Sub_Menu_Curr_Page(0), Sub_Menu_Max_Line(31), Active_Button(EActive_Button::EAB_Main_Menu),
  Active_Page(EPage::None), Border_Pressed(EPress::None), Hdc_Memory(0), H_Bitmap(0), Saved_Object(0),
  User_Map_Ptr(0), User_Map_Library(0), User_Map_Paused(0), User_Map_Wishlist(0)
{
	auto user_map_loaders = [&]()
		{
			User_Map_Ptr = new std::map<wchar_t *, S_Extend *, cmp_wchar>;
			User_Map_Load("Data/Watching.bin", *User_Map_Ptr);  // Load from file and add to User_Map_Ptr
			for (auto &it : *User_Map_Ptr)  // Convert to User_Map_Ptr
				Convert_Data(it.first, it.second);
		};

	auto user_map_library = [&]()
		{
			User_Map_Library = new std::map<wchar_t *, S_Extend *, cmp_wchar>;
			User_Map_Load("Data/Library.bin", *User_Map_Library);  // Load from file and add to User_Map_Library
			for (auto &it : *User_Map_Library)  // Convert to User_Map_Library
				Convert_Data(it.first, it.second);
		};

	// THREAD FIRST
	std::thread thread_wch(user_map_loaders);
	std::thread thread_lib(user_map_library);

	Init();
	Draw_Menu_Main();

	thread_wch.detach();
	thread_lib.detach();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Builder_Handler(HDC ptr_hdc, const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam)
{
	Ptr_Hdc = ptr_hdc;

	switch (builder_handler)
	{
	case EUI_Builder_Handler::Draw_Full_Window:  // draw after maximazed window
		Draw_Menu_Main();
		Draw_Menu_Sub();
		break;
	case EUI_Builder_Handler::Draw_Menu_Main:
		Draw_Menu_Main();
		break;
	case EUI_Builder_Handler::Draw_Menu_Sub:
		Handler_User_Input();
		break;
	case EUI_Builder_Handler::Draw_User_Input_Button:
		User_Input_Update(static_cast<wchar_t>(wParam) );
		break;
	case EUI_Builder_Handler::Handle_Mouse_LButton:
	case EUI_Builder_Handler::Handle_Mouse_RButton:
		Handle_Button_Bordered(builder_handler, lParam);  // lparam to glob param
		break;
	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Init()
{
	if (!std::filesystem::exists(AsConfig::Image_Folder) )  // !!! Need Remove from here
		std::filesystem::create_directories(AsConfig::Image_Folder);

	Borders_Rect = new RECT *[(int)EPress::Exit] {};  // Exit 7 last border

	Borders_Rect[(int)EPress::Menu_Main] = new RECT {};
	Borders_Rect[(int)EPress::Menu_Sub] = new RECT {};
	Borders_Rect[(int)EPress::User_Input_Handler] = new RECT {};
	Borders_Rect[(int)EPress::Button_Reguest] = new RECT[2] {};  // Increase or Decrease Buttons
	Borders_Rect[(int)EPress::Button_Pages] = new RECT[3] {};
	Borders_Rect[(int)EPress::Buttons_User_Input] = new RECT[Sub_Menu_Max_Line] {};
	Borders_Rect[(int)EPress::Button_Menu_Main] = new RECT[AsConfig::Menu_Main_Button_Count] {};
	Borders_Rect[(int)EPress::Button_Context] = new RECT[AsConfig::Context_Button_Count] {};
	Borders_Rect[(int)EPress::Menu_Context] = new RECT {};

	Borders_Rect[(int)EPress::Button_Pages][EPage::Update] = { 1140, 12, 1228, 30 };
	Borders_Rect[(int)EPress::Button_Pages][EPage::Prev] = { 1231, 12, 1303, 30 };
	Borders_Rect[(int)EPress::Button_Pages][EPage::Next] = { 1305, 12, 1375, 30 };

	Mouse_Cord = new RECT {};
	Mouse_Cord_Destination = new RECT {};
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handler_User_Input()
{
	wchar_t *url_content;
	int length;
	//std::map<wchar_t *, S_Extend *, cmp_wchar> *map = 0;

	// 1.0. If it`s url use ACurl_Client to Get ID_Content, Get Title + Num + Season
	if (wcsstr(User_Input, AsConfig::Protocols[0]) != 0 || wcsstr(User_Input, AsConfig::Protocols[1]) != 0)
	{
		// If not threaded uselles sections except curl
		length = (int)wcslen(User_Input) + 1;
		url_content = new wchar_t[length]{};
		wcsncpy_s(url_content, length, User_Input, static_cast<rsize_t>(length) - 1);

		ACurl_Client client_url(EProgram::ASaver, url_content);  // Get content from url

		length = (int)wcslen(url_content) + 1;
		wcsncpy_s(User_Input, length, url_content, static_cast<rsize_t>(length) - 1);
		delete[] url_content;
	}

	return;

	// 1.5. If from ACurl, saved picture rename, can`t save if invalid file path
	//if (std::filesystem::exists(AsConfig::Image_Name_File) )  // !!! Need Remove from here
	//{
	//	try
	//	{
	//		std::filesystem::rename(AsConfig::Image_Name_File, (std::wstring(AsConfig::Image_Folder) + converted_data.Title_Name_Key + std::wstring(L".png") ) );
	//	}
	//	catch (const std::exception &)
	//	{
	//		std::filesystem::remove(AsConfig::Image_Name_File);  // !!! Save ID Content like name to picture, or rename text || Or find invalid, if has check
	//	}
	//}

	// 1.6. Show and select new added title to window
	//if (Active_Menu != EActive_Menu::EAM_Main)
	//{
	//	It_User_Map_Active = User_Map_Ptr->find(User_Input);  // Find title and set it to iterator
	//	Active_Button = (EActive_Button)std::distance(User_Map_Ptr->begin(), It_Current_User);  // Set num to active button
	//	Sub_Menu_Curr_Page = ( (int)Active_Button - 1) / Sub_Menu_Max_Line;  // Find Page to draw useing active button
	//}

	//// 3.0. Final redraw All and Save
	//User_Input[0] = L'\0';  // Reset User_Input
	//Draw_Menu_Sub();  // Redraw All Sub Menu
	//User_Input_Draw();  // Redrow User Input
	//Draw_Active_Button_Advenced();  // Show Active Button
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Emplace(std::map<std::wstring, SUser_Input_Data> &user_arr, wchar_t *user_input)
{
	int title_len;
	SUser_Input_Data converted_data;

	title_len = (int)wcslen(user_input) - 1;
	converted_data = {};
	if (user_input[0] == L'\0')  // If pressed Enter while User_Input empty
		return;

	// 1.2  Check user_input orphography
	while (user_input[wcslen(user_input) - 1] == L' ')  // If last ch = space delete
		user_input[--title_len] = L'\0';

	// 1.3 Init_Data before set to map
	User_Input_Convert_Data(converted_data, user_input);

	// 1.4. Check if containts the same key if not save to map
	if (user_arr.contains(converted_data.Title_Name_Key) )
		user_arr[converted_data.Title_Name_Key] = converted_data;
	else
		user_arr.emplace(converted_data.Title_Name_Key, converted_data);  // if not add new title

	// 1.5. If from ACurl, saved picture rename, can`t save if invalid file path
	if (std::filesystem::exists(AsConfig::Image_Name_File) )
	{
		try
		{
			std::filesystem::rename(AsConfig::Image_Name_File, (std::wstring(AsConfig::Image_Folder) + converted_data.Title_Name_Key + std::wstring(L".png") ) );
		}
		catch (const std::exception &)
		{
			std::filesystem::remove(AsConfig::Image_Name_File);  // !!! Save ID Content like name to picture, or rename text || Or find invalid, if has check
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
	user_input[0] = L'\0';
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
void AsUI_Builder::User_Input_Set_From_Clipboard()
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
	User_Input_Draw();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Border(RECT &border_rect) const
{
	bool is_sub_menu;
	int scale;
	int x_cord;
	int border_width;
	int border_height;

	x_cord = 0;
	is_sub_menu = false;
	scale = AsConfig::Global_Scale;

	if (!IsRectEmpty(&Borders_Rect[(int)EPress::Menu_Main][0]) )  // if not main menu we must reset setting
	{// Draw Sub Menu

		x_cord = Borders_Rect[(int)EPress::Menu_Main][0].right + AsConfig::Global_Scale;
		border_width = (AsConfig::Window_Width - x_cord) - 23 * 2 - 9;  // window_width - main menu width
		border_height = AsConfig::Window_Height + 30 - scale;
		is_sub_menu = true;
		SelectObject(Ptr_Hdc, AsConfig::Brush_Green);
	}
	else
	{// Draw Main Menu

		border_width = (AsConfig::Menu_Main_Title_Length + scale) * AsConfig::Ch_W;  // 424
		border_height = (AsConfig::Menu_Main_Button_Count + 2) * (AsConfig::Ch_H + 6) + scale + 1;
	}

	// 1.2. Draw First Border
	border_rect.left = x_cord + scale;
	border_rect.top = scale;
	border_rect.right = x_cord + border_width;
	border_rect.bottom = border_height;
	Rectangle(Ptr_Hdc, border_rect.left, border_rect.top, border_rect.right, border_rect.bottom);

	// 1.2. Draw Second Border
	border_rect.left = border_rect.left + scale;
	border_rect.top = border_rect.top + scale;
	border_rect.right = border_rect.right - scale;
	border_rect.bottom = border_rect.bottom - scale;
	Rectangle(Ptr_Hdc, border_rect.left, border_rect.top, border_rect.right, border_rect.bottom);
	if (is_sub_menu != true)  // if not submenu return
		return;

	// 2.1. Fill Rect
	border_rect.left = border_rect.left + 1;
	border_rect.top = border_rect.top + 1;
	border_rect.right = border_rect.right - 1;
	border_rect.bottom = border_rect.bottom - 1;
	FillRect(Ptr_Hdc, &border_rect, AsConfig::Brush_Background_Dark);  // Fill sub_menu to grey menu
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
void AsUI_Builder::Draw_Button_Text(const HBRUSH &background, const COLORREF &color_bk, const COLORREF &color_tx, const RECT &rect, const wchar_t *str) const
{
	SelectObject(Ptr_Hdc, background);
	SetBkColor(Ptr_Hdc, color_bk);
	SetTextColor(Ptr_Hdc, color_tx);

	Rectangle(Ptr_Hdc, rect.left, rect.top, rect.right, rect.bottom);  // draw rect
	TextOutW(Ptr_Hdc, rect.left + AsConfig::Global_Scale, rect.top + AsConfig::Global_Scale, str, (int)wcslen(str) );  // button_prev text ( x its text out in middle
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button_Pages()
{
	int i = 0;
	RECT *button = 0;

	for (int i = 0; i < EPage::Last; i++)
	{
		button = &Borders_Rect[(int)EPress::Button_Pages][i];
		Rectangle(Ptr_Hdc, button->left, button->top, button->right, button->bottom);
		TextOutW(Ptr_Hdc, button->left + 1, button->top + 1, AsConfig::Battons_Page_Name[i], i == 0 ? 11 : 9);
	}
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Menu_Main()
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
	Draw_Border(Borders_Rect[(int)EPress::Menu_Main][0]);
	border_rect = Borders_Rect[(int)EPress::Menu_Main][0];
	x = (border_rect.right - str_length) / 2;  // Get the center of first middle border and half of title
	y = border_rect.top + AsConfig::Global_Scale;  // Get offset from border top
	TextOutW(Ptr_Hdc, x, y, AsConfig::Main_Menu_Title_Name, titles_length);
	border_rect.top = y + AsConfig::Ch_H + AsConfig::Global_Scale;  // go to next line

	// 1.2. Set Buttons in border and save they`re cords in Borders_Rect[(int)EPress::Button_Menu_Main]
	for (int i = 0; i < AsConfig::Menu_Main_Button_Count; i++)
		Draw_Button(border_rect, Borders_Rect[(int)EPress::Button_Menu_Main][i], AsConfig::Menu_Main_Buttons_Text_Eng[i]);

	// 1.3. Draw Image
	Draw_User_Title_Image();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Draw() const
{
	RECT *user_input = &Borders_Rect[(int)EPress::User_Input_Handler][0];
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
void AsUI_Builder::User_Input_Update(const wchar_t &user_text)
{
	if (Active_Menu == EActive_Menu::EAM_Main)  // main menu
		return;

	if (user_text != '\b' && User_Input_Len < AsConfig::User_Input_Buffer)
		User_Input[User_Input_Len++] = user_text;
	else if (User_Input_Len > 0)
		User_Input[--User_Input_Len] = 0;  // delete prev user_text

	User_Input_Draw();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Draw_Button_Request()
{
	int button_offset;
	int box_size;
	int scale;
	int half_box;
	RECT *ui_rect_offset = 0;
	
	button_offset = 9;
	box_size = 21;
	half_box = (int)( (float)box_size / 2.0f);
	scale = AsConfig::Global_Scale;

	FillRect(Ptr_Hdc, &Borders_Rect[(int)EPress::Button_Reguest][0], AsConfig::Brush_Background_Dark);  // hide buttons
	FillRect(Ptr_Hdc, &Borders_Rect[(int)EPress::Button_Reguest][1], AsConfig::Brush_Background_Dark);
	if (Active_Menu == EActive_Menu::EAM_Main || Active_Button == EActive_Button::EAB_Main_Menu)  // If clicked at menu main border return from func after clear buttons
		return;

	// 1.1 Draw first Rectangle decrease
	SelectObject(Ptr_Hdc, AsConfig::Brush_Green_Dark);  // Select color
	ui_rect_offset = &Borders_Rect[(int)EPress::Button_Reguest][0];
	ui_rect_offset->left = Borders_Rect[(int)EPress::Buttons_User_Input][Prev_Button].right + button_offset;
	ui_rect_offset->top = Borders_Rect[(int)EPress::Buttons_User_Input][Prev_Button].top;
	ui_rect_offset->right = Borders_Rect[(int)EPress::Buttons_User_Input][Prev_Button].right + button_offset + box_size;
	ui_rect_offset->bottom = Borders_Rect[(int)EPress::Buttons_User_Input][Prev_Button].top + box_size;

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
void AsUI_Builder::Draw_User_Title_Image() const
{
	int width = 0, height = 0, bpp = 0;
	DXGI_FORMAT format {};
	RECT img_cords {};
	std::filesystem::path image_path {};
	BITMAPINFO bmi = {};
	DirectX::ScratchImage image_title {};

	if (Active_Menu != EActive_Menu::EAM_Main)
		image_path = AsConfig::Image_Folder + std::wstring(It_User_Map_Active->second->Title_Name_Key) + L".png";
	else
		if (std::filesystem::exists(AsConfig::Main_Image_Folder) )
			image_path = AsConfig::Main_Image_Folder;

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
void AsUI_Builder::Draw_Active_Button_Advenced()
{
	int title_name_length;
	std::map<wchar_t *, S_Extend *, cmp_wchar> *map;

	title_name_length = 0;
	map = 0;
	if ( !(Active_Button != EActive_Button::EAB_Main_Menu) )
		return;  // !!! Is this really need?

	if (Active_Menu == EActive_Menu::EAM_Main)
	{
		Draw_Button_Text(AsConfig::Brush_Background_Dark, AsConfig::Color_Dark, AsConfig::Color_Text_Green, Borders_Rect[(int)EPress::Button_Menu_Main][Prev_Main_Menu_Button], AsConfig::Menu_Main_Buttons_Text_Eng[Prev_Main_Menu_Button]);
		Draw_Button_Text(AsConfig::Brush_Green_Dark, AsConfig::Color_Text_Green, AsConfig::Color_Dark, Borders_Rect[(int)EPress::Button_Menu_Main][(int)Active_Button], AsConfig::Menu_Main_Buttons_Text_Eng[(int)Active_Button]);

		Prev_Main_Menu_Button = (int)Active_Button;
		return;
	}

	switch (Active_Menu)
	{
	case EAM_Watching:
		map = User_Map_Ptr;
		break;
	case EAM_Library_Menu:
		map = User_Map_Library;
		break;
	case EAM_Paused_Menu:
		map = User_Map_Paused;
		break;
	case EAM_Wishlist:
		map = User_Map_Wishlist;
		break;
	case EAM_Exit:
		return PostQuitMessage(0);
	}

	if (!map != 0)
		return;

	if (Prev_Button == 99)
		Prev_Button = (int)Active_Button;

	It_User_Map_Active = map->begin();  // Prev_Button
	std::advance(It_User_Map_Active, (int)Prev_Button);
	Draw_Button_Text(AsConfig::Brush_Background_Dark, AsConfig::Color_Dark, AsConfig::Color_Text_Green, Borders_Rect[(int)EPress::Buttons_User_Input][Prev_Button], It_User_Map_Active->second->Title_Name_Num);
		
	It_User_Map_Active = map->begin();  // Active Button
	std::advance(It_User_Map_Active, (int)Active_Button);

	if (Active_Page != EPage::Update)  // if from update button change color
		Draw_Button_Text(AsConfig::Brush_Green_Dark, AsConfig::Color_Text_Green, AsConfig::Color_Dark, Borders_Rect[(int)EPress::Buttons_User_Input][(int)Active_Button], It_User_Map_Active->second->Title_Name_Num);
	else
	{
		Active_Page = EPage::None;
		Draw_Button_Text(AsConfig::Brush_Background_Button_Update, AsConfig::Color_Backgrount_Text, AsConfig::Color_Dark, Borders_Rect[(int)EPress::Buttons_User_Input][(int)Active_Button], It_User_Map_Active->second->Title_Name_Num);
	}
	Prev_Button = (int)Active_Button;

	Draw_Button_Request();
	Draw_User_Title_Image();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Context_Menu_Draw(const int &x, const int &y)
{
	int button_text_len;
	int scale;
	int context_offset;
	int user_input_len;
	int button_heigh;
	RECT *context_rect;
	
	context_rect = &Borders_Rect[(int)EPress::Menu_Context][0];
	button_text_len = (int)wcslen(AsConfig::Menu_Main_Buttons_Text_Eng[3]);
	scale = AsConfig::Global_Scale;
	context_offset = 6;
	user_input_len = button_text_len * AsConfig::Ch_W + context_offset;
	button_heigh = AsConfig::Ch_H * AsConfig::Context_Button_Count + context_offset;

	// 1. Start point where RMB pressed, then we take longest word and add his len like width
	context_rect->left = x;
	context_rect->top = y;
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
void AsUI_Builder::User_Map_Load(const char *file_path, std::map<wchar_t *, S_Extend *, cmp_wchar> &map)
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

				to_map = new wchar_t[wcslen(user_input) + 1] {};
				wcsncpy_s(to_map, wcslen(user_input) + 1, user_input, wcslen(user_input) );
				map.emplace(to_map, new S_Extend{} );

				
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

			to_map = new wchar_t[wcslen(user_input) + 1] {};
			wcsncpy_s(to_map, wcslen(user_input) + 1, user_input, wcslen(user_input) );
			map.emplace(to_map, new S_Extend{});
		}
	}
	infile.close();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Convert_Data(wchar_t *user_input, S_Extend *&data)
{
	unsigned short current_ch;
	wchar_t *pattern_season;
	int user_input_length;
	int season_counter;

	pattern_season = 0;
	user_input_length = (int)wcslen(user_input) - 1;
	current_ch = (unsigned short)user_input[user_input_length];
	season_counter = 0;

	while (current_ch == L' ' || current_ch >= 48 && current_ch <= 57)
	{//Find spaces if current ch numeric

		if (current_ch == L' ')
		{// If space str to int

			if (data->Title_Num == 0)
				data->Title_Num = std::stoi(user_input + user_input_length + 1);  // if first ' ' get num
			else
				data->Title_Season = std::stoi(user_input + user_input_length + 1);  // if second ' ' get seasons

			user_input[user_input_length] = L'\0';  // Hide nums
		}
		current_ch = (unsigned short)user_input[--user_input_length];
		
		while (user_input[user_input_length] == L'X' || user_input[user_input_length] == L'I' || user_input[user_input_length] == L'V')
		{
			season_counter++;
			user_input[user_input_length--] += L' ';
		}
		user_input_length += season_counter;
		
		while (user_input[user_input_length] == L'x' || user_input[user_input_length] == L'i' || user_input[user_input_length] == L'v')  // Find Seasons =) 
		{// Find next season character

			user_input_length--;
			if (user_input[user_input_length] == L' ')
			{// Find next space

				pattern_season = user_input + user_input_length + 1;
				for (int i = 0; i < 10; i++)
				{// Find Pattern in Config and set to data

					if (wcscmp(pattern_season, AsConfig::Season_Case_Low[i]) == 0)
					{
						data->Title_Season = i + 1;
						break;
					}
				}
				user_input[user_input_length] = L'\0';  // erase season from user_input
			}
		}
	}
	
	// Title_Name_Key
	data->Title_Name_Key = new wchar_t[wcslen(user_input) + 1] {};
	wcsncpy_s(data->Title_Name_Key, wcslen(user_input) + 1, user_input, wcslen(user_input) );

	// TITLE_NAME_NUM
	if (data->Title_Num == 0)
		data->Title_Num = 1;

	if (data->Title_Season != 0)
	{
		const wchar_t *season_char = AsConfig::Season_Case_Up[data->Title_Season - 1];
		season_counter = (int)wcslen(season_char);

		user_input[user_input_length] = L' ';
		user_input[user_input_length + season_counter + 1] = ' ';

		do { user_input[user_input_length + season_counter] = season_char[season_counter - 1]; }
		while (--season_counter != 0);  // Changle Seasons to upper case
	}
	else
		user_input[user_input_length + 1] = L' ';

	data->Title_Name_Num = new wchar_t[wcslen(user_input) + 1] {};
	wcsncpy_s(data->Title_Name_Num, wcslen(user_input) + 1, user_input, wcslen(user_input) );
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Erase_Data(std::map<wchar_t *, S_Extend *, cmp_wchar> &map)
{
	for (std::map<wchar_t *, S_Extend *>::iterator it = map.begin(); it != map.end(); )
	{// Free Memorry from all pointers in map

		delete it->second->Title_Name_Key;
		delete it->second->Title_Name_Num;
		delete it->second;
		delete it->first;

		it = map.erase(it);
	}

	delete &map;
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Map_Save(const char *file_path, std::map<wchar_t *, S_Extend *, cmp_wchar> &map)
{
	int title_index = 0, title_index_length = 0;
	int number_index = 0;
	unsigned short ch_i = 0;
	unsigned long long numbers = 0;

	std::ofstream outfile(file_path, std::ios::out | std::ios::binary);  // Создаем новые данные
	if (!outfile)
		return;

	for (std::pair<wchar_t *, S_Extend *> it : map)
	{
		while (it.second->Title_Name_Num[title_index_length] != L'\0')
		{// Title length

			ch_i = User_Map_Save_Convert( (unsigned short)it.second->Title_Name_Num[title_index_length++]);
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
void AsUI_Builder::Draw_Menu_Sub()
{
	int curr_line;
	int curr_page_max_line;
	std::map<wchar_t *, S_Extend *, cmp_wchar> *map;
	std::map<wchar_t *, S_Extend *, cmp_wchar>::iterator it;
	RECT border_rect;

	curr_line = 0;
	curr_page_max_line = 0;
	map = 0;
	border_rect = {};
	Prev_Button = 99;  // Need to switch between arrays

	// 1.1.Draw Border, Set colors, Draw Titles and user input handler
	Draw_Border(Borders_Rect[(int)EPress::Menu_Sub][0]);  // draw border
	border_rect = Borders_Rect[(int)EPress::Menu_Sub][0];

	border_rect.top += AsConfig::Global_Scale;  // without title? i can fix but it`s look good enough
	SelectObject(Ptr_Hdc, AsConfig::Brush_Background_Dark);
	SetBkColor(Ptr_Hdc, AsConfig::Color_Dark);
	SetTextColor(Ptr_Hdc, AsConfig::Color_Text_Green);
	Draw_Button(border_rect, Borders_Rect[(int)EPress::User_Input_Handler][0], AsConfig::Sub_Menu_Title);  // Write Sub menu title
	Draw_Button(border_rect, Borders_Rect[(int)EPress::User_Input_Handler][0], AsConfig::Sub_Menu_User_Input_Title);  // Write User Input Handler

	switch (Active_Menu)
	{
	case EAM_Watching:
		map = User_Map_Ptr;
		break;
	case EAM_Library_Menu:
		map = User_Map_Library;
		break;
	case EAM_Paused_Menu:
		map = User_Map_Paused;
		break;
	case EAM_Wishlist:
		map = User_Map_Wishlist;
		break;
	default:
		return;

	}

	// 2.1. Set iterator to start and check sub menu
	it = map->begin();
	if (map->size() < Sub_Menu_Max_Line)
		Sub_Menu_Curr_Page = 0;
	
	// 2.2. If in next page move iterator to needed page
	curr_page_max_line = Sub_Menu_Max_Line * (Sub_Menu_Curr_Page + 1);
	curr_line = Sub_Menu_Curr_Page * Sub_Menu_Max_Line;
	std::advance(it, curr_line);
	Draw_Button_Pages();

	// 2.3. Draw needed buttons to submenu
	for (; it != map->end(); ++it)
	{
		if (curr_line < curr_page_max_line)
			Draw_Button(border_rect, Borders_Rect[(int)EPress::Buttons_User_Input][curr_line], it->first);
		else
			return;
		
		curr_line++;
	}
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
				outfile.write(reinterpret_cast<const char *>( &numbers), sizeof(numbers) );
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
		outfile.write(reinterpret_cast<const char *>( &numbers), sizeof(numbers) );
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::User_Input_Convert_Data(SUser_Input_Data &ui_data, wchar_t *user_input)
{
	unsigned short current_ch;
	wchar_t *pattern_season;
	int current_user_input_length;
	int season_counter_index;

	pattern_season = 0;
	current_user_input_length = (int)wcslen(user_input) - 1;
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
unsigned long long  AsUI_Builder::User_Map_Load_Convert(unsigned long long &ch)
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
void AsUI_Builder::Handle_Button_Bordered(const EUI_Builder_Handler &builder_handler, const LPARAM &lParam)
{
	const int x = lParam & 0xffff;
	const int y = (int)(lParam >> 16);
	int index = 0;

	Mouse_Cord->left = x - 1;
	Mouse_Cord->top = y;
	Mouse_Cord->right = x;
	Mouse_Cord->bottom = y + 1;

	// 1.0. Border Chooser : at which border pressed, get index
	for (index = 0; index < (int)EPress::Non_Bordered; index++)
		if (IntersectRect(Mouse_Cord_Destination, Mouse_Cord, &Borders_Rect[index][0] ) )
			break;

	// 1.1. Context Menu if rect is not empty clear
	if (Borders_Rect[(int)EPress::Menu_Context][0].left != 0)
		Context_Image_Restore(Borders_Rect[ (int)EPress::Menu_Context][0] );

	// 1.2. Borders Handler
	switch (Border_Pressed = (EPress)index)
	{
	case EPress::Non_Bordered:
		Handle_Non_Bordered();
		break;

	case EPress::Menu_Main:
		Border_Pressed = EPress::Button_Menu_Main;
		Handle_Menu_Main();
		break;

	case EPress::Menu_Context:
		Border_Pressed = EPress::Button_Context;
		Handle_Menu_Context();
		break;

	case EPress::Menu_Sub:
		Border_Pressed = EPress::User_Input_Handler;  // Find from user input
		Handle_Menu_Sub();
		break;
	}
	
	if (builder_handler == EUI_Builder_Handler::Handle_Mouse_RButton)  // If RM Button
		Context_Menu_Draw(Mouse_Cord->right, Mouse_Cord->top);
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Main()
{
	int i = 0;

	Cycle_Finder(*Mouse_Cord, (int)Border_Pressed, AsConfig::Menu_Main_Button_Count, i);
	if ( !(i != AsConfig::Menu_Main_Button_Count) )
		return;

	if ( (Active_Menu = (EActive_Menu)i) == EActive_Menu::EAM_Exit)
		return PostQuitMessage(0);

	Active_Menu = EAM_Main;
	Active_Button = (EActive_Button)i;

	Draw_Button_Request();  // Clean reguest
	Draw_Active_Button_Advenced();  // Redraw pressed button

	Active_Menu = (EActive_Menu)i;
	Draw_Menu_Sub();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Context()
{
	int i;
	std::map<wchar_t *, S_Extend *, cmp_wchar> *map;

	i = 0;
	map = 0;
	Cycle_Finder(*Mouse_Cord, (int)EPress::Button_Context, AsConfig::Context_Button_Count, i);

	// 1.0. Add to Array
	switch ( (EActive_Menu)i)  // If pressed first button context menu choose action
	{
	case EAM_Watching:
		map = User_Map_Ptr;
		break;
	case EAM_Library_Menu:
		map = User_Map_Library;
		break;
	case EAM_Paused_Menu:
		map = User_Map_Paused;
		break;
	case EAM_Wishlist:
		map = User_Map_Wishlist;
		break;
	}
	map->insert(std::make_pair(It_User_Map_Active->first, std::move(It_User_Map_Active->second) ) );

	// 1.1. Erase from array
	switch (Active_Menu)
	{
	case EAM_Watching:
		map = User_Map_Ptr;
		break;
	case EAM_Library_Menu:
		map = User_Map_Library;
		break;
	case EAM_Paused_Menu:
		map = User_Map_Paused;
		break;
	case EAM_Wishlist:
		map = User_Map_Wishlist;
		break;
	}
	map->erase(It_User_Map_Active->first);

	Active_Button = EActive_Button::EAB_Main_Menu;
	Prev_Button = 99;  // set to no prev_button
	Draw_Button_Request();
	Draw_Menu_Sub();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Non_Bordered()
{
	int i = 0;
	const int how_many_buttons = 2;

	Cycle_Finder(*Mouse_Cord, (int)EPress::Button_Reguest, how_many_buttons, i);
	if (i == how_many_buttons)  // if cant find any return
		return;

	Active_Button = (EActive_Button)Prev_Button;
	User_Input_Value_Is_Changed(i);
	Draw_Active_Button_Advenced();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Handle_Menu_Sub()
{// 1.0. Manu Sab Handler
	int i = 0;

	// 1.1. User Input Button Handler
	if (IntersectRect(Mouse_Cord_Destination, Mouse_Cord, &Borders_Rect[(int)EPress::User_Input_Handler][0] ) )
		if (User_Input[0] != 0)
			return Handler_User_Input();
		else
			return User_Input_Set_From_Clipboard();

	// 1.2. Next or Prev page Handler
	Cycle_Finder(*Mouse_Cord, (int)EPress::Button_Pages, (int)EPage::Last, i);
	if (i != (int)EPage::Last)
	{
		if (i == EPage::Prev)
			if (Sub_Menu_Curr_Page < 1)
				return;
			else
				Sub_Menu_Curr_Page--;

		if (i == EPage::Next)
			Sub_Menu_Curr_Page++;

		Active_Button = EActive_Button::EAB_Main_Menu;
		Draw_Menu_Sub();
		Draw_Button_Request();
		return;
	}

	// 1.3. Title Buttons
	for (i = Sub_Menu_Curr_Page * Sub_Menu_Max_Line; i < i + Sub_Menu_Max_Line; i++)
		if (IntersectRect(Mouse_Cord_Destination, Mouse_Cord, &Borders_Rect[(int)EPress::Buttons_User_Input][i] ) )
			break;

	Active_Button = (EActive_Button)i;
	Draw_Active_Button_Advenced();
}
//------------------------------------------------------------------------------------------------------------
void AsUI_Builder::Cycle_Finder(const RECT &mouse_cord, const int border_index, const int count, int &result)
{
	for (result = 0; result < count; result++)
		if (IntersectRect(Mouse_Cord_Destination, &mouse_cord, &Borders_Rect[border_index][result] ) )
			break;
}
//------------------------------------------------------------------------------------------------------------  1850 - 1583




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
		UI_Builder = new AsUI_Builder(Ptr_Hdc);

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
