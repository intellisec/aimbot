#include "stdafx.h"
#include "dllmain.h"

/**
	Get the horizontal and vertical screen sizes in pixel.
	SOURCE: https://stackoverflow.com/questions/8690619/how-to-get-screen-resolution-in-c

	@param horizontal the pointer to the horizontal value of the desktop resolution
	@param vertical the pointer to the vertical value of the desktop resolution
	@return nothing
*/
void Helper::getDesktopResolution(int& horizontal, int& vertical) {
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

/**
	Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	SOURCE: https://stackoverflow.com/questions/3121825/error-c2039-string-is-not-a-member-of-std-header-file-problem

	@return the last error as a string
*/
std::string Helper::getLastErrorAsString() {
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

 /**
	 Converts a virtualkey code to a string.
	 SOURCE: https://stackoverflow.com/questions/38100667/windows-virtual-key-codes

	 @param virtualKey the virtualkey code
	 @return the name of the key as a string
*/
std::string Helper::virtualKeyCodeToString(UCHAR virtualKey) {
	UINT scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	CHAR szName[128];
	int result = 0;
	switch (virtualKey)
	{
		case VK_LBUTTON: return std::string("Left Button");
		case VK_RBUTTON: return std::string("Right Button");
		case VK_MBUTTON: return std::string("Middle Button");
		case VK_XBUTTON1: return std::string("X Button 1");
		case VK_XBUTTON2: return std::string("X Button 2");
		case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN:
		case VK_RCONTROL: case VK_RMENU:
		case VK_LWIN: case VK_RWIN: case VK_APPS:
		case VK_PRIOR: case VK_NEXT:
		case VK_END: case VK_HOME:
		case VK_INSERT: case VK_DELETE:
		case VK_DIVIDE:
		case VK_NUMLOCK:
			scanCode |= KF_EXTENDED;
		default:
			result = GetKeyNameTextA(scanCode << 16, szName, 128);
	}
	if (result == 0)
		return szName;
		//throw std::system_error(std::error_code(GetLastError(), std::system_category()), "WinAPI Error occured.");
	return szName;
}

/**
	Checks if the csgo process is running.

	@return true if csgo process is running, otherwise false
*/
bool Helper::doesCSGOExist() {
	HWND hwnd = FindWindow(NULL, "Counter-Strike: Global Offensive");
	return (hwnd != NULL);
}

/**
	Checks if the csgo window is in the foreground.

	@return true if csgo window is in the foreground, otherwise false
*/
bool Helper::checkCSGOWindowState() {
	char windowTitle[256];
	HWND hwnd = GetForegroundWindow();
	GetWindowText(hwnd, windowTitle, sizeof(windowTitle));

	csgoWindowIsActive = !strcmp(windowTitle, "Counter-Strike: Global Offensive");
	return csgoWindowIsActive;
}

/**
	Checks if the csgo window is in the foreground. Based on cached value.

	@return true if csgo window is in the foreground, otherwise false
*/
bool Helper::isCSGOWindowActive() {
	return csgoWindowIsActive;
}

/**
	Generates a random string of given length.
	SOURCE: https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c

	@param outputString the reference to the output string
	@param length the length of the string to generate
	@return true if csgo window is in the foreground, otherwise false
*/
void Helper::generateRandomString(char *outputString, const int length)  {
	static const char possibleCharacters[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < length; ++i) {
		outputString[i] = possibleCharacters[rand() % (sizeof(possibleCharacters) - 1)];
	}

	outputString[length] = 0;
}
