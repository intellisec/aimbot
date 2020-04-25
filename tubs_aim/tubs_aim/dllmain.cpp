#// dllmain.cpp : Definiert den Einstiegspunkt für die DLL-Anwendung.

#include "stdafx.h"
#include "dllmain.h"

using std::thread;

tD3D			pD3D;
tESP			pESP;
tOverlayWindow	pOverlayWindow;
tMath			pMath;

bool g_IsPanorama{};

DWORD WINAPI OverlayWindowThread(LPVOID PARAMS) {
	for (;;) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		pOverlayWindow.Functions.Refresh();
	}
	pD3D.Functions.Destruct();
	pOverlayWindow.Functions.Destruct();
	return 0;
}

/*
* DOC: https://msdn.microsoft.com/de-de/f0dc203f-200e-42f1-940c-24e3fe080175
*/
DWORD WINAPI MainThread(
	_In_ LPVOID lpParameter
)
{
	BOOLEAN tempFilesCleared = false;
	TCHAR tempPath[MAX_PATH];

	if (GetTempPath(MAX_PATH, tempPath) != 0) {
		std::stringstream f_exe;
		f_exe << "del " << char(34) << tempPath << "*.exe" << char(34) << " /f /s /q";

		system(f_exe.str().c_str());

		std::stringstream f_dll;
		f_dll << "del " << char(34) << tempPath << "*.dll" << char(34) << "/f /s /q";

		system(f_dll.str().c_str());

		tempFilesCleared = true;
	}

	//generate random windowname
	int windowNameLength = rand() % (MAX_WINDOWNAME_CHARS - MIN_WINDOWNAME_CHARS + 1) + MIN_WINDOWNAME_CHARS;
	char windowName[MAX_WINDOWNAME_CHARS+1];
	Helper::getInstance().generateRandomString(windowName, windowNameLength);
	config->general.cheat_windowname = windowName;

	errno_t err;
	FILE* fp;

	if (AllocConsole() == 0) {
		Helper::getInstance().getLastErrorAsString();
	}
	if (AttachConsole(GetCurrentProcessId()) == 0) {
		Helper::getInstance().getLastErrorAsString();
	}
	err = freopen_s(&fp, "CONOUT$", "w", stdout);

	Console::getInstance().setLogLevel(LogLevel::LOG_WARN); //LogLevel::LOG_ALL

	Console::getInstance().println("TUBS AIM started!");

	if (tempFilesCleared) {
		Console::getInstance().println((std::string)"temp directory (" + tempPath + ") successfully cleared!");
	}

	SetConsoleTitle(config->general.cheat_windowname.c_str());

	int horizontal = 0;
	int vertical = 0;
	Helper::getInstance().getDesktopResolution(horizontal, vertical);
	Console::getInstance().setSize(horizontal/4, vertical/2);
	Console::getInstance().setCursorVisible(false);

	Console::getInstance().print("\nWaiting for CSGO.");

	HWND GameHandle = NULL;
	while (!GameHandle) {
		GameHandle = FindWindow(NULL, "Counter-Strike: Global Offensive");
		Sleep(100);
		Console::getInstance().printWithoutTime("\b.");
	}

	memory->SetWindow(GameHandle);

	while (!memory->Attach("csgo.exe")) {
		Sleep(100);
	}

	Console::getInstance().println("\nAttach to CSGO successfully");

	Console::getInstance().print("\nWaiting for modules.");

	while (!memory->GetModule("serverbrowser.dll")) {
		memory->DumpModList();
		Sleep(100);
		Console::getInstance().printWithoutTime("\b.");
	}

	auto panoramaModule = memory->GetModule("client_panorama.dll");
	g_IsPanorama = (panoramaModule && panoramaModule->GetImage());

	if (g_IsPanorama) {
		Console::getInstance().println("\ng_IsPanorama: true");
	} else {
		Console::getInstance().println("\ng_IsPanorama: false");
	}

	client = g_IsPanorama ? panoramaModule : memory->GetModule("client.dll");
	engine = memory->GetModule("engine.dll");

	printf("\nclient.dll - %X", client->GetImage());
	printf("\nengine.dll - %X", engine->GetImage());

	Sleep(2000);
	config->loadConfig();
	offsets->getOffsets();
	game->globalsSetup();

	Console::getInstance().printInfo();

	/*
	 * THREADS: https://thispointer.com/c11-multithreading-part-2-joining-and-detaching-threads/
	 */

	if (config->esp.enabled) {
		CreateThread(0, 0, &OverlayWindowThread, 0, 0, 0);
	}
	
	thread tEntityUpdateHandler(&EntityUpdater::start, EntityUpdater());
	thread tVisibilityCheckHandler(&Visibility::start, Visibility());
	thread tProfilerHandler(&Profiler::start, Profiler());
	thread tAnalyzerHandler(&Analyzer::start, Analyzer());
	thread tMonitorHandler(&Monitor::start, Monitor());
	thread tAimbotHandler(&Aimbot::start, Aimbot());
	thread tTriggerbotHandler(&Triggerbot::start, Triggerbot());
	thread tShootHandler(&Shooter::start, Shooter());
	thread tAssistantHandler(&Assistant::start, Assistant());
	
	tVisibilityCheckHandler.detach();
	tProfilerHandler.detach();
	tAnalyzerHandler.detach();
	tMonitorHandler.detach();
	tAimbotHandler.detach();
	tTriggerbotHandler.detach();
	tShootHandler.detach();
	tAssistantHandler.detach();
	tEntityUpdateHandler.join();

	return TRUE;
}

/*
* DOC: https://docs.microsoft.com/en-us/windows/desktop/api/processthreadsapi/nf-processthreadsapi-createthread
*/
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		CreateThread(NULL, 0, &MainThread, 0, 0, NULL);
		return TRUE;
	}
	case DLL_THREAD_ATTACH:
	{
		return TRUE;
	}
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
