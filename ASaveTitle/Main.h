#pragma once

#include "Engine.h"

// EPrograms
enum class EPrograms
{
	Invalid = -1,
	ASaver = 0,
	ABook_Reader,
	End
};
//------------------------------------------------------------------------------------------------------------




// SWord
struct SWord
{
	SWord(unsigned long long *word_begin, size_t word_end);

	size_t Word_End;  // how much ull
	unsigned long long *Word_Begin;  // Ptr at start word
};
//------------------------------------------------------------------------------------------------------------




// AsDictionary
class AsDictionary
{
public:
	~AsDictionary();
	AsDictionary();

	void Emplace_Word(char *words);  // Add to Dictionary_Map

	void Save();
	void Load();  // Load From File to Dictionary_Map

private:
	AsTools Tools;
	std::map<unsigned long long, SWord> Dictionary_Map;
};
//------------------------------------------------------------------------------------------------------------

// AsMain
class AsMain
{
	AsMain(HINSTANCE handle_instance);

public:
	AsMain(AsMain &main_window) = delete;
	void operator=(const AsMain&) = delete;
	
	int Get_WParam();

	static AsMain *Get_Instance(HINSTANCE handle_instance);

	static AsMain *Main_Window;

private:

	bool Init_Instance();
	ATOM Register_Class();
	
	// TEMP
	RECT *Rect_Programs;

	static WCHAR SZ_Title[AsConfig::Max_Loadstring];
	static WCHAR SZ_Window[AsConfig::Max_Loadstring];
	AsEngine Engine;
	HINSTANCE HInstance;

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static bool Is_Hwnd_Created;
	static EPrograms Programs;
};
//------------------------------------------------------------------------------------------------------------
