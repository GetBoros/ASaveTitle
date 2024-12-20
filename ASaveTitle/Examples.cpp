#include "stdafx.h"
#include "Examples.h"

// AModule_Menu
void AModule_Menu::Button_Create()
{

}
//------------------------------------------------------------------------------------------------------------
void AModule_Menu::Button_Remove()
{

}
//------------------------------------------------------------------------------------------------------------




// AModule_Menu_Option
void AModule_Menu_Option::Button_Create()
{

}
//------------------------------------------------------------------------------------------------------------
void AModule_Menu_Option::Button_Remove()
{

}
//------------------------------------------------------------------------------------------------------------




// AActor
AActor::~AActor()
{

}
//------------------------------------------------------------------------------------------------------------
AActor::AActor()
 : Func_Ptr(0), Func_Address(0)
{
	Init();
}
//------------------------------------------------------------------------------------------------------------
void AActor::Init()
{
	Func_Ptr = reinterpret_cast<void *>( &Func_Address);
	Func_Address = &AActor::Call_Handler;
	Func_Handler = [this]() { (this->*Func_Address)(); };

	(this->*Func_Address)();
}
//------------------------------------------------------------------------------------------------------------
void AActor::Call_Handler()
{
	if (Func_Address != 0)
		Func_Address = &AActor::Call_Me;
	else
		Func_Address = &AActor::Call_Me_Adnveced;
}
//------------------------------------------------------------------------------------------------------------
void AActor::Call_Me()
{
	int yy = 0;
}
//------------------------------------------------------------------------------------------------------------
void AActor::Call_Me_Adnveced()
{
	int yy = 0;
}
//------------------------------------------------------------------------------------------------------------





// AClient
unsigned long long AClient::Data_Size = 6;
//------------------------------------------------------------------------------------------------------------
AClient::AClient(const unsigned long long *data_to_send, int len)
 : Client_State(EClient_State::ECS_None), Buffer_To_Server(0), Socket_To_Server {}, Address_Server {}
{
	Client_State = EClient_State::ECS_Server_Buffers;  // first time check data correct

	Data_Emplace(data_to_send, len);  // - Refactor
	Client_Handler();

}
//------------------------------------------------------------------------------------------------------------
void AClient::Client_Handler()
{
	if ( !(Client_State != EClient_State::ECS_Failure) )
		return;

	switch (Client_State)
	{
	case EClient_State::ECS_Server_Connect:
		Connect_Server();
		break;
	case EClient_State::ECS_Server_Sending:
		Send_To_Server();
		return;
	case EClient_State::ECS_Server_Buffers_Failed:
	case EClient_State::ECS_Failure:
		return;
	}
	Client_Handler();
}
//------------------------------------------------------------------------------------------------------------
void AClient::Connect_Server()
{
	WSADATA wsocket_data;

	Client_State = EClient_State::ECS_Failure;  // !!! refactoring to return this value, not now
	if (WSAStartup(MAKEWORD(2, 2), &wsocket_data) )
		return;
	Socket_To_Server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 1.1. Создаем сокет и задаем адрес сервера
	if (Socket_To_Server == INVALID_SOCKET)
	{
		WSACleanup();
		return;
	}

	Address_Server.sin_family = AF_INET;
	Address_Server.sin_addr.s_addr = inet_addr("127.0.0.1");  // IP-адрес сервера
	Address_Server.sin_port = htons(666);  // Порт сервера

	if (connect(Socket_To_Server, (sockaddr *) &Address_Server, sizeof(Address_Server) ) == SOCKET_ERROR)  // can`t connect server
	{
		closesocket(Socket_To_Server);
		WSACleanup();
	}

	Client_State = EClient_State::ECS_Server_Sending;  // first time check data correct
}
//------------------------------------------------------------------------------------------------------------
void AClient::Send_To_Server()
{
	if (send(Socket_To_Server, (char*)&Data_Size, sizeof(Data_Size), 0) == SOCKET_ERROR)
		AsConfig::Throw();

	if (send(Socket_To_Server, (char *)Buffer_To_Server, sizeof(unsigned long long) * (int)Data_Size, 0) == SOCKET_ERROR)
		AsConfig::Throw();

	closesocket(Socket_To_Server);
	WSACleanup();

	delete[] Buffer_To_Server;
}
//------------------------------------------------------------------------------------------------------------
void AClient::Data_Emplace(const unsigned long long *data_to_send, int len)
{
	if (!data_to_send != 0)
		Client_State = EClient_State::ECS_Server_Buffers_Failed;
	else
	{
		Data_Size = (unsigned long long)len;
		Buffer_To_Server = new unsigned char[sizeof(unsigned long long) * len] {};
		memcpy(Buffer_To_Server, data_to_send, len * sizeof(unsigned long long) );  // cpy data to char *
		Client_State = EClient_State::ECS_Server_Connect;
	}
}
//------------------------------------------------------------------------------------------------------------



// AsExamples
AsExamples::AsExamples()
 : Client(0)
{

}
//------------------------------------------------------------------------------------------------------------
AsExamples::AsExamples(const EShow_Preview show_preview)
 : Client(0)
{
	// 1.0 EP_Show_Path_Info Data
	std::filesystem::path path_to_display(L"C:/FileSystemTest/SubDir3/SubDirLevel2/File2.txt ");

	// 2.0. EP_Show_Func_Ptrs Data
	switch (show_preview)
	{
	case EShow_Preview::EP_None:
		break;
	case EShow_Preview::EP_Show_Path_Info:
		Display_Path_Info(path_to_display);
		break;
	case EShow_Preview::EP_Show_Byte_Info:
		Display_Byte_Info();
		break;
	case EShow_Preview::EP_Show_Void_Ptrs:
		Display_Void_Ptrs();
		break;
	case EShow_Preview::EP_Show_Func_Ptrs:
		Display_Func_Ptrs();
		break;
	case EShow_Preview::EP_Show_Func_Ptrs_Advenced:
		Display_Func_Ptrs_Advenced();
		break;
	case EShow_Preview::EP_Show_Constexpr_Examples:
		Display_Constexpr_Examples();
		break;
	case EShow_Preview::EP_Show_Cast_Exam:
		Display_Cast_Exam();
		break;
	case EShow_Preview::EP_Show_Html_Decode:
		Display_Html_Decoder();
		break;
	case EShow_Preview::EP_Show_Conect_To_Servers:
		Display_Connect_Server();
		break;
	case EShow_Preview::EP_Show_Std_Map_Pair_Ptrs:
		Display_Map_Pair_Ptr();
		break;
	case EShow_Preview::EP_Show_FFmpeg_Ex:
		Display_FFmpeg_Commands();
		break;
	case EShow_Preview::EP_Show_FFMpeg_Example:
		//Display_FFmpeg_Examples();
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Show_Case(EShow_Preview example_preview)
{

}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Path_Info(std::filesystem::path &path)
{
	int i;
	std::wostringstream w_string_stream;  // stor folder

	i = 0;
	w_string_stream << L"Displaying path info for: " << path << std::endl;

	// 1.0. Get Parts useing *itr
	for (std::filesystem::path::iterator itr = path.begin(); itr != path.end(); ++itr)
		w_string_stream << L"path part: " << i++ << L" = " << *itr << std::endl;

	// 2.0. Format stream with filesystem help
	w_string_stream
		<< L"root_name() = " << path.root_name() << std::endl
		<< L"root_path() = " << path.root_path() << std::endl
		<< L"relative_path() = " << path.relative_path() << std::endl  // full path withour root path
		<< L"parent_path() = " << path.parent_path() << std::endl  // full path without file name
		<< L"filename() = " << path.filename() << std::endl  // file with format
		<< L"stem() = " << path.stem() << std::endl  // file format
		<< L"extension() = " << path.extension() << std::endl;  // .txt

	int yy = 0;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Byte_Info()
{
	int number = 65535;

	const size_t type_size = sizeof(int);  // if (int = 4 byte) its (255 max DEC)
	std::bitset<8UI64> bits;  // (1 1 1 1) - 255 DEC | 1 1 1 1 || bits ||
	std::array<std::byte, type_size> bytes_arr = std::bit_cast<std::array<std::byte, type_size> >(number);

	for (std::byte byte : bytes_arr)
		bits = std::bitset<8>(std::to_integer<unsigned char>(byte) );  // 
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Void_Ptrs()
{
	double main_vector[] = { 1.0, 2.0, 3.0 };  // Main Data
	void *vector_ptr = (void *)main_vector;  // convert to void ptr
	double *ptr_listenter = (double *)vector_ptr;  // from void ptr to double
	double a = ptr_listenter[2];  // check if ok
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Func_Ptrs()
{// For what? but we can so...

	void(AsExamples:: *ptr_func_arg_0)() = 0;  // can get ptr to func with zero arguments and type void
	void (AsExamples:: *ptr_func_arg_1) (EShow_Preview) = 0;  // void - return type || ( *ptr_func_arg_1)  - name ||  (int) - func args || = 0;  = &Say_Hello; - adress func with arg int||
	AsExamples experemental;

	// 1.0 Inits
	ptr_func_arg_0 = &AsExamples::Display_Void_Ptrs;  // get func adress
	ptr_func_arg_1 = &AsExamples::Show_Case;

	// 2.0. Cast to (void *) ptr can send from empry args
	void *ptr = (void *)&ptr_func_arg_0;  // to void *
	void (AsExamples::*restored_ptr_func)() = *static_cast<void (AsExamples:: **) () >(ptr);  // from (void *)

	// 3.0. How to call
	(experemental.*restored_ptr_func)();  // Call func
	(this->*restored_ptr_func)();  // Call func
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Func_Ptrs_Advenced()
{
	void (AActor:: *func_actor)() = 0;
	void (AsExamples:: *ptr_func_update)(EShow_Preview) = 0;  // Тип(void) (где, указателя на функцию-член класса)(аргументы) = нулевой указатель
	void *ptr_func = 0;  // Тип, имя указателя общего типа void * (универсальный указатель) может указывать на любой тип данных
	uintptr_t func_addr = 0;  // Тип, имя адресса для хранения целочисленного представления адреса указателя
	void ( *restored_func_ptr)(EShow_Preview) = 0;  // Объявление указателя на функцию которая принимает аргумент типа EShow_Preview и возвращает void
	AActor actor;  // Example how to work with other class

	func_actor = &AActor::Init;
	(actor.*func_actor)();
	(actor.*func_actor)();

	// Преобразование указателя на функцию
	ptr_func_update = &AsExamples::Show_Case;  // Назначение указателю ptr_func_update адреса функции-члена Show_Case класса AsExamples.
	(this->*ptr_func_update)((EShow_Preview)0);  // Call func
	ptr_func = reinterpret_cast<void *&>(ptr_func_update);  // Приведение указателя, на функцию-член класса к типу void *.
	func_addr = reinterpret_cast<uintptr_t>(ptr_func);  // Преобразование void * в uintptr_t полезно передать адрес функции между модулями, сохранить его для последующего использования

	// Восстановление указателя на функцию и вызов функции
	restored_func_ptr = reinterpret_cast<void (*) (EShow_Preview)> (func_addr);  // Привести func_addr в void * c arg. Необходимо, чтобы корректно вызвать функцию через указатель.
	restored_func_ptr(EShow_Preview::EP_None);  //  Вызов функции через восстановленный указатель и передача аргумента типа EShow_Preview.
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Constexpr_Examples()
{
	int a = Func();
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Cast_Exam()
{
	struct SBase
	{
		int A = 0;
	};
	struct SSame
	{
		int B = 0;
	};

	SBase base;
	base.A = 5;
	SSame *same = reinterpret_cast<SSame *>(&base);
	int yy = base.A;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Html_Decoder()
{
	unsigned int unicode_char;
	std::size_t bgn, found, end;  // Begin hex, found hex end, end - index of ;
	std::string hex_code;
	std::string html_encoded;
	std::wstring decoded_str;
	std::stringstream str_strm;

	unicode_char = 0;
	bgn = 0, found = 0, end = 0;
	hex_code = {};
	html_encoded = "&#x414;&#x438;&#x441;&#x43A;&#x432;&#x430;&#x43B;&#x438;&#x444;&#x438;&#x446;&#x438;&#x440;";
	decoded_str = {};
	str_strm << std::hex;

	while ( (found = html_encoded.find("&#x", bgn) ) != std::string::npos)
	{
		decoded_str.append(html_encoded.begin() + bgn, html_encoded.begin() + found);  // Копируем часть строки до кодировки
		end = html_encoded.find(';', found);
		if (!(end != std::string::npos) )
			break;  // Если нет ';', прекращаем обработку

		hex_code = html_encoded.substr(found + 3, end - found - 3);  // Extract hex value
		str_strm << hex_code;  // hex to stream buffer
		str_strm >> unicode_char;  // From hex to char

		decoded_str += static_cast<wchar_t>(unicode_char);  // Add to std::wstring decoded value
		bgn = end + 1;  // offset to next value
		hex_code.clear();
		str_strm.clear();
	}
	decoded_str.append(html_encoded.begin() + bgn, html_encoded.end() );  // Копируем остаток строки
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Replace_S()
{
	std::string string_need_replace("0123 I think that this is not good idea");

	string_need_replace
		.replace(0, 5, "")  // delete 0 - 5 index | from 0 to 5, change to ""
		.replace(string_need_replace.find("that"), 4, "only")  // Заменяем "that" на "only" 
		.replace(string_need_replace.find("this"), 4, ""); // Убираем "this"

	string_need_replace += "Well Done";
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Connect_Server()
{
	//unsigned long long *send_to_serv = 0;  // Get from User Input
	const int length = 3;
	unsigned long long send_to_serv[length] {};
	unsigned long long *send_to_serv_v = 0;

	//send_to_serv[0] = 0ULL;
	send_to_serv[0] = AsConfig::ULL_Max;
	send_to_serv[1] = 999999ULL;
	send_to_serv[2] = 888888ULL;

	Client = new AClient(send_to_serv, length);  // temp

	// TASKS
	/*
	V	- Connect to server
	V	- Send to server
	V		- temp arg send ull *
	X		- what if send 0?
				- Who check, clien or server? think server will be good

	*/

	delete Client;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Map_Pair_Ptr()
{
	struct S_Test { int a; int b; } S_Example = {};

	std::map<std::wstring *, S_Test *> *Base_Array = 0;
	std::pair<std::wstring *, S_Test *> pair_exmpl = {};

	// 1.0. Create pointer and pair
	Base_Array = new std::map<std::wstring *, S_Test *> {};
	pair_exmpl.first = new std::wstring(L"Hello");
	pair_exmpl.second = new S_Test {};

	// 1.1. Add to Base_Array
	Base_Array->emplace(pair_exmpl);
	
	// 1.2. How to erase from array and delete all data || Free allocated memmory
	for (std::map<std::wstring *, S_Test*>::iterator it = Base_Array->begin(); it != Base_Array->end(); )
	{
		delete it->second;
		delete it->first;

		it = Base_Array->erase(it); // Удаляем элемент и перемещаем итератор на следующий
	}
	delete Base_Array;
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_FFmpeg_Commands()
{//  ffmpeg -i inFile.mp3 outfile.mp3

	// Command Line Examples ffmpeg
	/*
	
	- QSP  // Only to convert scale
		- ffmpeg -hwaccel qsv -hwaccel_output_format qsv -i 0.mp4 -vf "format=nv12,scale=1920:1080:flags=lanczos" -c:v h264_qsv -preset veryslow -global_quality 21 output.mp4
		- ffmpeg -hwaccel qsv -hwaccel_output_format qsv -i 0.mp4 -c:v h264_qsv -preset veryslow -global_quality 21 output.mp4
	
	- Temp Experements
		- ffmpeg -i input.mp4 -vf "unsharp=5:5:1.0" -c:v libx264 -preset slow -crf 0 output.mp4
		- ffmpeg -i input.mp4 -vf "scale=1920:1080:flags=lanczos,minterpolate='mi_mode=mci:mc_mode=aobmc:vsbmc=1:fps=60',unsharp=3:3:0.8" -c:v libx264 -preset veryslow -crf 21 output.mp4

	- Screen_Shots
		- ffmpeg -i improved_output.mp4 -ss 00:01 -frames:v 1 -q:v 1 -vf "scale=1920:1080" screen_00.png  // Get Screen
	
	- Video:
			- Cut Video:  //  // Cut video from ss to...
				- ffmpeg -i input.mp4 -ss 00:01:18 -to 00:02:48 -c copy 0.mp4  // from to
				- ffmpeg -i input.mp4 -t 15 -c copy output.mp4  // Save only first 15 second

				- ffmpeg -ss 00:00:00 -to 00:00:17 -i input.mp4 -c copy outfile.mp4  // Save 17 second 
				- ffmpeg -i inFile.mp4 -ss 00:00:00 -to 00:00:17 out.mp4 -c copy outfile.mp4  //
				- ffmpeg -ss 14 -i inFile.mp4 -t 4 outfile.mp4  //  -ss 14 - skip 14 second | -t time duration 4 seconds
				- ffmpeg -i inFile.mp4 -ss 14 -to 18 outfile.mp4

			- Intorpolate :
		- ffmpeg -i input.mp4 -vf "minterpolate='mi_mode=mci:mc_mode=aobmc:vsbmc=1:fps=60',scale=1920:-1,unsharp=5:5:1.0" -b:v 6000k -c:v libx264 -preset slow -crf 18 output_60fps_smooth_unsharp.mp4
		- ffmpeg -i input.mp4 -vf "minterpolate='mi_mode=mci:mc_mode=aobmc:vsbmc=1:fps=60',unsharp=5:5:1.0" -c:v libx264 -preset slow -crf 18 output.mp4
			
			- Divide : stack two video in one
		- ffmpeg -i 0.mp4 -i output.mp4 -filter_complex " \
			[0:v]crop=in_w:in_h/2:0:in_h/2[v1bottom]; \
			[1:v]crop=in_w:in_h/2:0:in_h/2[v2bottom]; \
			[v1bottom][v2bottom]vstack[vout]" \
			-map "[vout]" -map 0:a -c:v libx264 -crf 16 -preset veryfast output_00_with_audio.mp4

			- Convert :
		- ffmpeg -i input.mp4 -vf "scale=1920:1080, fps=60, unsharp=5:5:1.0" -c:v libx264 -b:v 14866k -profile:v high -preset slow -c:a aac -b:a 128k -ar 44100 output.mp4
		- ffmpeg -i input.mp4 -vf "scale=1920:1080,fps=60" -c:v libx264 -b:v 14866k -profile:v high -preset slow -c:a aac -b:a 128k -ar 44100 output.mp4
		- ffmpeg -i input.mp4 outfile.mp4  // Convert from avi to mp4
		- ffmpeg -i input.mp4 -q 23 quality_23.avi  // or if mp4 use -crf || -q(quality) 23 value quality low better?!?

	- Video Concatenation :
		- ffmpeg -f concat -safe 0 -i file_list.txt -c copy outfile.mp4  // Create .txt and set all files to concatenate ||" file 'Test_00.mp4' "

	- Audio:
		- ffmpeg -i inFile.mp3 -b:a 320k outfile.mp3  // -b:a maybe bitrate for audo can change 320k to 1000k need write k

	- Video / Audio:
		- ffmpeg -i input.avi -b:v 1000k -b:a 128k outfile.mp4  // change byterate video to 1000k and audio to 128k

	- Filters: (-filter || -vf )
		- Volume:
			- ffmpeg -i inFile.mp4 -filter:a "volume=2" outfile.mp4  // -filter:a (use on audio channel) if =2 double volume
		- Mapping
			- ffmpeg -i inFile.mp4 -filter:a "channelmap=0-0|0-1" outfile.mp4  // from left 0 to right 0 and from l 0 to r 1
		- Cropping Video
			- ffmpeg -i inFile.mp4 -filter:v "crop=w=640:h=480:x=100:y=200" outfile.mp4
			- ffmpeg -i inFile.mp4 -filter:v "crop=w=2/3*in_w:h=2/3*in_h" outfile.mp4  // crpo the video to 2/3 of its origin
		- Scalling Video
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=640:h=480" outfile.mp4  // Scale video to
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=2/3*in_w:h=2/3*in_h" outfile.mp4  // Scale video to 2/3
			- ffmpeg -i inFile.mp4 -filter:v "scale=w=852:h=-1" outfile.mp4  // Scale video to
		- Rotation Video
			- ffmpeg -i inFile.mp4 -filter:v "rotate=45*PI/180" outfile.mp4  // Result rotate to 45 degreases
			- ffmpeg -i inFile.mp4  -vf "hflip,transpose=2,transpose=2" outfile.mp4  // Result from left to right
			- ffmpeg -i inFile.mp4  -vf "hflip" outfile.mp4  // Result from left to right
		- Equalization :
			- ffmpeg -i inFile.mp4 -filter "colorbalance=rs=-0.3:bs=0.3:rh=0.1:bh=-0.1" outfile.mp4  // r,g,b and (shadow, midtones, highlights)
			- ffmpeg -i inFile.mp4 -vf "eq=contrast=1.3" outfile.mp4  // 1.3 Darker and more 0.8 and less brighter
			- ffmpeg -i inFile.mp4 -vf "eq=brightness=0.3" outfile.mp4  // 0.3 to mush light or -0.3 too much dark
			- ffmpeg -i inFile.mp4 -vf "eq=saturation=1.5, unsharp" outfile.mp4
			- ffmpeg -i inFile.mp4 -vf "eq=gamma_r=1.5" outfile.mp4  // _r red channel _g green _b blue
		- Filter Audio:
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showwaves=s=1280x720:mode=cline:colors=#00FF00|#FF00FF,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showcqt=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showfreqs=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]avectorscope=s=1280x720,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]avectorscope=s=1280x720:draw=line,format=yuv420p[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4
			- ffmpeg -i inFile.mp4 -filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt,eq=gamma=2.0[v]" -map "[v]" -map 0:a -c:a copy outfile.mp4

		- OBS | STREAM | WRITE DESKTOP
			- ffmpeg -f gdigrab -framerate 30 -i desktop -vcodec libx264 -preset ultrafast -crf 0 -threads 0 -y output.mp4
			- ffmpeg -f gdigrab -framerate 60 -video_size 1440x900 -i desktop -vcodec libx264 -preset medium -crf 0 -threads 0 -y output_1.mp4


	*/
}
//------------------------------------------------------------------------------------------------------------
constexpr int AsExamples::Func()
{
	return 0;
}
//------------------------------------------------------------------------------------------------------------

// Restream || Effects ++ || yt-dlp Examples
/*
	// Download .m3u8 best quality
		- ffmpeg -i "https://edge4-hel.live.mmcdn.com/live-edge/amlst:asianqueen93-sd-06676b964caecc82a68c1ae84c0e1ed0a5e93a39c200a6a11408a34307384192_trns_h264/chunklist_w2008446981_b5128000_t64RlBTOjMwLjA=.m3u8" \
		-c copy asianqueen93.mp4


	// Get youtube url for ffmpeg
		- yt-dlp -f best -g https://www.youtube.com/watch?v=rn4PSf_-J1s
	// Get download video + audio from site useing manifest format .mpd or .m3u8
		- yt-dlp -f bestvideo+bestaudio --merge-output-format mp4 -o "4.mp4" "https://edge35-waw.live.mmcdn.com/live-edge/am"
		- yt-dlp --merge-output-format mp4 -o "4.mp4" "https://edge31-waw.live.mmcdn.com/live-edge/amlst:aurora_radiance-sd-38e5483d956715fbb3a7a1c5616abd535b0b5f496f25ec869cf5b0d0126cbc1c_trns_h264/chunklist_w2049916998_b5128000_t64RlBTOjMwLjA=.m3u8"
		- yt-dlp -f bestvideo+bestaudio --merge-output-format mp4 --write-sub --sub-lang en https://www.youtube.com/watch?v=liftGcnBvKs

	// Re-Streaming
ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/expire/1725287016/ei/CHbVZqPcM8"
 -filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt,eq=gamma=2.0[v]" -map "[v]" -map 0:a
 -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 // Work GOOD
 ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/" \
  -filter_complex "[0:a]showwaves=s=1920x360:mode=cline:colors=cyan|magenta:scale=sqrt,format=rgba,fade=t=in:st=0:d=1,alphaextract[a]; [0:v]scale=1920:1080,pad=iw:ih+360:0:0:black[v]; [a]format=rgba,colorchannelmixer=aa=0.5[overlay]; [v][overlay]overlay=0:main_h-overlay_h" \
  -c:v libx264 \
  -crf 23 -preset fast \
  -c:a aac -b:a 192k \
  -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 // Work
 ffmpeg -i "https://manifest.googlevideo.com/api/manifest/hls_playlist/expire/172" \
-filter_complex "[0:a]showwaves=s=1920x1080:mode=cline:colors=cyan|magenta:scale=sqrt[v];[0:v][v]hstack=inputs=2" \
-c:v libx264 
-crf 23 
-preset fast 
-c:a aac -b:a 192k 
-f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

ffmpeg -i "https://manifest.googlevideo.com/api/manifest/" \
-filter_complex "[0:a]showwaves=s=1920x360:mode=cline:colors=cyan|magenta:scale=sqrt,format=rgba,fade=t=in:st=0:d=1,alphaextract[a]; [0:v]pad=iw:ih+360:0:0:black[v]; [a]format=rgba,geq='r=0:g=0:b=0:a=1.0'[overlay]; [v][overlay]overlay=0.5:main_h-overlay_h" \
-c:v libx264 -crf 23 -preset fast -c:a aac -b:a 192k -f flv "rtmp://a.rtmp.youtube.com/live2/6qr3-2umk-tquf-9kjy-563h"

 ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CqMGIo_mYhKfjtfZHQi4FxidzrVBb1ny2xiBiRCHDtencN6IVyhg5X0cKRqqZnE1ExwbQQZwag7E4kWh6aq_Er8EenBSpVRdfPURRluTl2z35uKdGvz85KxqBKEffD_mdXDxz4pYpj7yjcoOJolUF-5TnBL4AgrHrsN9OHjELhucsZ3WeWXaKVIFpyEP4xmGHjuOuGcoUpilRIkwz1y0VMPk4JqM3RZ5drDK9SKe0vN5of5rgXd-hYPQ4KG9E3r-sKzlw0J_czTU0sdu4i5oJUCcvTZo7c651_HVPTJRxDLPHZITmaNVqbAX0-5OAHGvzOAbE-SbcGbVmhkANxy3tvtJqd_ifGN3XSUphuhAr_I6_cP_F6Cqc9P1VAWJvnIb7djWB7QCLR4wgIG-ZTHoivwZ5Y3t6e-0YoxfnaH0AkR_bFwCa1ZLlBA3x3dQFxn0ZJ9Hi5Sc414DL2HifI8VI-a4phoEcfdvE6ljcK_mxOFL2szgRLjA8drx0PAn5LTHeebyPudAZWeLML6sOzJ4oAVAEkdTUddnzpUvQXmThbd6ctY0GCV1EORpDnJZeUjrz4XDnowftg0JbySgFeLH3wmxRW3CFEE-EacJsLPxgEV1Fw5wuLiXwI_F7PecB1RK0xCqOiUawuWxjQ9Z5UhPYzZFkY2UV4PeXKXXeGS-E0Qy2Cm7na35DD2DQBLjG_PZboPf1Q-eLmm0vCTY3Jg0Zsqozt0ogKeNbvwZ4GqmNmSXg0sqOp-YLhlTQ0Kr37ZRFB0t_EfI9-xehWGOYIF8B8OQa7tFjSnuehN5SZAY_Avi-piygj097ptj8pr-eDLgRMxk4kcNz3RvCcBKe7BkVhyXDTiAlOGwutJspVppPGbBJuraz5jZ1Z24axfluOtrgXgWgcpX0PQZYF5EYQblRFFHKOllQ4Wu8Z9WdJULZl5XFNRbWG59qjK7H4nBVETc6OspM4hKUBt5xyWo54l8W8qpgxwcEy5XRNUJ_so7fHpE9DGmOVQYsF9YNtq-f0jNh_8Jkro8fon4B0BGWQEGe6ijvJewF48Mk8hkl_1jwEybtJnOoR4aDNN6qUJwUaTpLqQaJyABKglldS13ZXN0LTIwrgo.m3u8" \
  -c:v libx264 \
  -crf 23 -preset fast \
  -c:a aac -b:a 192k \
  -f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"

  ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CoQHD1eoI2W4Sq6FuLMojkcorAugp-lPwT2_2cgLogvMndI_hGv0ZgF6sMQmvSuaEfF996AHvjY23KA-yHmBNUuQ4Oes1RkiMUOW_K0Q6uWqnvredafT1o0yBImHG0CcL3PjqSovCtKEHX1IZSaCENHWIVGm-vct5H0AtVYrAbFrcdhRZML_HS9tySnpbrOO-0t5VVx0MJitDlgqOnBBUyCRKTV7ZawzDhlvRR1EgcMgglHTyvwV9ctRNmMHo4s3RH4ksKozhtTbYLlpMzFaEiTKT7wnDJDge-GobImZRw0nXuNg6UuA6uKytm7KJZf_IsuGxoa1C-6Ilor94ZBR8Fud9ynJm-UoLGDJWvkkJzB66HIPAZ5XbR0NTCjlPzpGl_MZW28dA3ZqdrZ-TgiUlRGD7xNu4tIfZ-7XXW-0PMTfs0_arnYhTspOpumR-5LfmyGAmjjChJ9AHwHp1WGBDayauR1QuK7CwU7BTlQb3XGSQLZM1RQ5Kfxm4ZLkACpUrlTlry6YfD17nTSgS4zgb39qV7Ey4I2EM3_GtwwNlQCeh0-mzZT1uLJAZsn68XRr5UoZartJDF6r9Fz-BOyE0IhIzbCg8IMN0mf0gDYVgBbk6H_RuMaix84cEdasnQEyBxCzZuo3Lrz2gILqftdmnpDE6EgkP94yqZPJi0F-C1PWvjp4ckkltess6_gKohX6gpyh7AIYXj1iRq8dQsPWq6lTvGGNjOH8UYW6gOdvyMyjORKx8Kjk6pE2vSMTWcVDdy3dlBuPvh_m2RFh0lbjvqGUdReOZ7uRgUyB33ql2XAVQlYixyMhJwBIotdJ_EtMtWkGOMUHu8kx95DVuI4GPtB3maSrth3IQ7FPuqv_6KNdY1JWizpNVb0-wicmOPXn-zVibf85rBdcN2Z9qgYY_i10h4c478WTRrqpdeICokhKWeMAuyC2xKLANPyOqAzKcy-HXdDbpDXvH0dOsQgOtW7mgIBePOgNverfTNRkSX-xVAUdVD5UJvgLTVOiibMfqTZFa6dKnXo8uvOjscnYtsKIKjEsIxIaRUOZKEgI2fEm4PT04B8jz7qIXSpXjpZucNQ2S4rZ9GCPEl9WQ4kBq-VajLlDCre3hQ94KFVNmyxXeA3kC3DdaZ4f1DrstFC00rJbeQ2nIBaJVUt33oEqcpyksdvaLQuFWM9M7jfsWy41KUfqyXezGgxpfMn8vldCEqmhjbwgASoJZXUtd2VzdC0yMLAK.m3u8" \
  -c:v libx264   -crf 23 -preset fast   -c:a aac -b:a 192k \
  -f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"
  
  // Restream шумоподавление, аншарп, вывести надпись
ffmpeg -i "https://video-weaver.vie02.hls.ttvnw.net/v1/playlist/CpUIEqBUXsqXxPolz7mKcvv1qyOQZjwIxZJOCoQjj4Kr0W5fMcji8ChyQ9VdPrAWrdnC9QmBjKG28XMv6pFGOEewSSGUhbAjOGQ3l6K3ZWryDH5pq_9MZQHepWA08WBNBBozWakYSy4nKJjfryly2H7qjWJ9jTjh9iGhriJiMnVXCKBOzqt5yYTLu2Im3RIATuzL3L5BrA0Wffp2sJ4wuY4nYDjN5PJDY-H1PDClx_0fiZ_7cqLctwbO1Ek9qxdqDtkpltwi2eqVbNhv4KS5fuVfwzhsa-l4PYOxjX5UdbYw5f0EGYyZm3ai4JDEb9FQxCjtPQi8ZCjkNDG2M7dUbZ6P2CbLuRxnJyWpN7K-dbPprQYx_DpiedBU__sMn5whDlzef3lgYL_0FsBOYL6hNVHWo7ChST-eLtHkdxwriuf8KSzh846xiaMa0434HQhanZx76bz4s5S6bBSe4RKnvLIKgnrWw1g5M_ZSM1wbgdGd2dhSrJQv0r5oh-tYZdHr3vOU15wN_4IcRYnVp3jR5G_RNGOz6oghSwn3sttiMtZzzDAwL9i60B56V9lUx_un4I5eOqBRicEPps1bvDpXNCZ0uBG7l6E3GS8SPyCGBlrarosuUSvgPTV6Okemm2kb-9h0GacQe2OY3k9Vh7b4NVGsd6g-8b9SkttmPgiTH8szZvCVkr6ItA4Cy8UQtNPkl1pf_fxYvPepfETEHSOsAAhS_SED9ZzaQC7TNXmS4AwdpKJhqUQsDEqgTTg-end3J__22SvSsrQ47ZMRPrgKkyC7iZZKVGqF17iZI25nueor1s6IexVW8SQ6Ri_XXws6cImpWqN0sa7hzWCRofNZuphNFbJkqlwH64phbvQrNKNZlzGqflYbTwvsBnCXAramO5qcKXUZRLsTs6w2G9bRKY1wvkJdQmU17o4JLGrGZx8Un7AN1qYuU0e-7FPglfkPeb51ZDn26uv9o9VzhAL7IzFZAUKB28GlwhwRY7dDhHgte8YkAcJnGAvTecYNSTUUCF7B-TTrDzfKThErHlWLurSs3kJIVQsaXkj9AGYlMj9jngB--7ezyf4-XRU0-u-ypd5tqJs1l4ydkn2t5T8DGe90kuJG8m1veCXsWrwI4UrIvSVdm8NnzXsm97DuamU3E9U4-7p1P6tPZRvWFWuMN1qgHyUFIDC2wFTA6h4HKH5btJ99b0dN3RI8B4pwxzlDoLN0_B4cyn5ZDJ6i-pbzXN7psZ9vM7B5BcBlzW9UkDRZD3xmFQxemdD_s02vB1oUcNssOGM8QNfgCzRbOXm8U4fg0nWrhMrwUr4tDTxWhgIOfegPu0WltAGEBG6GcL8TnbWXCYDNHLZrqzzI-Um00ihnhK4LBElPj4Wd_5m86MQIof9RsTTT1hoMS39TTbBwzr-2tAyVIAEqCWV1LXdlc3QtMjCuCg.m3u8" \
-vf "hqdn3d,scale=1920x1080,unsharp=5:5:1.0,drawtext=fontfile='C\\:/Windows/Fonts/ahronbd.ttf':text='twitch.tv/AlinaRinRin':x=w-tw-100:y=20:fontsize=24:fontcolor=green,eq=brightness=0:contrast=1" \
-b:v 6800k -c:v libx264 -crf 26 -preset fast -c:a aac -b:a 192k \
-f flv "rtmp://b.rtmp.youtube.com/live2?backup=1/6qr3-2umk-tquf-9kjy-563h"

// Twitch Stream
ffmpeg -f gdigrab -framerate 30 -video_size 1440x900 -i desktop -vf "format=yuv420p" \
-c:v libx264 -preset veryfast -b:v 2500k -maxrate 2500k -bufsize 5000k \
-f flv rtmp://live.twitch.tv/app/live_263586276_aedKDsnRJZ6InTyvvWzLFL1V5Bzc16


*/

// AUDIO
/*
	- atempo -af "asetrate=44100*1.2, atempo=1/1.2"  // Speed
	- asetrate -af "asetrate=44100*1.2, atempo=1/1.2"  // 
	- atempo -af "atempo=1.5"
	- acompressor -af "acompressor=threshold=-20dB:ratio=4:attack=200:release=1000"

	- equalizer -af "equalizer=f=1000:t=q:w=1:g=-10, aecho=0.8:0.88:60:0.4"
*/

// HOTKEYS
/*
	- CTRL+SHIFT+BACKSPACE Back to prev changed in code
	- CTRL + G  // Go to Line
	- ALT + F12 show func defenition and can change it
	- CTRL + - || CTRL + SHIFT + -   // Toggle pages
	- CTRL + K,S  // Surrond with if or elsewhere || + 1
	- TOOLS Windows Managment:
		- CTRL + ALT + O  // Show output
		- CTRL + ALT + L  // Show solution + 1
		- CTRL + ALT + B  // Show Breakpoints
		- CTRL + \,E  // Show Errors
		- SHIFT + ESC  // Close current tool window

	- DEBUG:
		- Shift+F5  // Stop debug
		- Ctrl+F10  // Run to cursor


*/

#pragma region CURL
/*
	// -A user agent can be anything
	- curl -L -A "Dick_Hunter" -O https://animestars.org/aniserials/video/action/2719-moguschestvennye-ucheniki.html
	- curl -L -A "Dick_Hunter" -O https://api.animestars.org/ | jq

*/
#pragma endregion
