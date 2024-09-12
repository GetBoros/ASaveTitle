#pragma once

#include "AsConfig.h"

enum class EShow_Preview : int
{
	EP_None,
	EP_Show_Path_Info,  // File Path example
	EP_Show_Byte_Info,  // Convert int to bytes, bits
	EP_Show_Void_Ptrs,  // Example how to use void * (void *)
	EP_Show_Func_Ptrs,  // how to get func ptr and handle it
	EP_Show_Func_Ptrs_Advenced,  // how to get func ptr and handle it advenced
	EP_Show_Cast_Exam,  // Reinterpret cast examples and other useful thinks
	EP_Show_Html_Decode,
	EP_Show_String_Replace,  // Example how to handle string, replace, find texts
	EP_Show_Conect_To_Servers,
	EP_Show_Std_Map_Pair_Ptrs,  // Example how tu use ptr Maps and pairs
	EP_Show_FFmpeg_Ex,  // Example, commands and how to use ffmpeg
	EP_Show_FFMpeg_Example,
	EP_Show_Last
};

// AsExamples
class AsExamples
{
public:
	AsExamples();
	AsExamples(const EShow_Preview show_preview);
	
	void Show_Case(EShow_Preview example_preview);

private:
	void Display_Path_Info(std::filesystem::path &path);  // File System Examples
	void Display_Byte_Info();
	void Display_Void_Ptrs();
	void Display_Func_Ptrs();
	void Display_Func_Ptrs_Advenced();
	void Display_Cast_Exam();
	void Display_Html_Decoder();
	void Display_Replace_S();  // Example how replace std::string
	void Display_Connect_Server();
	void Display_Map_Pair_Ptr();
	void Display_FFmpeg_Commands();
	void Display_FFmpeg_Examples();
};
//------------------------------------------------------------------------------------------------------------
