#pragma once
class Helper
{
public:
	static Helper& getInstance() {
		static Helper instance;
		return instance;
	}

private:
	Helper() = default;
	~Helper() = default;
	Helper(const Helper&) = delete;
	Helper operator&(const Helper&) = delete;

public:
	std::string		getLastErrorAsString();
	std::string		virtualKeyCodeToString(UCHAR virtualKey);

	bool			doesCSGOExist();
	bool			checkCSGOWindowState();
	bool			isCSGOWindowActive();

	void			getDesktopResolution(int& horizontal, int& vertical);
	void			generateRandomString(char *outputString, const int length);

private:
	bool			csgoWindowIsActive = false;

};
