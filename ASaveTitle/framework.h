#pragma once
#pragma comment(lib, "dxgi.lib")

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// My Include
#include "resource.h"
#include "fstream"  // SAVE || LOAD
#include "map"  // STL || 
#include "filesystem"  // FOLDERS || CREATE
#include "d3d12.h"  // DirectX || IMAGE || Must be first
#include "DirectXTex.h"  // DirectX || IMAGE :
#include "thread"  // THREADS
#include "chrono"  // THREADS || TIME SLEEP
#include "WinSock2.h"  // NET
#include "curl/curl.h"  // NET || BROWSE

// TEMP
/*
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
