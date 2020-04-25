#include "stdafx.h"
#include "dllmain.h"

using std::string;
using std::cout;
using std::endl;
using std::cin;

/**
	Displays the current values and informations of the program.

	@return nothing.
*/
void Console::printInfo() {
	clear();

	println("TUBS AIMBOT v1.0", red, black);
	println("\b");

	println("WARNING:", aqua, black);
	println("For features to work, type the following command in the game console:", aqua, black);
	println("unbind mouse1;bind / +attack", aqua, black);
	println("\b");

	println("Important NOTE:", aqua, black);
	println("If you are suspected, then use the PANIC MODE. All hacks are deactivated immediately.", aqua, black);
	println("\b");

	println("          GENERAL KEYS", magenta, black);
	println("           Reload Config   - " + Helper::getInstance().virtualKeyCodeToString(config->keys.reload_config), green, black);
	println("           Panic Mode      - " + Helper::getInstance().virtualKeyCodeToString(config->keys.panic_mode), green, black);
	println("           Start Analyzer  - " + Helper::getInstance().virtualKeyCodeToString(config->keys.start_analyzer), green, black);
	println("\b");

	println("          HOLD KEYS", magenta, black);
	println("           Aimbot          - " + Helper::getInstance().virtualKeyCodeToString(config->keys.aimbot_hold), white, black);
	println("           Triggerbot      - " + Helper::getInstance().virtualKeyCodeToString(config->keys.triggerbot_hold), white, black);
	println("\b");

	println("          TOGGLE KEYS", magenta, black);
	if (config->aimbot.faceit_aim) {
		print("           Aimbot[Faceit]  - ");
		printlnWithoutTime(Helper::getInstance().virtualKeyCodeToString(config->keys.aimbot_toggle), white, ((config->general.aimbot_enabled) ? green : red));
	} else {
		print("           Aimbot[MEM]     - ");
		printlnWithoutTime(Helper::getInstance().virtualKeyCodeToString(config->keys.aimbot_toggle), white, ((config->general.aimbot_enabled) ? green : red));
	}
	print("           Triggerbot      - ");
	printlnWithoutTime(Helper::getInstance().virtualKeyCodeToString(config->keys.triggerbot_toggle), white, ((config->general.triggerbot_enabled) ? green : red));
	print("           Profiler        - ");
	printlnWithoutTime(Helper::getInstance().virtualKeyCodeToString(config->keys.profiler_toggle), white, ((config->general.profiler_enabled) ? green : red));
	println("\b");

	println("READY TO PLAY!", aqua, black);
	println("\b");
}

/**
	Writes a given string with the current time to the console.

	@param str the string to write
	@param textColor the color of the text
	@param bgColor the color of the background
	@return nothing
*/
void Console::print(std::string str, ConsoleColor textColor, ConsoleColor bgColor) {
	setColor(textColor, bgColor);
	cout << getTimeString() << str;
	setColor(white, black);
}

/**
	Writes a given string with the current time to the console and makes a break.

	@param str the string to write
	@param textColor the color of the text
	@param bgColor the color of the background
	@return nothing
*/
void Console::println(std::string str, ConsoleColor textColor, ConsoleColor bgColor) {
	setColor(textColor, bgColor);
	cout << getTimeString() << str << "\n";
	setColor(white, black);
}

/**
	Writes a given string without the current time to the console.

	@param str the string to write
	@param textColor the color of the text
	@param bgColor the color of the background
	@return nothing
*/
void Console::printWithoutTime(std::string str, ConsoleColor textColor, ConsoleColor bgColor) {
	setColor(textColor, bgColor);
	cout << str;
	setColor(white, black);
}

/**
	Writes a given string without the current time to the console and makes a break.

	@param str the string to write
	@param textColor the color of the text
	@param bgColor the color of the background
	@return nothing
*/
void Console::printlnWithoutTime(std::string str, ConsoleColor textColor, ConsoleColor bgColor) {
	setColor(textColor, bgColor);
	cout << str << "\n";
	setColor(white, black);
}

/**
	Writes a given string with the current time to the console.

	@param str the string to write
	@param level the log level to compare
	@return nothing
*/
void Console::log(LogLevel level, std::string str) {
	if (logLevel > level)
		return;

	appendToLogFile(str);
	setColor(static_cast<ConsoleColor>(logLevelColors[level]), black);
	cout << getTimeString() << str;
	setColor(white, black);
}

/**
	Writes a given string with the current time to the console and makes a break.

	@param str the string to write
	@param level the log level to compare
	@return nothing
*/
void Console::logln(LogLevel level, std::string str) {
	if (logLevel > level)
		return;
	
	appendToLogFile(str);
	setColor(static_cast<ConsoleColor>(logLevelColors[level]), black);
	cout << getTimeString() << str << "\n";
	setColor(white, black);
}

/**
	Writes a given string without the current time to the console.

	@param str the string to write
	@param level the log level to compare
	@return nothing
*/
void Console::logWithoutTime(LogLevel level, std::string str) {
	if (logLevel > level)
		return;

	appendToLogFile(str);
	setColor(static_cast<ConsoleColor>(logLevelColors[level]), black);
	cout << str;
	setColor(white, black);
}

/**
	Writes a given string without the current time to the console and makes a break.

	@param str the string to write
	@param level the log level to compare
	@return nothing
*/
void Console::loglnWithoutTime(LogLevel level, std::string str) {
	if (logLevel > level)
		return;

	appendToLogFile(str);
	setColor(static_cast<ConsoleColor>(logLevelColors[level]), black);
	cout << str << "\n";
	setColor(white, black);
}

/**
	Creates a formatted string with the current time.

	@return the formatted string with the current time
*/
std::string Console::getTimeString() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 80, "[%H:%M:%S]", &timeinfo);
	return buffer;
}

/**
	Sets the size of the console.

	@param width the width the console should be set to
	@param height the height the console should be set to
	@return nothing
*/
void Console::setSize(int width, int height) {
	HWND hConsole = GetConsoleWindow();	RECT r;
	GetWindowRect(hConsole, &r);
	MoveWindow(hConsole, r.left, r.top, width, height, TRUE);
}

/**
	Sets the color of the console.

	@param textColor the color of the text
	@param bgColor the color of the background
	@return nothing
*/
void Console::setColor(ConsoleColor textColor, ConsoleColor bgColor) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (textColor + (bgColor * 16)));
}

/**
	Sets the cursor visible/invisible.

	@param visible whether the cursor should be visible or not
	@return nothing
*/
void Console::setCursorVisible(bool visible) {
	CONSOLE_CURSOR_INFO curCursorInfo;
	curCursorInfo.bVisible = visible;
	curCursorInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curCursorInfo);
}

/**
	Sets the position of the Cursor in the console.

	@param x the x position the cursor should be set to
	@param y the y position the cursor should be set to
	@return nothing
*/
void Console::setCursorPosition(int x, int y) {
	COORD xy;
	xy.X = x;
	xy.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), xy);
}

/**
	Clears the console. (without system() call)
	SOURCE: https://stackoverflow.com/questions/6486289/how-can-i-clear-console

	@return nothing
*/
void Console::clear() {
	setCursorPosition(0, 0);
	for (int j = 0; j < 500; j++) {
		cout << string(100, ' ');
	}
	setCursorPosition(0, 0);
}

/**
	Sets the Loglevel.

	@param level the log level
	@return nothing
*/
void Console::setLogLevel(LogLevel level) {
	this->logLevel = level;
}

/**
	Append string to log file.

	@param str the string to append
	@return nothing
*/
void Console::appendToLogFile(std::string str) {
	errno_t err;
	FILE *pFile;
	err = fopen_s(&pFile, "C:\\tubs\\log.txt", "a");
	if (err == 0) {
		if (pFile) {
			fprintf_s(pFile, "%s\n", str.c_str());
			fclose(pFile);
		}
	}
}