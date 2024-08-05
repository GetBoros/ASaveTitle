#pragma once

#include "AsConfig.h"

//------------------------------------------------------------------------------------------------------------
const int Timer_ID = WM_USER + 1;
//------------------------------------------------------------------------------------------------------------
enum class EPatterns_Site : byte
{
	Title_Bgn = 0,
	Title_End,
	Title_Num_Bgn,
	Title_Num_End,
	Image_Bgn,
	Image_End,
	Last_Index
};
//------------------------------------------------------------------------------------------------------------
enum class EProgram
{
	Invalid = -1,
	ASaver = 0,
	ABook_Reader,
	End
};
//------------------------------------------------------------------------------------------------------------
enum class EUI_Builder_Handler : int
{
	Draw_Full_Window,
	Draw_Menu_Main,
	Draw_Menu_Sub,
	Draw_User_Input_Button,
	Handle_Mouse_LButton,
	Handle_Mouse_RButton
};
//------------------------------------------------------------------------------------------------------------
enum class EActive_Button : int
{
	EAB_Main_Menu = -1,
	EAB_Sub_Menu = 0
};
//------------------------------------------------------------------------------------------------------------
enum EActive_Menu
{
	EAM_Main = -1,
	EAM_Watching = 0,
	EAM_Library_Menu,
	EAM_Paused_Menu,
	EAM_Wishlist,
	EAM_Erase,
	EAM_Exit
};
//------------------------------------------------------------------------------------------------------------
enum EActive_Page
{
	EAP_None = -1,
	EAP_Update = 0,
	EAP_Prev,
	EAP_Next,
	EAP_Last
};
//------------------------------------------------------------------------------------------------------------
struct SUser_Input_Data
{// Some day refactor this poop

	int Title_Num = 0;
	int Title_Data = 0;
	int Title_Season = 0;
	std::wstring Title_Name_Key = L"";
	std::wstring Title_Name_Num = L"";
};
//------------------------------------------------------------------------------------------------------------



// ACurl_Client
class ACurl_Client
{
public:
	~ACurl_Client();
	ACurl_Client(const EProgram &program, wchar_t *&user_input);

	bool Erase_ID(const int &if_not_last_id_content);  // !!! Not Used
	bool Make_URL(wchar_t *result, const int &id_content_index);  // !!! Not used

private:
	void Init();
	void CURL_Handler();  // !!! Need Refactoring

	// Done | Parsing Site, Get Title Name(num, season) and Download Image
	void Add_Pattern_File();  // Create File for user`s can set prefered patterns to curl || 164
	void Get_ID();  // Handle ID_Content_Array | Save / Load Array | Resize Array | Tested only Anime-bit
	void Get_Patterns();  // Try to receive valid patterns from file, if bad string next func died
	void Get_URL_Data() const;  // Download content to file from user url, need to find needet data | CURL
	void Get_Contents();  // Use pattern to find title name, nums, season and image domain
	void Get_Title();  // Send(format) to Title_Result finale result if patterns was good
	void Get_Image();  // Download image to temporary folder and file, don`t care about to save  | CURL || 362
	static size_t Write_To_File(void *ptr, size_t size, size_t nmemb, FILE *stream);  // Save to file

	unsigned short ID_Content_Size;
	unsigned short *ID_Content_Array;  // All saved ID_Content, Handle by Get_ID
	wchar_t **Patterns_Array;  // Stored patterns from file, which user can formad, need to get titles, images 
	wchar_t *Title_Result;  // return result to called object
	wchar_t *Title_Site;  // Just need to store site url, download image

	std::wstring *Content_W;
};
//------------------------------------------------------------------------------------------------------------




// AsUI_Builder
class AsUI_Builder
{// !!! Build UI, Handle User Inputs, Load Save Content, 

public:
	~AsUI_Builder();
	AsUI_Builder(HDC hdc);

	void Builder_Handler(HDC ptr_hdc, const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam);

	EActive_Menu Active_Menu;
	HDC Ptr_Hdc;

private:
	void Draw_Menu_Main_Alpha();
	void Draw_Menu_Sub(const EActive_Menu &active_menu = EActive_Menu::EAM_Main);  // Sub Menu draw arrays from curr active button || User_Array_Map or User_Array_Library
	void Draw_User_Input_Button() const;  // Show user_input in sub menu
	void Handle_User_Input(const wchar_t &text);  // Add input to User_Input
	void Handle_RM_Button(const LPARAM &lParam);
	void Handle_LM_Button(const LPARAM &lParam);
	void Handle_Update_Button_Beta();
	void Handle_Active_Button_Advence();
	void Handle_Active_Button(const EActive_Button &active_button);  // Redraw Button in current Active Menu
	void Draw_Active_Button(const EActive_Button &active_button, std::map<std::wstring, SUser_Input_Data> &user_array);
	void Draw_User_Input_Request();  // Draw Request
	void Draw_User_Title_Image(const wchar_t *image_path) const;
	void Draw_User_Map(RECT &border_rect, std::map<std::wstring, SUser_Input_Data> &map);  // Draw Button in subMenu
	void Draw_Button_Text(const HBRUSH &background, const COLORREF &color_bk, const COLORREF &color_tx, const RECT &rect, const wchar_t *str) const;
	RECT Add_Border(const int &x_cord) const;
	RECT Add_Button(RECT &border_rect, const std::wstring &title) const;
	void Add_Button_Next_Page();
	void Context_Menu_Draw(const int &x, const int &y);
	void Context_Image_Save(const RECT &rect);  // Save image in rect
	void Context_Image_Restore(RECT &rect);  // redraw image
	void User_Input_Handle();  // Add to array
	void User_Input_Value_Is_Changed(const bool is_increment);  // Change active title num
	bool User_Input_Set_To_Clipboard();
	bool User_Input_Get_From_Clipboard(wchar_t *to_clipboard);  // and Re-Draw_User_Input
	unsigned long long User_Map_Load_Convert(unsigned long long &ch);
	void User_Map_Main_Load(std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path);
	void User_Map_Main_Save(const EActive_Menu &active_menu);  // Main Save | Threaded || Call this to save current std::map
	void User_Map_Init_Buffer(const std::map<std::wstring, SUser_Input_Data> &user_arr, const char *file_path);  // Format std::map to wchar_t ** after save
	void User_Map_Save_Array(const char *file_path, wchar_t **user_array, int user_input_counter);
	unsigned short User_Map_Save_Convert(unsigned short ch);
	void User_Map_Emplace(std::map<std::wstring, SUser_Input_Data> &user_arr, wchar_t *user_input);  // Save User_Input to User_Map
	void User_Map_Erase();  // Errase from Array
	void User_Input_Convert_Data(SUser_Input_Data &converted_data, wchar_t *user_input);

	wchar_t User_Input[AsConfig::User_Input_Buffer];  // User Input Buffer | or border_width / 8 = Max_Char_Length
	int Rect_Menu_List_Length;
	int Rect_Sub_Menu_Length;
	int Sub_Menu_Curr_Page;
	int Prev_Main_Menu_Button;
	int Prev_Button;
	int Main_Menu_Titles_Length_Max;

	const int Sub_Menu_Max_Line = 31;  // Нужен алгоритм что бы понять сколько влезет в пользувательский экран, или настроить через config
	const int User_Array_Max_Size = 50;  // нужно будет выгрузить из конфига

	// TEMP
	RECT Main_Menu_Border;

	AsConfig Config;
	EActive_Button Active_Button;  // If AB = 0 we init_sub_menu if not only draw Main menu, that`s all
	EActive_Page Active_Page;
	RECT *Rect_Menu_List;  // main menu buttons cords here when they`r created
	RECT *User_Input_Rect;  // user_inputs cords
	RECT *Rect_User_Input_Change;
	RECT *Rect_Buttons_Context;
	RECT Prev_Context_Menu_Cords;  // prev context cords
	RECT Rect_Pages[EActive_Page::EAP_Last];
	RECT Input_Button_Rect;  // User Input RECT
	HDC Hdc_Memory;
	HBITMAP H_Bitmap;
	HGDIOBJ Saved_Object;
	SUser_Input_Data User_Input_Data;
	std::map<std::wstring, SUser_Input_Data> User_Array_Map;
	std::map<std::wstring, SUser_Input_Data> User_Library_Map;
	std::map<std::wstring, SUser_Input_Data> User_Paused_Map;
	std::map<std::wstring, SUser_Input_Data> User_Wishlist_Map;
	std::map<std::wstring, SUser_Input_Data>::iterator It_Current_User;
	std::thread Thread_First;
	std::thread Thread_Second;
	std::thread Thread_Third;
	std::thread Thread_Fourth;

	static int Context_Button_Length;
	static int User_Input_Len;  // count user input after press enter set to zero
	static const wchar_t Main_Menu_Title_Name[];
	static const wchar_t *Sub_Menu_Title;
};
//------------------------------------------------------------------------------------------------------------



// AsUI_Book_Reader
class AsUI_Book_Reader
{
public:
	AsUI_Book_Reader(HDC hdc);

	//void Handle_Input(EKey_Type &key_type) const;

	HDC Ptr_Hdc;
};
//------------------------------------------------------------------------------------------------------------



// AsEngine
class AsEngine
{
public:
	~AsEngine();
	AsEngine();

	void Draw_Frame_ASaver(HWND hwnd);
	void Redraw_Frame(const EUI_Builder_Handler &builder_handler, const WPARAM &wParam, const LPARAM &lParam);
	void Get_Clipboard_From_Else();

	bool Is_After_Maximazied;
	AsUI_Builder *UI_Builder;
	AsUI_Book_Reader *UI_Book_Reader;

private:
	void Get_Current_Data_Time();  // !!! Temp Get curre
	void Draw_Frame_Book_Reader(HWND hwnd);
	int Draw_Frame_Chooser_Main_Mane(HWND hwnd);  // !!! bad
	int Connect_To_Server();  // Send to server ULL; change IP ADDRESS of serv || NEED TURN ON SERVER prog

	WPARAM W_Param;
	LPARAM L_Param;
	EUI_Builder_Handler EBuilder_Handler;
	HWND Ptr_Hwnd;
	HDC Ptr_Hdc;
	PAINTSTRUCT Paint_Struct;
};
//------------------------------------------------------------------------------------------------------------




// TASKS	--- 09.02.2024 ---
/* Completed
V	- Отрисувать сразу 3 или 2 меню?
V		- Создать switch переключения по меню?

V	- Сохранение и Загрузка данных:
V		- Загрузить данные из файла приоритет на те что нужно просмотреть а потом просмотренные
V			- Возможно создать разные файлые? Data.bin || Data_01.bin
V		- Сохранять при изменении, добавление
V			- После добавления, перерисувать USER_INPUT и всё меню или нет?
*/
// TASKS --- 13.02.2024 ---
/*
V	- Сделать обработку ENTER
V		- Ззагрузить из файла и вывести на экран
V		- Когда лучше всего загружать файлы? Когда нужно а когда не нужно загружать?
V		- Сохранить в файл и вывести на экран

V	- Удаления из списка |	11.02.2024
V		- если выбрана кнопка с главного меню удаления и выбрать кнопку с User_Array удалить из массива
V			- проблема остаеться свободное место | нету проблем
V			- так же удалить из файла
V				- когда перезаписивать? | Во время удаления

// TASK ---13.02.2024---
V	- Создать контекстное меню для кнопок, а может и не только, посмотрим
V		- в месте нажатия RMB:
V			- обработаем где нажалия RMB(
V			- создаем Rectangle, задать параметры
V				- ширина рамки должна быть шириной самого длиного текста кнопки
V				- висота + 6px от конца последней кнопки
				-	-||-
V			- создаем 2-4 Add_Button
V		- Сохранять координаты контекстного меню что бы потом зарисувать его
V			- Создать функции Context_Image_Save and Restore image
V				- use bitmap to save image
V				- use bitmap to restore image
V		- также нужна обработка кнопок самого контекстного меню
V			- Сохранить кординаты кнопок контекстного меню
V			- обрабативать нажимая LMB
V			- сохранять где то в ректе массив ректов что бы сравнить на что нажали
V			- if LMB press in Context Menu -> set_state -> delete_from_array else(example - > delete if selected sub menu button else do nothing
V	- Сделать наконец выход для програми по нажатию на кнопку
V	- Рефакторить сохранение и загрузку изображения контекстного меню

*/
// TASKS --- 14.02.2024 ---
/*	
--- Library_Array ---
V	- Создать новый массив "Просмотренные"?
V		- Хранить просмотренные в отдельном файле название примерно(Library.bin)
V		- Выгружать из файла(библиотеки) все данные и загружать их в User_Array_Library
V				- как удалять если придеться, вдруг законченное решили обновить,
X				- стоит ли делать проверку например если хочу что добавить что уже есть в законченом как удалить из библиотеки и переместить в data.bin
V		- Добавить в просмотренные через контекстное меню

V	- Модификации Init_Sub_Menu & Library_Array
V		- Если активная кнопка просмотра то выводим один список а если просмотренных то другой
V	- При нажатии на кнопку в главном меню <Просмотренные> отоброжать в саб меню список из библиотеки
V		- Нужно нарисувать поверх новое меню или просто изменить User_Array on User_Array_Library и просто перерисувать
V	- Нажимая на "Просмотренные" выгрузить из файла все(часть) данныех что бы вывести их для пользувателя
V	+ 1 Array на все данные, будет много файловых данных не так как в "просмотреть"
V		- Как добавлять в список? Выгружаем из Library.bin в файл библиотеки
V	- Удаление из Library_Array, как лучше? 
V		- не заморачивать и повторить методом User_Array
*/
// TASKS	--- 15.02.2024 - 29.02.2024 ---
/*
--- Watching Array ---
V	- Сделать инкримент серии
V		- вывести 2 кнопки + или - нажимая на которые можно увеличить или уменить количество просмотренных серий
V		- выделя кнопку с User_Array рисувать 2 кнопки + -
V	- Как добавлять серии, например блич 25 как сделать + 1 серия просмотренна?
V		- Создать новую кнопку с боку щелкая на неё увиличивать или уменьшать, нравиться больше но сложней

*/
// TASKS	--- 01.03.2024 ---
/*
 --- Clipboard future ---
V		- скопирував из браузера названия и тайтл серии, отобразить его в User_Input с помощью клавиш CTRL + V добавим лишь серию
X			- если будет FPSeconds можно убрать CTRL + V так обноволять скопируванный текст
X			- может даже эта функция будет форматирувать строки, посмотрим. ( В Config Setting возмонжо определить как форматирувать или задать)

 --- Config Setting ---
V		- Настройки для главного меню (Add Border нужно переделать )
V			- Сделать размер на несколько пикселей больше чем длина самого большого названия
V			- Возможно выгружать эти данные из файла Config || Пока только один параметр

V		- Настройки для дополнительного меню:
V			- изначальная длина будет зависит от User_Input
V			- 	При нажатии на CTRL + V заказать перерисовку саб меню, проверить максимальную длину если самая большая длина то сохранить
V			- Например есть стандарт для длини Watching Array в стоко то символов, можно сделать его статичним или по самому длиному тайтлу что будет отлично
V		- Создать файл настройки:
V			- загружать зарание указаную длину саб меню || Из Config.txt выгружаеться самый длиний тайтл который был добавлен через пользувательский ввод

*/
// TASKS	--- 02.03.2024 ---
/*
	--- Отложенные ---

V	--- Watching Array ---
V		- Поменять фон в саб меню на серий

V	- Начать тесты:
V		- узнать на какую длину максимально можно сохранить тайтл
V		- После перевибора закрасить старую область в цвет фона
		- Посмотреть в IDEAS что то для реализации

*/
// TASKS	--- 03.03.2024 ---
/*
V		- Пробуем сохранение || Переделать, проблемы в записи и выгрузке
V			- Загружаемся только когда заходим в приложение

V			- Сохраняем только после выхода из приложения
V				- Сохраняет только цифры и char 
V				- Только на русском
V				- сохраняет только User_Array
V				- При чтении придеться форматирувать || Формат типа делаем начало тайтла с заглавной
V						- как понять где конец строки? Последняя цифра, а что если добавлять без цифр? можно автоматом задавать 1 посмотрим
V				- когда нибудь сделать сохранение
V		- Пока нету продвинутого сохранения перезаписивать файл? Не так уже и плохо так как макс тайтлов 26 на производительность не скажеться

*/
// TASKS	--- 04.03.2024 ---
/*
V	- Поменять картинку, название, версию и тд

V	- Перемещения в библиотеку просмотренных --- curr ---
V		- Нужно зделать сохранения для библиотеки
V			- Можно тем же методом что и пользувательский ввод, только менять названия файла
V				- Такие же минусы сохраняеться только при заходе и выходе?
V				- Можно добавить так что бы при добавление сохранялось но данных будет слишком много, так что скорей всего посмотрим
V				- Сделать универсальный метод сохранение в который передаем путь сохранения и данные которые нужно сохранить? 
V					- Из минусов наверное много данных нужно передавать
V						- Массив
V						- Счетчик ввода
V						- Путь куда сохранять

V	- Save \ Load
V		- Рефакторинг старого сохранения и удаления
X		- недостатки при некоретном сохранении, данные могу не записаться что плохо, возможно создать интервальное сохранение?
		- Не перезаписивать файл если не было изменений
			- Создать переменную типа bool если были изминения перезаписивать
*/
// TASKS	--- 05.03.2024 ---
/*

--- Main Tags ---
V	- Pages:
V	- Сделать страницы
V			- Пока что обойдемься лишь 1 кнопкой след страницы
V		- На каждой странице например 40 строк
V			- Для этого нужна кнопка которая будет переключать страницы
				- Или сделать без страниц а по продвинутом поиске, тогда нужна клавиатура
V	- Сделать страницы для Библиотеки
V		- Создать кнопки для страниц? если будет привишать макс вместительность?

	--- Рефакторинг --- 
	- Обработка левой кнопки миши

*/
// TASKS	--- 06.03.2024 ---
/*
 --- TCP/IP ---

V	- Создать клиент
V		- отправлять ULL серверу для обработки
X		- Придумать для чего это ввобще нужно?
			- Возможно для рейтинга серий
				- Например 5 звезд или 4 и тд. 
			- Чат? Посмотрим
			- 
V	- Создать сервер
V		- принимать ULL от клиента для обработки
		- Добавить потоки для ожидания например через каждую секунду проверять?
			- Может запускать сервер в отдельном потоке?

*/
// TASKS	--- 09.03.2024 ---
/*
V		- Сделать что бы сразу добавлять в просмотренные если нужно заполнить базу
V	- Поиск по первому символу
X		- По последующих
*/
// TASKS	--- 10.03.2024 ---
/*
V	- Сортировка:
V		- Вводя текст по буквам выделяються из первой страници все доступные тайтлы.
*/
// TASKS	--- 11.03.2024 ---
/*
V	- Захват текста в свернутом окне, обработки нету, если нужно будет сделаем не тяжело
V		- Вставили в клан Engine? 
V			- Теперь являеться частю движка
*/
// TASKS	--- 17.03.2024 ---
/*
	- User_Input -
V		- Найти совпадение по текущему массиву
V			- Введя 1 символ проходимся по всему массиву и выделяем совпадения тоисть:
V				- Присваиваем Prev_Button значение и заказиваем перерисовку кнопку
					- Можно сделать ище круче, передалать весь массив под сортировку
V		- Исправить выход за пределы User_Input
V			- улучшить функцию Find_Matches

V	- Переименовать Save\Load
		- Отрефакторить Convert
V			- Добавить коментов/Инфо
V	- Отключить KeyboardHook
V	- Сделать наконец сезоны
*/
// TASKS	--- 18.03.2024 ---
/*
V	- Создать способ добавление картинки DirectXTex
V	- Нажимая на пользувательский User_Input, создать картинку если она добавлена
V		- Отобразить её под главним меню
V			- Перерисувать:
V				- главное меню вмести с картинкой
V				- саб меню вмести со списком
X		- Если картинки нету, ничего не менять
			- Остаеться старая картинка, так будет интересней, возможно добавить опцию, посмотрим
*/
// TASKS	--- 19.03.2024 - 31.03.2024 ---
/*

V	- Перенести все данные в старой(консольной) версии ASaver в оконую новую
		- Перенести данные из библиотеки(консольной) в новую, создать функцию?

V	- Advence finder:
V		- Вводя в поле User_Input например букву Б, пройтись по всему списку User_Array найти заглавную букву Б и перекрасить её в красный и обратно
X			- при этом можно выводить её первой в список, но для этого потребуеться перерисувать саб меню

*/
// TASKS --- 01.04.2024 ---
/*
V	- Подключить библиотеку STD
V		- Передалать всю логики с std::wstring && std::map
V			- Передать главное меню в std::wstring
V			- Передать дополни меню в std::wstring
V				- Добавлять через пользувательский ввод данные
V					- Сделать для них кнопки
V	- Рефакторининг
		- Розширил начальное меню до изображения картинки(50 символов в конструкторе)
*/
// TASKS --- 02.04.2024 ---
/*
V	- Потестить выход за пределы массивов, решить проблемы
V	- При добавление данных сразу сортируються по алфавите и перерисовываются
V	- Переделиваем сохранение
V	- Сделать удаление
V	- Config по закритии програми сохранить в файл Configa User_Input_Counter что бы после открития взять из него User_Array_Max_Size
V		- Нужна для него структура
V			- Безлимитное хранение тайтлов

V	- Sorting:
		- Нумерация данных?
V		- Сортировка по алфавиту
V			- Через std::
*/
// TASKS --- 03.04.2024 ---
/*
V	- Зациклить функционал след страницы(можно переходить на след страницу, возрат если последняя)
V	- Сбрасивать страницу на начальную
*/
// TASKS --- 04.04.2024 ---
/*
V	- Вводя Тест 1 Тест 2 удалить 1 и оставить второй или переписать первый
V	- Понять хороша ли идея
V	- Сделать запрет на добавление существуюющых данные(повторение)
V		- Старые данные удалить, новые добавить
*/
// TASKS --- 05.04.2024 ---
/*
V	- Механика сезонов I V X
V		- Если добавлять те же данные но новый сезон тогда удалить редактирувать старый и добавить новый
		- Не проверял обратный порядок но это неправельно или что

V	- Сохранять данные и выгружать
*/
// TASKS --- 06.04.2024 ---
/*
V	- Провести тестовый заход, найти баги
V		- Добавили 22 тайтла
V			- Удалили старые
V		- Починили удаление через контекстное меню

V	- Посмотреть можно ли SUser_Input_Data std сделать wchar_t
		- Пока оставим памяти не так много

V	- Поправить главное меню
V	- Исправили баг с выводом тайтлом
		- улучшили код удаления

V	- Исправить контекстное меню
V		- очень маленькая ширина
*/
// TASKS --- 07.04.2024 ---
/*
V	- Сделать инкримент для серии ( - + )
V		- Нужно зделать добавление сезонов
X		- Зарисувать то что осталось
V	- Не сохраняеться X или не выгружаеться
V	- Рефакторинг добавления в массив
*/
// TASKS --- 08.04.2024 - 09.04.2024 ---
/*
V	- Сделать массив законченные, обновить текущый не реализованный с std::
V		- Создать отдельный массив библиотека
V		- Сохранять так же законченый массив
V			- Рефакторинг автосохранения

V	- Ошибка перерисовки кнопок
V	- Переделать полностю handle
V	- Оптимизирувать обработку кнопок миши
V	- Добавил список желаемых
V	- Переделать контекстное меню
V	- Переделать перемещение по масивам

*/
// TASKS --- 10.04.2024 - 12.04.2024 --- 
/*
V	- Добавить инкримент для данных
V		- При нажатие на пользувательский ввод отрисувать запрос на добавление или уменьшение
V		- Увиличить текущые данные на 1 при нажатии на LMB
V		- При виборе других данных стереть пред запрос и нарисувать новый
V			- Так же уменьшил размер для красоты

V	- Под главним меню изображение 404, создать размер
V	- Форматирувать ввод данных
V		- Забил пробел добавить перед символом или сезоном

V	- Рефакторинг, перемещение удаление по разним массивам с помощью switch && gui

*/
// TASKS --- 13.04.2024 ---
/*
V	- Добавлять сразу в Библиотеку, или wishlist которого ище нету
		- После нажатия enter в текущый выделенный массив добавить User_Input

V	- После удаления данных стереть запрос на инкримент

V	- Добавить wishlist, paused map, обрабативать в Draw_Menu
V		- Создать функцию которая будет отрисовывать map

V	- Внедрить файловую систему, что бы не засорять главную папку

V	- Сохранять paused and wishlist в отдельные файлы
V		- Отложить потому что сохранение будут переделиваться
*/
// TASKS --- 14.04.2024 ---
/*

V	- Изменить фон на черный, шрифт на зеленный

V	- Брать url из сайта с помощью curl
V		- Конвертирувать в wstring

*/
// TASKS --- 15.04.2024 ---
/*
V	- Создал специальный класс в который можно добавлять URL
V		- в wstring записиваем все данные c url (так же русс символы которые теперь можем форматирувать
X			- главная задача получить тайтл сезон и серию
*/
// TASKS --- 16.04.2024 - 17.04.2024 --- 
/*
V	- Создал специальный класс в который можно добавлять URL
V		- в wstring записиваем все данные c url (так же русс символы которые теперь можем форматирувать
V			- главная задача получить тайтл сезон и серию
V	- <h1> &laquo;Гордый владыка вечности&raquo; ТВ-1 </h1>
V		- найти <h1> после все символы за 1000+ добавить в User_Array ТВ-1(скип) если ТВ-2(II) когда начинаеться < перестать добавлять
V	- <div class='anime_page_upload'>Добавленно: <span itemprop='dateCreated'>13 апреля 2024</span> | Серии: [17 из 20]</div>
V		- найти [ и добавить текущую серию 17, и поставить L'\0' в wchar_t User_Input
V		- Так же нужно как то выделить серию из примера ниже
V			- Пример:
V				<div class='anime_page_upload'>Добавленно: <span itemprop='dateCreated'>12 апреля 2024</span> | Серии: [292 из 380]</div>
V		- Найти Тайтл, сохранить название, выделить id
V	- Брать url из сайта с помощью curl
V		- Конвертирувать в wstring

V	- Может создать сущность которая будет принимать URL обрабативать и получать от нужную нам структуру?

*/
// TASKS --- 18.04.2024 ---
/*
V	- Сделать рефакторинг кода, избавиться от утечек, отрефакторить ACURL
V	- <img src='/src/img/content/Wangu_Kuang_Di__The_Proud_Emperor_of_Eternity_TV_1_648057337desctop.webp' width='200' height='300'
V		- картинку можно сохранить, но как вопрос,
V		- Картинку скачать, переименувать, сохранить в отдельную папку
V			- id можно использувать для сохранения картинки, уникальное имя
V			- Сохраняем картинку в главную папку под названием TEMP
*/
// TASKS --- 19.04.2024 ---
/*

V	- Обработать сезон при чтении URL если Боево 2 345 нужно 2 перевести в II или что то придумать посмотрим
V		- Рабочая вверсия хоть и ограниченная
V	- Рефакторинг User Input Convert Data()
V		- Обработка сезонов пробелами
V		- Если вставлять через URL теперь работает обработка

*/
// TASKS --- 20.04.2024 ---
/*
V	- Доделать сезоны, Тест II 3 не выводиться
V	- После добавление через URL
V		- Сохранить картинку в папку Current_Array/Pictures/ *.png
V			- изначально картинка добавляеться в Pictures/Temporary.png
V			- хорошо если все картинки будут в одном месте
V				- из минусов пока что не вижу
V			- определить где лучше сохранять
V	- Выводить картинку при нажатии на title
V		- Перенести из движка функцию вывода изображение в UI_Builder
V		- Её можно реализувать или в User_Input_Reguest или отдельно
V	- Куда сохранять картинку и после обработки тоже её нужно куда то сохранить
V		- Переименнувать под ID
V		- Сохранять в Picture / Library / key.png
V	- Поменять цвет для вставки в пользувательский ввод(User Input)
*/
// TASKS --- 21.04.2024 ---
/*
V	- Указать в User_Input URL обработать с помощью curl
V		- Получить такие данные как
V			- Сезон, Серию, Дату добавления, Название
V	- Указать запрос куда сохранять файл и откуда выгружать данные
V		- Использувать #include <filesystem> // Для работы с файловой системой
V	- Что то делал, забыл записать)
*/
// TASKS --- 22.04.2024 ---
/*
V	- Запускаем тест драйв
V		- Исправили баги в релизной версии
V		- улучшили код в курле
V		- протестирувать добавление с сезонами что скорей всего поламает мне всё
			- Всё сработало на удевление
*/
// TASKS --- 23.04.2024 - 24.04.2024 ---
/*
X	- Проблема с ё конвертации
V	- Нажим user input add to arra
V	- Переделал User_Adjust теперь производительней и переделана логика
V	- Починил сохранения
V		- Пофиксил перенос данных от одного массива к другому что бы, исправил добавления сайтов
*/
// TASKS --- 01.05.2024 ---
/*
V	- Не сохраняет " ',' ':' "
		- Форматирувать User_Input
			- Заменить пробелами
				- такой как формат 3D не сработает в названии
V	- Перенести в С++ Main
*/
// TASKS --- 12.05.2024 ---
/*
V	- Форматирувать название картинок которые сохраняються в файл, изменять ё на е
		- Форматируем сразу при получение данных, нет необходимости лезть куда не нада)
*/
// TASKS --- 13.05.2024 --- Second Realize
/*
V	- Рефакторинг
V		- Избавились от всех hdc в классе и функциях
V		- Сделали лишь одну переменую

V	- Сохранять в отдельный
V		- Сохранение после добавления, сохраняеться в отдельных потоках
V		- Новая функция которая авто сохраняет после удаления, добавление изминения, и выходе из программы

V	- Задание для меня на след месяц
V		- Начать работать с потоками, переделать курс для сохранения файлов в текстовый документ для последущией обработки

V	- Желаемое, например добавить в список то что ище не вишло

V	- Создали синглтон для AsMain
		- Все функции теперь в классе
*/
// TASKS --- 15.05.2024 ---
/*
V	- Отправляем на тесты)
		- Ошибки, вилеты:
			- Перерождение Монстр 23 
			- седьмой принц 23
*/
// TASKS --- 18.05.2024 --- 
/*
V	- Пофикшены два бага, с неправельним отображением данных
V	- После добавление данных выделяем эти данные
V	- Исправили на какое то время утечку данных
*/
// TASKS --- 19.05.2024 ---
/*
* 
V	- Draw_Button Next_list
V		- If presed redraw all Sub Menu
V			- Draw from curr line to sub menu
V	- Draw_Button Prev_list
V		- If presed redraw all Sub Menu
V			- Draw from curr_lin - Sub_Menu_Max_Line

V	- Есть переход по страницам, немного рефакторинга
*/
// TASKS --- 23.05.2024 ---
/*
V	- Добавить кнопку пролистивания, обновления всего окна,
V	- Можно добавлять на 2 странице, и с первой на 2ую без багов.
V	- Баг конекстного меню на страницах 2+
V	- Отрефакторили много функций, улучшили контекстное меню, вырезали ненужный код
V	- Теперь виделяя тайтл он копируеться в буфер обена, можно вставить название в поисковик
V	- Контекстное меню работает так же хорошо на первой странице и на второй и последующых
V	- url from file to cpy
*/
// TASKS --- 26.05.2024 ---
/*
V	- fixed bugs with L'<<' now work good
*/
// TASKS --- 1.06.2024 ---
/*
V	- Добавил выбор между программами
V	- Обнаружил кучу багов которые не пойми как появляються
V		- Будем монитроить и фиксить

V	- Добавить возможность выбора с какой программы стартувать
*/
// TASKS --- 18.06.2024 - 21.06.2024 ---
/*
V	- Tools
		- Save, Load, Format_text

V	- Добавлять в структуру ID Title

V	- Создать в Tools статический метод сохранения входящих параметров
V	- Доставать из url index(short)

V		- Как сохранять структуру?
			- Создать класс сохранение, в котором будут:
				- // !!!
*/
// TASKS --- 23.06.2024  ---
/*

V	- Fix Going to page - 1 >

X	- Переделать сохранение
V		- Сохранить сайт, потом id (user input)
X		- Создать структуру в которой храним:
			- ID URL
				- Файл називаеться например anime-bit в нём храним id

*/
// TASKS --- 29.06.2024  ---
/*

V	- Format url to site and ID Content
V	- Save ID Content to bin, don`t save same id
V	- Load ID_COntent from bin

*/
// TASKS --- 30.06.2024  ---
/*

V	- Переделанно сохранения в ACurl_Component при создании и удаленеие обэкта


V	- После выхода из програмы не сохраняет.... Fixed

V	- ACurl_Component передалать сохранять и загружать конструктор, деструктор

V	- Make url to start parsing
V		- При нажатии кнопки пройтись по массиву, взять из структуры ID зайти на сайт, проверить доступную серию.

*/
// TASKS --- 01.07.2024  ---
/*

V	- Если серия другая изменить цвет кнопки и перерисувать
V	- Add to clipboard url with id, can go site and watch
V	- Double click on User_Input_Button to update new watched

*/
// TASKS --- 03.07.2024  ---
/*
V	- When press update button
V		- if not in main array delete ID Content from base
*/
// TASKS --- 04.07.2024  ---
/*
V	- Correct delete while title was watching
V		- While 12 / 12 Add to watched
V	- Refactoring How to get folder ?
V	- Good Add ID Content and delete while 0 - 1
V	- Have bugs while not show to user input button
V	- Refactoring

V	- Добавить новую кнопку после которой будут происходить такие действия
V		- Создали enum для кнопок, 3 кнопки для Page Update, Prev, Next
V			- Update Page || CURL
V				- Зайти на сайт, найти тайтл, если серия отличаеться от просмотренной:
V					- перекрасить кнопку в цвет поярче на *50

V	- Возможно сохранять и сам URL что бы скопирувать его в буфер и легко вставить в бразуер?
*/
// TASKS --- 05.07.2024  ---
/*
V	- Работа над Потоками
*/
// TASKS --- 07.07.2024  ---
/*

V	- Убрать функционал добавления в clipboard


V	- Пофиксить баг когда User_Input не отображаеться, и при клике на него мерджаться тайтлы
*/
// TASKS --- 09.07.2024  ---
/*
V	- fix while ID Content less then 1000 it`s have access to wrong url, fixed.
V		- Потоки не конфликтуют
*/
// TASKS --- 10.07.2024  ---
/*
V	- Улучшить конвертацию, добавил english
V	- Рефакторинг Convert
V	- better converting, performance ++
*/
// TASKS --- 11.07.2024 - 17.07.2024  ---
/*
V	- Внедрить новую конвертацию
V		- Протестирувать сохранения
V		- Добавлять english Titles

V	- Переделать ACurl:
V		- Сохранять в файл, потом его редактирувать
V			- Причина, может попасть половина данных, и это никак не обрабативаеться

V	- Проверить потоки, кнопка Update работает пока стабильно тестируем дальше

*/
// TASKS --- 18.07.2024 ---
/*
V	- Поиск начала тайтла по патерну а не по строке, надёжней
V	- Рефакторинг движка 
V	- Создать запрет на ввод в меню

V	- Добавлять фильмы, или сериалы.
*/
// TASKS --- 19.07.2024 --- 
/*
V	- Use Threads for Update_Button
V	- Добавить поток, в котором будет обработка curl
V		- Когда укажут URL при нажатии дважди или ENTER в не основной поток передать обработку входний данных
V			- По завершению, обновить добавленные данные
V			- Больше одного сайта или страниц, добавить многопоточность
*/
// TASKS --- 20.07.2024 - 24.07.2024 ---
/*
	- Refactoring Curl Class
*/
// TASKS --- 26.07.2024 ---
/*
V	- Create .txt file with filter example:
V		- title bgn = "user must write here title start after this words"
V			- and else where
V		- while fill and all is alright, read fill array and continue
V		- if not say those to user
V	- Send user massage to fill all new created patterns
V	- Check if already exists setting

V	- load pattern for result

X	- Problem with loading some site`s picture`s
X	- A lot of memory leak, need fat refactoring class CURL_Handler

*/
// TASKS --- 25.07.2024 - 01.08.2024 ---
/*
V	- ACurl_Component соеденить с ACURL

V	- Брать из файла Config pattern для поиска по URL
V	- Проводить тесты, до конца месяца, как работает программа, отлавливать баги и фиксить
X	- Финальный рефакторинг, переносить по своим .h .cpp
*/
// TASKS --- 02.08.2024 - 03.08.2024 ---
/*
V	- Refactoring CURL
*/
// TASKS --- 04.08.2024 ---
/*

V	-	Problem with current page
V		- Fixed, architecture sucks, also need refactoring
V	- Threads load files, 4m Tacts
*/
// TASKS --- 05.08.2024 --- Current ---
/*


*/

// MAIN TASKS
/*
--- MOST WANTED ---
X	- Добавиить опцию, увиличение шрифта?
		- Не тяжело но нужно ли

X	- Добавить отоборжение страниц?

--- Optional ---
X	- Добавить функционал сохранения даты последнего редактирувания
X		- Где отображать эти данные?
X		- Дата добавления данных, и редактирувание их и тд.
		- Записать в ui_data LL сохранить
			- выгружать после добавления в map


--- IDEAS ---
X	- Сжатие файлов, картинок


--- Data Futures ---

--- Sub Futures ---
X	- Реализувать горизонтальный бордер
X	- Backups
		- Может создать резерв копию если нужно

Сложность *** Elite *** Senior Level
X	- Информационная панель
x		- Данные о тайле, где их хранить и тд?
			- Создать новый border, buttons
			- Записивать время добавления серии и хранить её где то
			- Пользувательская картинка которую он захочет добавить ***
			- Возможно добавить рейтинг( в виде звездочек? =) )

*/




// SITES
/*
RU
https://anistar.org/ || https://anistar.org/9791-pererozhdenie-monstr-remonster.html
https://www.anilibria.tv/ || https://www.anilibria.tv/release/maou-gakuin-no-futekigousha-ii-part-2.html

ENG
https://animixplay.st/ || https://animixplay.st/v1/ninja-kamui/ep11

*/
// Bugs
/*
X	- Проблемы шрифта, некоторые символы от 7 до 8 пикселей, поэтому нельзя просто взять и по пикселям выравнивать рамку
X	- Проблемы цвета в картинка, хз в чем проблема
*/
// Fixed
/*
V	- После перемещения в другой массив остаеться запрос, нужно убрать
V		- Запрос на данный момент убираеться другим запросом
V	- После добавление через URL без сезона, после загрузки добавляет сезон 2
V	- При вводе User_Input ( Проблески тьмы 1) отображаться только названия без 1. КАК ТАК?
V	- Удаляя из библиотеки удаляються файли и массива тех данных что нужно просмотреть
V	- Request(); если 9 + 1 будет 0 но не увиличит 1 на 2
V	- Request(); если 10 уменьшить на 1 нужен код
V	- после инкримента "Ну добавим новый тест 1" - не сохраняет и не загружает
V	- плохой код сохранения \ загрузки, переделать
V	- Слишком длиная рамка для второго меню, кажеться из-за точек, разобраться когда нибудь
V	- После удаление остаеться кнопка
V	- При переходе в библиотеку вылетала ошибка перерисовки предидущей кнопки
V	- Если попитаться добавить данные в файл просмотренные, добавляет в основной список
V	- Ошибка перерисовки
V	- ё не конвертируеться || Лучший зять всех времён II || <Пожиратель звёзд> ||
V	- утечка памяти при удалении пользувательского массива в конструкторе удаления
V	- Потеря данных когда при вигрузке из файла, когда LL содержал лишь 1 символ(return -> break;)
V	- Если в концу есть пробел то это плохо)
V	- Если записанно в файлйе много данных а в конфиге нету его количиства выдаеться ошибка
		- проблемы не будет если начинать с 0
		- но что если кто то скопирует базу, и поставить новую программу? плохо, нужно решить
V	- после удаления первого и последнего поль ввода остаеться пустая рамка в Library

*/
// New Phase
/*
	- Сделать Рефакторинг когда всё будет отлично работать
		- Сохранение в файл 80% хорошо -
		- Загрузка из файла 80% хорошо - много лишний итераций если тайтл длиней ща 8 символо и до 10
*/
// Futures from 01 to 100
/*
	len = (int)wcslen(User_Array[i]) + 5;
	next_level_line = new wchar_t[len];
	count = 4;

	if (count < len)
	{
		next_level_line[0] = '0' + i / 10;
		next_level_line[1] = '0' + (i % 10);
		next_level_line[2] = '.';
		next_level_line[3] = ' ';

		while (count < len)
			next_level_line[count++] = User_Array[i][count - 4];
	}
	delete []next_level_line;
*/
// FUTURES FAST FIND 
/*
	wchar_t result[100] {};
	wchar_t url[] = L"https://animevost.org/tip/tv/3183-tadaima-okaeri.html";
	wchar_t *ptr_start = wcsstr(url, L"animevost");

	// Находим совпадение с доступними сайтами
	if (ptr_start != 0)
	{
		wchar_t *ptr_end = wcsstr(ptr_start, L".");
		if (ptr_end != 0)
		{
			size_t length = ptr_end - ptr_start;
			wcsncpy_s(result, 100, ptr_start, length);
			result[length] = L'\0';
		}
	}
	// Если находим такой сайт в базе можно вигрузить паттерн для собственного класса CURL

	// Сохраняем в Config? Или может передать через конструктор напрямую
	ACurl_Client client_url(url);  // reset from url to User_Input

*/
// TEMP
/*

	- Суть определить влияние шумов изображения при распознавании частотными методами.
		- Преобразование входного изображения из цветного в оттенки серого
		- Бинаризация
		- Двухмерное преобразование Фурье
		- Превращение результата в лог-полярные координаты
		- Кросс-корреляция входных данных с каждой цифрой из базы данных 
	- После этого заменяем лог-полярные координаты на полярные и сравниваем эффективность.

*/
// DRXT
/*
	IDXGIFactory* dxgiFactory = 0;
HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
if (FAILED(hr))
	return 1;

// Поиск первого адаптера
IDXGIAdapter *adapter = 0;
hr = dxgiFactory->EnumAdapters(0, &adapter);
if (FAILED(hr) )
{
	dxgiFactory->Release();
	return 1;
}

// Получение информации о графическом адаптере
DXGI_ADAPTER_DESC adapterDesc;
adapter->GetDesc(&adapterDesc);

// Вывод информации о графическом адаптере
wchar_t *description = adapterDesc.Description;  // Get Addapter Descriptions

// Освобождение ресурсов
adapter->Release();
dxgiFactory->Release();
*/
// TASKS --- REGUESTS
/*
* 	- Stoped at http://loveread.me/read_book.php?id=92157&p=14
*/
