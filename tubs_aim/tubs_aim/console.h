#pragma once

enum ConsoleColor {
	black = 0,
	dark_blue = 1,
	dark_green = 2,
	dark_aqua, dark_cyan = 3,
	dark_red = 4,
	dark_purple = 5, dark_pink = 5, dark_magenta = 5,
	dark_yellow = 6,
	dark_white = 7,
	gray = 8,
	blue = 9,
	green = 10,
	aqua = 11, cyan = 11,
	red = 12,
	purple = 13, pink = 13, magenta = 13,
	yellow = 14,
	white = 15
};

enum LogLevel {
	LOG_ALL = 0,
	LOG_INFO = 1,
	LOG_WARN = 2,
	LOG_ERROR = 3
};

const ConsoleColor logLevelColors[4] = { white, blue, yellow, red };

class Console
{
	public:
		static Console& getInstance() {
			// Since it's a static variable, if the class has already been created,
			// it won't be created again.
			// And it **is** thread-safe in C++11.
			static Console instance;

			// Return a reference to our instance.
			return instance;
		}

		// delete copy and move constructors and assign operators
		Console(Console const&) = delete;				// Copy construct
		Console(Console&&) = delete;					// Move construct
		Console& operator=(Console const&) = delete;	// Copy assign
		Console& operator=(Console &&) = delete;		// Move assign

		void printInfo();

		void print(std::string str, ConsoleColor textColor = white, ConsoleColor bgColor = black);

		void println(std::string str, ConsoleColor textColor = white, ConsoleColor bgColor = black);

		void printWithoutTime(std::string str, ConsoleColor textColor = white, ConsoleColor bgColor = black);

		void printlnWithoutTime(std::string str, ConsoleColor textColor = white, ConsoleColor bgColor = black);

		void log(LogLevel level, std::string str);

		void logln(LogLevel level, std::string str);

		void logWithoutTime(LogLevel level, std::string str);

		void loglnWithoutTime(LogLevel level, std::string str);

		void setSize(int width, int height);

		void setColor(ConsoleColor textColor, ConsoleColor bgColor);

		void setCursorVisible(bool visible);

		void setCursorPosition(int x, int y);

		void setLogLevel(LogLevel level);

		void clear();

		void appendToLogFile(std::string str);

		std::string getTimeString();

	protected:
		Console() {
			// Constructor code goes here.
			logLevel = LOG_ERROR;
		}

		~Console() {
			// Destructor code goes here.
		}

		// And any other protected methods.

	private:
		LogLevel		logLevel;
};
/*
 * THREADSAFE SINGLETON CLASS
 * SOURCE: https://stackoverflow.com/questions/11711920/how-to-implement-multithread-safe-singleton-in-c11-without-using-mutex
 */