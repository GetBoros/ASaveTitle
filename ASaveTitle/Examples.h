#pragma once

#include "AsTools.h"

enum class EExample_Preview : int
{
	EP_None,
	EP_Show_Path_Info,
	EP_Show_Void_Ptrs,  // Example how to use void * (void *)
	EP_Show_Func_Ptrs,
	EP_Show_Cast_Exam
};

// AsExamples
class AsExamples
{
public:
	AsExamples();
	
	void Show_Case(EExample_Preview example_preview);

private:
	void Display_Path_Info(std::filesystem::path &path, std::wstring &result);  // File System Examples
	void Display_Void_Ptrs();
	void Display_Func_Ptrs();
	void Display_Cast_Exam();

};
//------------------------------------------------------------------------------------------------------------
