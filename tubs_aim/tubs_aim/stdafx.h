// stdafx.h: Includedatei für Include-Standardsystemdateien
// oder häufig verwendete projektspezifische Includedateien,
// die nur in unregelmäßigen Abständen geändert werden.
//

#pragma once

#include "targetver.h"

#define NOMINMAX						// USE NOMINMAX: https://stackoverflow.com/questions/7035023/stdmax-expected-an-identifier
#define WIN32_LEAN_AND_MEAN             // Selten verwendete Komponenten aus Windows-Headern ausschließen
// Windows-Headerdateien
#include <windows.h>

// Hier auf zusätzliche Header verweisen, die das Programm benötigt.
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <psapi.h>
#include <xmmintrin.h>
#include <thread>
#include <tlhelp32.h>
#include <shlobj.h>
#include <random>
#include <math.h>

#include "Include/DirectX/d3d9.h"
#include "Include/DirectX/d3dx9.h"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
/*
#ifdef _WIN32
	#pragma comment(lib, "Lib/x86/d3d9.lib")
	#pragma comment(lib, "Lib/x86/d3dx9.lib")
#elif defined _WIN64
	#pragma comment(lib, "Lib/x64/d3d9.lib")
	#pragma comment(lib, "Lib/x64/d3dx9.lib")
#endif
*/
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include "dllmain.h"