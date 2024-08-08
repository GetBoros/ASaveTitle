#include "Examples.h"

// AsExamples
AsExamples::AsExamples()
{
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Show_Case(EExample_Preview example_preview)
{
	// 1.0 EP_Show_Path_Info Data
	std::wstring temp;
	std::filesystem::path pathToDisplay(L"C:/FileSystemTest/SubDir3/SubDirLevel2/File2.txt ");

	// 2.0. EP_Show_Func_Ptrs Data

	switch (example_preview)
	{
	case EExample_Preview::EP_None:
		break;
	case EExample_Preview::EP_Show_Path_Info:
		Display_Path_Info(pathToDisplay, temp);
		break;
	case EExample_Preview::EP_Show_Void_Ptrs:
		Display_Void_Ptrs();
		break;
	case EExample_Preview::EP_Show_Func_Ptrs:
		Display_Func_Ptrs();
		break;
	case EExample_Preview::EP_Show_Cast_Exam
		:Display_Cast_Exam();
		break;

	default:
		break;
	}
}
//------------------------------------------------------------------------------------------------------------
void AsExamples::Display_Path_Info(std::filesystem::path &path, std::wstring &result)
{
	int i;
	std::wostringstream w_string_stream;

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

	result = w_string_stream.str();
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

	void(AsExamples:: * ptr_func_arg_0)() = 0;  // can get ptr to func with zero arguments and type void
	void (AsExamples:: * ptr_func_arg_1) (EExample_Preview) = 0;  // void - return type || ( *ptr_func_arg_1)  - name ||  (int) - func args || = 0;  = &Say_Hello; - adress func with arg int||
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
void AsExamples::Display_Cast_Exam()
{
	byte a = 5;
	char b = '6';
	char *c = reinterpret_cast<char *>(&a);
	int yy = (int)*c;
	c = reinterpret_cast<char *>(&b);
	yy = (int)*c;
	//int *c = reinterpret_cast<int *>(&a);
}
//------------------------------------------------------------------------------------------------------------
