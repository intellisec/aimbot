#pragma once

enum SIGNONSTATE {
	SIGNONSTATE_NONE = 0,
	SIGNONSTATE_CHALLENGE = 1,
	SIGNONSTATE_CONNECTED = 2,
	SIGNONSTATE_NEW = 3,
	SIGNONSTATE_PRESPAWN = 4,
	SIGNONSTATE_SPAWN = 5,
	SIGNONSTATE_FULL = 6,
	SIGNONSTATE_CHANGELEVEL = 7
};

class Engine
{
public:
	Engine();
	~Engine();

	std::string		getGameDirectory();

	const char*		getMapDirectory();

	Matrix4x4		getViewMatrix();

	void			pAttack();
	void			mAttack();
	void			pressAttackKey();
	void			releaseAttackKey();
	void			forceFullUpdate();
	void 			globalsSetup();
	void			getViewAngles(Vector& viewAngles);
	void			setViewAngles(const Vector& viewAngles);

	bool			isConnected();
	bool			isInGame();
	bool 			isTeamSelected();

	int				getLocalPlayer();
	int				getMaxClients();

	float			getCurTime();
	float			getFrameTime();
	float			getIntervalPerTick();
	float   		getInterpolationAmount();

public:
	char			m_pMapName[32];
	char			m_pMapDirectory[255];

private:
	DWORD_PTR		m_dwGlobals;

};

extern Engine* game;