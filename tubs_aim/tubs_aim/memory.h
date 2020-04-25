#pragma once

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

#define SAFE_DELETE_VECTOR(vec)	if (!vec.empty()) { \
									for (auto& slot : vec) { \
										delete slot; \
										slot = nullptr; \
									} \
								}

class Memory
{
public:
	Memory();
	~Memory();

	bool					Attach(const std::string& ExeName);
	void					Detach();
	bool					DumpModList();

	bool					Read(DWORD_PTR dwAddress, LPVOID lpBuffer, DWORD_PTR dwSize);
	bool					Write(DWORD_PTR dwAddress, LPCVOID lpBuffer, DWORD_PTR dwSize);

	Module*					GetModule(const std::string& ImageName);
	HMODULE					LoadRemote(const std::string& ImageName);

	void                    SetWindow(HWND window);
	HWND                    GetWindow();

	DWORD_PTR				Scan(DWORD_PTR dwStart, DWORD_PTR dwSize, const char* pSignature, const char* pMask);
	DWORD_PTR				FindPattern(Module* mod, const char* pattern);

	template<typename T>
	T inline ReadF(DWORD addr)
	{
		T memory;
		ReadProcessMemory(m_hProcess, (LPVOID)addr, &memory, sizeof(T), NULL);
		return memory;
	}

	template<typename T>
	void inline WriteF(DWORD addr, T data)
	{
		WriteProcessMemory(m_hProcess, (LPVOID)addr, &data, sizeof(T), NULL);
	}

	template<typename T>
	T Read(DWORD_PTR dwAddress, const T& tDefault = T())
	{
		T tRet;

		if (!Read(dwAddress, &tRet, sizeof(T)))
		{
			return tDefault;
		}

		return tRet;
	}

	template<typename T>
	bool Write(DWORD_PTR dwAddress, const T& tValue)
	{
		return Write(dwAddress, &tValue, sizeof(T));
	}

	template<typename T>
	bool WriteProtected(DWORD_PTR dwAddress, const T& tValue)
	{
		DWORD_PTR oldProtect;
		VirtualProtectEx(m_hProcess, (LPVOID)dwAddress, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect);
		Write(dwAddress, &tValue, sizeof(T));
		VirtualProtectEx(m_hProcess, (LPVOID)dwAddress, sizeof(T), oldProtect, NULL);
		return true;
	}

	void debuglog(std::string str) const
	{
		errno_t err;
		FILE *pFile;
		err = fopen_s(&pFile, "debugLog.txt", "a");
		if (pFile) {
			fprintf_s(pFile, "Crash: %s", str.c_str());
			fclose(pFile);
		}
		printf("Crash: %s", str.c_str());
		system("pause");
	}

private:
	DWORD					GetProcessIdByName(const std::string& name);

private:
	DWORD					m_dwProcessId = NULL;
	std::vector<Module*>	m_pModList;
	HWND					m_hWindow = NULL;

public:
	HANDLE					m_hProcess = NULL;
};

extern Memory*	memory;
extern Module*	client;
extern Module*	engine;
