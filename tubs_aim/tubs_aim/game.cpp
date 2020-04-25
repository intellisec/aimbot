#include "stdafx.h"
#include "dllmain.h"

Engine* game = new Engine();

/**
	Constructor. Creates a new Engine object.

	@return nothing
*/
Engine::Engine() {
	m_dwGlobals = NULL;
}

/**
	Destructor.

	@return nothing
*/
Engine::~Engine() {

}

/**
	Checks if the local player is connected to a server.

	@return true if so, otherwise false
*/
bool Engine::isConnected() {
	if (!offsets->m_dwClientState)
		return false;
	return (memory->Read<int>(offsets->m_dwClientState + offsets->m_dwInGame) == (int)SIGNONSTATE_CONNECTED);
}

/**
	Check if the local player has completely entered the game.

	@return true if so, otherwise false
*/
bool Engine::isInGame() {
	if (!offsets->m_dwClientState)
		return false;
	return (memory->Read<int>(offsets->m_dwClientState + offsets->m_dwInGame) == (int)SIGNONSTATE_FULL);
}

/**
	Check if the local player has selected a team.

	@return true if so, otherwise false
*/
bool Engine::isTeamSelected() {
	return (localEntity.getTeamNum() == 2 || localEntity.getTeamNum() == 3);
}

/**
	Reads and returns the index of the local player.

	@return the index of the local player
*/
int Engine::getLocalPlayer() {
	if (!offsets->m_dwClientState)
		return -1;
	return memory->Read<int>(offsets->m_dwClientState + 0x180);
}

/**
	Reads and returns the view matrix of the local player.

	@return the view matrix of the local player
*/
Matrix4x4 Engine::getViewMatrix() {
	return memory->Read<Matrix4x4>(client->GetImage() + offsets->m_dwViewMatrix);
}

/**
	Reads and returns the view angles of the local player.

	@viewAngles the pointer to the view angles
	@return the view angles of the local player
*/
void Engine::getViewAngles(Vector& viewAngles) {
	if (!offsets->m_dwClientState)
		return;
	viewAngles = memory->Read<Vector>(offsets->m_dwClientState + offsets->m_dwViewAngles);
}

/**
	Writes the given view angles into the memory.

	@viewAngles the pointer to the view angles
	@return nothing
*/
void Engine::setViewAngles(const Vector& viewAngles) {
	if (!offsets->m_dwClientState || viewAngles.IsZero() || !viewAngles.IsTrue())
		return;
	memory->Write<Vector>(offsets->m_dwClientState + offsets->m_dwViewAngles, viewAngles);
}

/**
	Reads and returns the map directory.

	@return the map directory
*/
const char* Engine::getMapDirectory() {
	if (!offsets->m_dwClientState) {
		return nullptr;
	}
	memory->Read(offsets->m_dwClientState + offsets->m_dwMapDirectory, m_pMapDirectory, sizeof(char[255]));
	return m_pMapDirectory;
}

/**
	Reads and returns the game directory.

	@return the game directory
*/
std::string Engine::getGameDirectory() {
	char filename[MAX_PATH];
	if (GetModuleFileNameEx(memory->m_hProcess, NULL, filename, MAX_PATH) == 0) {
		return nullptr;
	}

	std::string tmp_gamePath(filename);
	int pos = tmp_gamePath.find("csgo");
	tmp_gamePath = tmp_gamePath.substr(0, pos);
	tmp_gamePath = tmp_gamePath + "csgo\\";
	return tmp_gamePath;
}

/**
	Returns the pointer to the memory address of the global vars.

	@return the pointer to the memory address
*/
void Engine::globalsSetup() {
	m_dwGlobals = memory->Read<DWORD_PTR>(client->GetImage() + offsets->m_dwGlobalVars);
}

/**
	Reads and returns the current time.

	@return the current time
*/
float Engine::getCurTime() {
	return memory->Read<float>(m_dwGlobals + 0x10);
}

/**
	Reads and returns the frame time.

	@return the frame time
*/
float Engine::getFrameTime() {
	return memory->Read<float>(m_dwGlobals + 0x14);
}

/**
	Reads and returns the number of maximum clients.

	@return the number of maximum clients
*/
int	Engine::getMaxClients() {
	return memory->Read<int>(m_dwGlobals + 0x18);
}

/**
	Reads and returns the interval per tick.

	@return the interval per tick
*/
float Engine::getIntervalPerTick() {
	return memory->Read<float>(m_dwGlobals + 0x20);
}

/**
	Reads and returns the interpolation amount.

	@return the interpolation amount
*/
float Engine::getInterpolationAmount() {
	return memory->Read<float>(m_dwGlobals + 0x24);
}

/**
	Simulates pressing the fire button. Based on mouse/keyboard action.

	@return nothing
*/
void Engine::pressAttackKey() {
	if (!GetAsyncKeyState(config->keys.ingame_fire)) {
		INPUT    Input = { 0 };
		ZeroMemory(&Input, sizeof(INPUT));
		WORD vkey = config->keys.ingame_fire;

		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));
	}
}

/**
	Simulates releasing the fire button. Based on mouse/keyboard action.

	@return nothing
*/
void Engine::releaseAttackKey() {
	if (GetAsyncKeyState(config->keys.ingame_fire)) {
		INPUT    Input = { 0 };
		ZeroMemory(&Input, sizeof(INPUT));
		WORD vkey = config->keys.ingame_fire;

		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));
	}
}

/**
	Simulates pressing the fire button. Based on memory writing.

	@return nothing
*/
void Engine::pAttack() {
	if (!memory->Read<bool>(client->GetImage() + offsets->m_dwForceAttack))
		memory->Write<bool>(client->GetImage() + offsets->m_dwForceAttack, true);
}

/**
	Simulates releasing the fire button. Based on memory writing.

	@return nothing
*/
void Engine::mAttack() {
	if (memory->Read<bool>(client->GetImage() + offsets->m_dwForceAttack))
		memory->Write<bool>(client->GetImage() + offsets->m_dwForceAttack, false);
}

/**
	Forces an update of the player. Sometimes the game is crashing, if the function is called too frequently.

	@return nothing
*/
void Engine::forceFullUpdate() {
	if (!offsets->m_dwClientState) {
		return;
	}
	memory->Write<int>(offsets->m_dwClientState + offsets->m_nDeltaTick, -1);
}
