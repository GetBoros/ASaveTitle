#include "Examples.h"

// AsExamples
AsExamples::AsExamples()
{

}
//------------------------------------------------------------------------------------------------------------
AsExamples::AsExamples(const EShow_Preview show_preview)
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
	case EShow_Preview::EP_Show_Cast_Exam:
		Display_Cast_Exam();
		break;

	case EShow_Preview::STD_Map_Pair_Ptrs:
		Display_Map_Pair_Ptr();
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
	void (AsExamples:: *ptr_func_update)(EShow_Preview) = 0;  // ���(void) (���, ��������� �� �������-���� ������)(���������) = ������� ���������
	void *ptr_func = 0;  // ���, ��� ��������� ������ ���� void * (������������� ���������) ����� ��������� �� ����� ��� ������
	uintptr_t func_addr = 0;  // ���, ��� ������� ��� �������� �������������� ������������� ������ ���������
	void ( *restored_func_ptr)(EShow_Preview) = 0;  // ���������� ��������� �� ������� ������� ��������� �������� ���� EShow_Preview � ���������� void
	
	// �������������� ��������� �� �������
	ptr_func_update = &AsExamples::Show_Case;  // ���������� ��������� ptr_func_update ������ �������-����� Show_Case ������ AsExamples.
	ptr_func = reinterpret_cast<void *&>(ptr_func_update);  // ���������� ���������, �� �������-���� ������ � ���� void *.
	func_addr = reinterpret_cast<uintptr_t>(ptr_func);  // �������������� void * � uintptr_t ������� �������� ����� ������� ����� ��������, ��������� ��� ��� ������������ �������������

	// �������������� ��������� �� ������� � ����� �������
	restored_func_ptr = reinterpret_cast<void (*) (EShow_Preview)> (func_addr);  // �������� func_addr � void * c arg. ����������, ����� ��������� ������� ������� ����� ���������.
	restored_func_ptr(EShow_Preview::EP_None);  //  ����� ������� ����� ��������������� ��������� � �������� ��������� ���� EShow_Preview.
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
void AsExamples::Display_Replace_S()
{
	std::string string_need_replace("0123 I think that this is not good idea");

	string_need_replace
		.replace(0, 5, "")  // delete 0 - 5 index | from 0 to 5, change to ""
		.replace(string_need_replace.find("that"), 4, "only")  // �������� "that" �� "only" 
		.replace(string_need_replace.find("this"), 4, ""); // ������� "this"

	string_need_replace += "Well Done";
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

		it = Base_Array->erase(it); // ������� ������� � ���������� �������� �� ���������
	}
	delete Base_Array;
}
//------------------------------------------------------------------------------------------------------------
