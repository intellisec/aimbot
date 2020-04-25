#pragma once

#define		FL_ONGROUND				(1 << 0)
#define		FL_DUCKING				(1 << 1)
#define		FL_INAIR				256
#define		FL_INAIRDUCKING			262

/*
//SOURCE: https://github.com/C0re-Cheats/C0reExternal-Base-v2/blob/42eccf1e6f9aaf6c9418c991ac199bd43f06a8f6/Structs/Enum.cs
FL_IN_AIR_STAND = 256,
FL_ON_GROUND = 257,
FL_IN_AIR_MOVING_TO_STAND = 258,
FL_ON_GROUND_MOVING_TO_STAND = 259,
FL_IN_AIR_MOVING_TO_CROUCH = 260,
FL_ON_GROUND_MOVING_TO_CROUCH = 261,
FL_IN_AIR_CROUCHED = 262,
FL_ON_GROUND_CROUCHED = 263,
FL_IN_WATER = 1280,
FL_IN_PUDDLE = 1281,
FL_IN_WATER_CROUCHED = 1286,
FL_IN_PUDDLE_CROUCHED = 1287
*/

#define		HITGROUP_GENERIC		0
#define		HITGROUP_HEAD			1
#define		HITGROUP_CHEST			2
#define		HITGROUP_STOMACH		3
#define		HITGROUP_LEFTARM		4    
#define		HITGROUP_RIGHTARM		5
#define		HITGROUP_LEFTLEG		6
#define		HITGROUP_RIGHTLEG		7
#define		HITGROUP_GEAR			10

class Entity
{
public:
	Entity();
	~Entity();

	void			update(int playerIndex);
	void			clear();
	void			checkVisible();
	void			setVisible(bool isVisible);
	void			setBSPVisible(bool isVisible);

	DWORD_PTR		getPointer();
	DWORD_PTR		getLocalPointer();
	DWORD_PTR		getBoneMatrix();
	DWORD_PTR		getPlayerResourceBase();
	DWORD_PTR		getActiveWeaponBase();

	EWeaponType		getWeaponType();

	Matrix3x4		getBoneMatrix(int boneIndex);

	Vector			getPunchAngles();
	Vector			getEyeAngles();
	Vector			getOrigin();
	Vector			getVelocity();
	Vector			getEyePosition();
	Vector			getBonePosition(int boneIndex);

	std::wstring	getPlayerName();

	bool			isValidForProfiler();
	bool			isValid();
	bool			isDead();
	bool			isDormant();
	bool 			isJumping();
	bool			isFlashed();
	bool			isVisible();
	bool			isBSPVisible();
	bool			isActiveWeaponNotValid();
	bool			isActiveWeaponValid();
	bool			isActiveWeaponPistol();
	bool			isActiveWeaponPrimary();
	
	int				getActiveWeapon();
	int 			getActiveWeaponAmmo();
	int 			getShotsFired();
	int				getSpeed();
	int				getTeamNum();
	int				getHealth();
	int				getFlags();
	int				getCrosshairEnt();
	int 			getIndex();
	int				getLastHitGroup();
	int				getKillsNum();
	int				getDeathsNum();
	int				getLastKillerIndex(bool refresh);
	int				getTotalHitsOnServer(bool refresh);
	int				getSurvivalTeamNum();
	int				getCompetitiveWins();

	INT64			isSpottedByMask();

private:
	char			chunk[0xAA50];

	DWORD_PTR		m_dwPointer;
	DWORD_PTR		m_dwLocalPointer;
	DWORD_PTR		m_dwBoneMatrix;

	Matrix3x4		m_BoneMatrix[128];

	Vector			m_vEyeAngles;
	Vector			m_vOrigin;
	Vector			m_vEye;
	Vector			m_vPunch;
	Vector			m_vVelocity;

	int				m_iIndex;
	int				m_iTeamNum;
	int				m_iHealth;
	int				m_iFlags;
	int				m_iCrossHairID;
	int				m_hActiveWeapon;
	int 			m_iShotsFired;
	int 			m_dwIndex;
	int				m_iKills;
	int				m_iDeaths;
	int				m_nLastKillerIndex;
	int				m_totalHitsOnServer;
	int				m_iLastHitGroup;
	int				m_nSurvivalTeam;

	float			m_flFlashDuration;
	float			m_flFlashMaxAlpha;
	float			m_flFlashCurrentAlpha;

	INT64			m_i64SpottedByMask;

	bool			m_bDead;
	bool			m_bIsDormant;
	bool			m_bVisible;
	bool			m_bBSPVisible;
	bool			m_bHasBomb;
};

extern Entity localEntity;
extern Entity entityList[65];
