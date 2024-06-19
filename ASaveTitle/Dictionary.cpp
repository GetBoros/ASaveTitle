#include "Dictionary.h"


// SWord
SWord::SWord(unsigned long long* word_begin, size_t word_end)
	: Word_Begin(word_begin), Word_End(word_end)
{
}
//------------------------------------------------------------------------------------------------------------




// AsDictionary
AsDictionary::~AsDictionary()
{
	// TEST
	/*
	int qwert;
	unsigned long long check;

	for (auto& it : Dictionary_Map)
	{
		qwert = 0;
		while (it.second.Word_End > qwert)
			check = *(it.second.Word_Begin + qwert++);
	}
	*/

	Save();

	for (auto& it : Dictionary_Map)
	{
		delete[] it.second.Word_Begin;
		return;
	}

}
//------------------------------------------------------------------------------------------------------------
AsDictionary::AsDictionary()
{
	Load();
}
//------------------------------------------------------------------------------------------------------------
void AsDictionary::Emplace_Word(char* words)
{
	int ptr = 0, str = 0, counter_write = 0;
	size_t word_last_char;
	unsigned long long* memmory_block;
	unsigned long long number = 0;

	const size_t len = strlen(words);
	const size_t block_sum = (len + sizeof(unsigned long long) - 1) / sizeof(unsigned long long);  // (long long) 8 / size = how manny in unsigned long long data

	word_last_char = 0;
	memmory_block = new unsigned long long[block_sum] {};  // Выделяем память для чтения чисел из файла


	while (words[str] != '\0')  // Пока не достигнем конца обрабативаем строку
	{
		char ch;
		int ch_int;

		ch = words[str];
		ch_int = static_cast<int>(ch);  // 1105

		if (counter_write % 9 == 0 && counter_write != 0)  // Заходим каждый 9 раз
		{
			memmory_block[ptr] = number;
			number = 0;
			ptr++;
		}

		if (number == 0)
			number = ch_int;
		else
			number = number * 100 + ch_int;  // 100 - смещение на 2 числа

		counter_write++;
		str++;
	}

	number += AsConfig::ULL_Length;
	if (ptr < block_sum)
		memmory_block[ptr] = number;

	for (size_t i = 0; i < block_sum; i++)
	{
		word_last_char++;

		if (memmory_block[i] / AsConfig::ULL_Length)  // if true last word add to array or struct
		{
			i = i - word_last_char + 1;  // First word index
			Dictionary_Map.emplace(memmory_block[i], SWord(&memmory_block[i], word_last_char));  // Create Struct SWord and add to map
			i = word_last_char;  // start from end
		}
	}
}
//------------------------------------------------------------------------------------------------------------
void AsDictionary::Save()
{
	size_t ptr;

	std::ofstream outfile("Map.bin", std::ios::binary);
	if (!outfile)
		return;

	for (const auto& it : Dictionary_Map)
	{
		ptr = 0;
		do
		{
			const unsigned long long number = it.second.Word_Begin[ptr];
			outfile.write(reinterpret_cast<const char*>(&number), sizeof(number));
		} while (!(it.second.Word_Begin[ptr++] / AsConfig::ULL_Length));  // INCREMENT HERE
	}

	outfile.close();
}
//------------------------------------------------------------------------------------------------------------
void AsDictionary::Load()
{
	int how_much_g;
	int block_sum;
	unsigned long long* memmory_block;
	size_t word_last_char;

	std::ifstream infile("Map.bin", std::ios::binary);
	if (!infile)
		return;

	infile.seekg(0, std::ios::end);  // Вычисляем количество чисел в файле тем самим переходя в конец файла
	how_much_g = (int)infile.tellg();
	block_sum = how_much_g / sizeof(unsigned long long);  // (long long) 8 / size = how manny in unsigned long long data

	infile.seekg(0, std::ios::beg);  // Переходим в начало файла
	memmory_block = new unsigned long long[block_sum] {};  // Выделяем память для чтения чисел из файла
	infile.read(reinterpret_cast<char*>(memmory_block), how_much_g);  // Читаем и записиваем числа из файла в массив! как ull

	word_last_char = 0;

	for (size_t i = 0; i < block_sum; i++)
	{
		word_last_char++;

		if (memmory_block[i] / AsConfig::ULL_Length)  // if true last word add to array or struct
		{
			const size_t index = i + 1 - word_last_char;
			Dictionary_Map.emplace(memmory_block[index], SWord(&memmory_block[index], word_last_char) );  // Create Struct SWord and add to map
			word_last_char = 0;
		}
	}
}
//------------------------------------------------------------------------------------------------------------
