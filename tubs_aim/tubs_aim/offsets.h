#pragma once

class Offsets {
public:

	DWORD m_dwMapDirectory = 0x188;
	DWORD m_nDeltaTick;
	DWORD m_dwPlayerResource;
	DWORD m_dwViewMatrix;
	DWORD m_vecVelocity;
	DWORD m_aimPunchAngle;
	DWORD m_nTickBase;
	DWORD m_dwLocalPlayer;
	DWORD m_dwEntityList;
	DWORD m_dwForceAttack;
	DWORD m_dwClientState;
	DWORD m_dwGlowObject;
	DWORD m_dwForceJump;
	DWORD m_dwGlobalVars;
	DWORD m_hActiveWeapon;
	DWORD m_iCrossHairID;
	DWORD m_dwIndex;
	DWORD m_iHealth;
	DWORD m_iTeamNum;
	DWORD m_iShotsFired;
	DWORD m_bDormant;
	DWORD m_fFlags;
	DWORD m_bSpottedByMask;
	DWORD m_dwInGame;
	DWORD m_lifeState;
	DWORD m_vecOrigin;
	DWORD m_vecViewOffset;
	DWORD m_dwBoneMatrix;
	DWORD m_angEyeAngles;
	DWORD m_dwViewAngles;
	DWORD m_flNextPrimaryAttack;
	DWORD m_iItemDefinitionIndex;
	DWORD m_iClip1;
	DWORD m_iKills;
	DWORD m_iDeaths;
	DWORD m_nLastKillerIndex;
	DWORD m_totalHitsOnServer;
	DWORD m_LastHitGroup;

	DWORD m_dwGameRulesProxy;
	DWORD m_bIsQueuedMatchmaking;
	DWORD m_totalRoundsPlayed;
	DWORD m_flSurvivalStartTime;
	DWORD m_iMatchStats_RoundResults;
	DWORD m_nSurvivalTeam;
	DWORD m_iCompetitiveWins;
	DWORD m_flFlashDuration;
	DWORD m_flFlashMaxAlpha;
	DWORD m_flFlashCurrentAlpha;

	std::string toHex(DWORD offset);
	void getOffsets();
private:
	void findOffsets();
	bool checkOffsets();
};

extern Offsets* offsets;