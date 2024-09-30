#pragma once

#include "AsConfig.h"

//------------------------------------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------------------------------------
enum class EClient_State : byte
{
	ECS_None,
	ECS_Server_Connect,
	ECS_Server_Buffers,
	ECS_Server_Buffers_Failed,
	ECS_Server_Sending,
	ECS_Failure
};
//------------------------------------------------------------------------------------------------------------




// AActor
class AActor
{
public:
	~AActor();
	AActor();

	void Init();
	
	std::function< void()> Func_Handler;  // less performance then void (AActor:: *Func_Address)();

private:
	void Call_Handler();
	void Call_Me();
	void Call_Me_Adnveced();

	void *Func_Ptr;  // Max Performance
	void (AActor:: *Func_Address)();  // Max Performance
};
//------------------------------------------------------------------------------------------------------------




// AClient
class AClient
{
public:
	AClient(const unsigned long long *data_to_send, int len);

	void Client_Handler();

	void Connect_Server();
	void Send_To_Server();  // PUSH
	void Data_Emplace(const unsigned long long *data_to_send, int len);

private:
	EClient_State Client_State;
	unsigned char *Buffer_To_Server;
	SOCKET Socket_To_Server;
	sockaddr_in Address_Server;

	static unsigned long long Data_Size;
};
//------------------------------------------------------------------------------------------------------------




// AFFmpeg_Task
class AFFmpeg_Task
{
public:
	~AFFmpeg_Task();
	AFFmpeg_Task(char *file_name);

	void Init();

	void Show_Info();

private:
	char *File_Name;

	AVFormatContext *Format_Context_Input;  // data struct stored info about media info | Streams else
	AVFormatContext *Format_Context_Output;
	AVCodecContext *Decoder_Ctx;
	AVCodecContext *Encoder_Ctx;

	AVPacket *Packet;
	AVFrame *Frame;
};
//------------------------------------------------------------------------------------------------------------




// AsExamples
class AsExamples
{
public:
	AsExamples();
	AsExamples(const EShow_Preview show_preview);
	
	void Show_Case(EShow_Preview example_preview);
	void Change_Wormat(wchar_t *video_name);
	void Open_File(char *file_name);
	void Temp(wchar_t *video_name);

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

	void Ffmpeg_Open_File();

	AClient *Client;
};
//------------------------------------------------------------------------------------------------------------
