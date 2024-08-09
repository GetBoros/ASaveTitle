#pragma once

#include "AsTools.h"

enum class EExample_Preview : int
{
	EP_None,
	EP_Show_Path_Info,  // File Path example
	EP_Show_Byte_Info,  // Convert int to bytes, bits
	EP_Show_Void_Ptrs,  // Example how to use void * (void *)
	EP_Show_Func_Ptrs,  // how to get func ptr and handle it
	EP_Show_Func_Ptrs_Advenced,  // how to get func ptr and handle it advenced
	EP_Show_Cast_Exam,  // Reinterpret cast examples and other useful thinks
	EP_Show_String_Replace  // Example how to handle string, replace, find texts
};

// AsExamples
class AsExamples
{
public:
	AsExamples();
	
	void Show_Case(EExample_Preview example_preview);

private:
	void Display_Path_Info(std::filesystem::path &path, std::wstring &result);  // File System Examples
	void Display_Byte_Info();
	void Display_Void_Ptrs();
	void Display_Func_Ptrs();
	void Display_Func_Ptrs_Advenced();
	void Display_Cast_Exam();
	void Display_Replace_S();  // Example how replace std::string
};
//------------------------------------------------------------------------------------------------------------
