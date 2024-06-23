#pragma once

#include "framework.h"

//------------------------------------------------------------------------------------------------------------
class AsTools
{
public:
	~AsTools();
	AsTools();

	static bool Is_Button_Pressed;

	static void Format_Text_Using_Patterns(const wchar_t *url, const wchar_t *pattern_str, const wchar_t *pattern_end, wchar_t **result);
	template <typename Template> static void TObject_Save(const Template &data, const char *path);
	template <typename Template> static void TObject_Load(Template *&data, const char *path);


private:
	long long start_tick, end_tick, delta_tick;
};
//------------------------------------------------------------------------------------------------------------





//------------------------------------------------------------------------------------------------------------
template<typename Template> inline void AsTools::TObject_Save(const Template &data, const char *path)
{//  !!! AsTools --- INLINE || TArray<unsigned short> temp;  Create TArray
	std::ofstream outfile(path, std::ios::binary);
	if (!outfile)
		return;

	outfile.write(reinterpret_cast<const char*>(&data), sizeof(Template) );
	outfile.close();
}
//------------------------------------------------------------------------------------------------------------
template<typename Template> inline void AsTools::TObject_Load(Template *&data, const char *path)
{
	int how_much_g;
	int block_sum;

	std::ifstream infile(path, std::ios::binary);  // in file, read
	if (!infile)
		return;

	infile.seekg(0, std::ios::end);  // Вычисляем количество чисел в файле тем самим переходя в конец файла
	how_much_g = (int)infile.tellg();
	block_sum = how_much_g / sizeof(Template);  // (long long) 8 / size = how manny in unsigned long long data

	infile.seekg(0, std::ios::beg);  // Переходим в начало файла
	data = new Template[block_sum] {};  // Выделяем память для чтения чисел из файла
	infile.read(reinterpret_cast<char *>(data), how_much_g);  // Читаем и записиваем числа из файла в массив! как ull

	infile.close();
	return;
}
//------------------------------------------------------------------------------------------------------------
