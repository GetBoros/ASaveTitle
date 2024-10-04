#pragma once
#pragma comment(lib, "dxgi.lib")

#define UNICODE
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// My Includes
extern "C"
{// FFMPEG
#include <sys/stat.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
}

#include <fstream>  // SAVE || LOAD
#include <map>  // STL || 
#include <filesystem>  // FOLDERS || CREATE
#include <thread>  // THREADS
#include <array>
#include <cwctype>  // For std::towupper and std::towlower
#include <mutex>
#include <bitset>  // std::bitset || std::byte
#include "WinSock2.h"  // NET
#include "curl/curl.h"  // NET || BROWSE
#include "resource.h"
#include "d3d12.h"  // DirectX || IMAGE || Must be first
#include "DirectXTex.h"  // DirectX || IMAGE :

// TEMP
/*
//#include <cwchar>  // wcstombs_s
//#include <WS2tcpip.h>
//#include "strsafe.h"
//#include <windowsx.h>
//#include <cstring>
//#include <dxgi1_6.h>
//#include <iostream>
//#include <d3d12.h>
//#include <d3dcompiler.h>
//#include <d3d12.h>

//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "d3d12.lib")
*/
