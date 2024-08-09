#pragma once

#include "Examples.h"

// SWord
struct SWord
{
	SWord(unsigned long long* word_begin, size_t word_end);

	size_t Word_End;  // how much ull
	unsigned long long* Word_Begin;  // Ptr at start word
};
//------------------------------------------------------------------------------------------------------------




// AsDictionary
class AsDictionary  // byte 48 std
{
public:
	~AsDictionary();
	AsDictionary();

	void Emplace_Word(char* words);  // Add to Dictionary_Map

	void Save();
	void Load();  // Load From File to Dictionary_Map

private:
	AsTools Tools;
	std::map<unsigned long long, SWord> Dictionary_Map;
};
//------------------------------------------------------------------------------------------------------------
