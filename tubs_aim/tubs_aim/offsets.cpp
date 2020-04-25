#include "stdafx.h"
#include "dllmain.h"

Offsets* offsets = new Offsets();

std::string Offsets::toHex(DWORD offset) {
	std::stringstream ss;
	ss << std::hex << offset;
	std::string x = ss.str();
	for (auto &c : x) c = toupper(c);

	return x;
}

void Offsets::findOffsets() {
	if (!c_netvars::get().init()) {
		printf("Can't initialize netvar manager!");
		return;
	}

	if (config->general.debug_mode)
		c_netvars::get().dump();

	static auto find_ptr = [](Module* mod, const char* sig, DWORD sig_add, DWORD off_add, bool sub_base = true) -> DWORD
	{
		auto off = memory->FindPattern(mod, sig);
		auto sb = sub_base ? mod->GetImage() : 0;
		off = memory->Read<DWORD>(off + sig_add);

		return (!off ? 0 : off + off_add - sb);
	};

	m_dwIndex = 0x64;

	/* PATTERNS
	 * SOURCE: https://github.com/frk1/hazedumper/blob/3e92010f66b47f1b491a0427f3650b2febb96102/config.json
	 * SOURCE: https://www.unknowncheats.me/forum/counterstrike-global-offensive/103220-global-offensive-structs-offsets-319.html
	 */

	m_dwClientState = find_ptr(engine, "A1 ? ? ? ? 33 D2 6A 00 6A 00 33 C9 89 B0", 0x1, 0);
	m_dwViewAngles = find_ptr(engine, "F3 0F 11 80 ? ? ? ? D9 46 04 D9 05", 0x4, 0, false);
	m_dwInGame = find_ptr(engine, "83 B8 ? ? ? ? 06 0F 94 C0 C3", 0x2, 0, false);
	m_nDeltaTick = find_ptr(engine, "83 BE ? ? ? ? FF 74 25 8D 4E 08 E8", 0x2, 0, false);
	m_dwLocalPlayer = find_ptr(client, "A3 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 59 C3 6A", 0x1, 0x10);
	m_dwEntityList = find_ptr(client, "BB ? ? ? ? 83 FF 01 0F 8C ? ? ? ? 3B F8", 0x1, 0);
	m_dwGlowObject = find_ptr(client, "A1 ? ? ? ? A8 01 75 4B", 0x1, 0x4);
	m_dwForceJump = find_ptr(client, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 08", 0x2, 0);
	m_dwForceAttack = find_ptr(client, "89 0D ? ? ? ? 8B 0D ? ? ? ? 8B F2 8B C1 83 CE 04", 0x2, 0);
	m_bDormant = find_ptr(client, "88 9E ? ? ? ? E8 ? ? ? ? 53 8D 8E ? ? ? ? E8 ? ? ? ? 8B 06 8B CE 53 FF 90 ? ? ? ? 8B 46 64 0F B6 CB 5E 5B 66 89 0C C5 ? ? ? ? 5D C2 04 00", 0x2, 0, false);
	m_dwGlobalVars = find_ptr(client, "A1 ? ? ? ? 8B 40 10 89 41 04", 0x1, 0);
	m_dwPlayerResource = find_ptr(client, "8B 3D ? ? ? ? 85 FF 0F 84 ? ? ? ? 81 C7", 0x2, 0);
	m_dwViewMatrix = find_ptr(client, "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9", 0x3, 0xB0);
	m_dwGameRulesProxy = find_ptr(client, "A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 0F 84 ? ? ? ? 0F 10 05", 0x1, 0);

	m_hActiveWeapon = GET_NETVAR("DT_BaseCombatCharacter", "m_hActiveWeapon");
	m_iCrossHairID = GET_NETVAR("DT_CSPlayer", "m_bHasDefuser") + 0x5C;
	m_iHealth = GET_NETVAR("DT_BasePlayer", "m_iHealth");
	m_iTeamNum = GET_NETVAR("DT_BaseEntity", "m_iTeamNum");
	m_iShotsFired = GET_NETVAR("DT_CSPlayer", "m_iShotsFired");
	m_fFlags = GET_NETVAR("DT_BasePlayer", "m_fFlags");
	m_lifeState = GET_NETVAR("DT_BasePlayer", "m_lifeState");
	m_bSpottedByMask = GET_NETVAR("DT_BaseEntity", "m_bSpottedByMask");
	m_vecOrigin = GET_NETVAR("DT_BaseEntity", "m_vecOrigin");
	m_vecViewOffset = GET_NETVAR("DT_BasePlayer", "m_vecViewOffset[0]");
	m_angEyeAngles = GET_NETVAR("DT_CSPlayer", "m_angEyeAngles");
	m_vecVelocity = GET_NETVAR("DT_BasePlayer", "m_vecVelocity[0]");
	m_aimPunchAngle = GET_NETVAR("DT_CSPlayer", "m_aimPunchAngle");
	m_dwBoneMatrix = GET_NETVAR("DT_BaseAnimating", "m_nForceBone") + 0x1C;
	m_flNextPrimaryAttack = GET_NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	m_nTickBase = GET_NETVAR("DT_BasePlayer", "m_nTickBase");
	m_iItemDefinitionIndex = GET_NETVAR("DT_BaseCombatWeapon", "m_iItemDefinitionIndex");
	m_iKills = GET_NETVAR("DT_PlayerResource", "m_iKills");
	m_iDeaths = GET_NETVAR("DT_PlayerResource", "m_iDeaths");
	m_nLastKillerIndex = GET_NETVAR("DT_CSPlayer", "m_nLastKillerIndex");
	m_totalHitsOnServer = GET_NETVAR("DT_CSPlayer", "m_totalHitsOnServer");
	m_LastHitGroup = GET_NETVAR("DT_BaseCombatCharacter", "m_LastHitGroup");
	m_iClip1 = GET_NETVAR("DT_BaseCombatWeapon", "m_iClip1");
	m_bIsQueuedMatchmaking = GET_NETVAR("DT_CSGameRulesProxy", "m_bIsQueuedMatchmaking");
	m_totalRoundsPlayed = GET_NETVAR("DT_CSGameRulesProxy", "m_totalRoundsPlayed");
	m_flSurvivalStartTime = GET_NETVAR("DT_CSGameRulesProxy", "m_flSurvivalStartTime");
	m_iMatchStats_RoundResults = GET_NETVAR("DT_CSGameRulesProxy", "m_iMatchStats_RoundResults");
	m_nSurvivalTeam = GET_NETVAR("DT_CSPlayer", "m_nSurvivalTeam");
	m_iCompetitiveWins = GET_NETVAR("DT_CSPlayerResource", "m_iCompetitiveWins");
	m_flFlashDuration = GET_NETVAR("DT_CSPlayer", "m_flFlashDuration");
	m_flFlashMaxAlpha = GET_NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha");
	m_flFlashCurrentAlpha = GET_NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha") - 0x8;
}

bool Offsets::checkOffsets() {
#define CHECK_OFFSET(n) \
	if (config->general.debug_mode) \
		printf("%s = %X\n", #n, n); \
	if (!n) { \
		result = false; }

	bool result = true;

	CHECK_OFFSET(m_dwIndex);

	CHECK_OFFSET(m_dwClientState);
	CHECK_OFFSET(m_dwViewAngles);
	CHECK_OFFSET(m_dwInGame);
	CHECK_OFFSET(m_nDeltaTick);
	CHECK_OFFSET(m_dwLocalPlayer);
	CHECK_OFFSET(m_dwEntityList);
	CHECK_OFFSET(m_dwGlowObject);
	CHECK_OFFSET(m_dwForceJump);
	CHECK_OFFSET(m_dwForceAttack);
	CHECK_OFFSET(m_bDormant);
	CHECK_OFFSET(m_dwGlobalVars);
	CHECK_OFFSET(m_dwPlayerResource);
	CHECK_OFFSET(m_dwViewMatrix);
	CHECK_OFFSET(m_dwGameRulesProxy);

	CHECK_OFFSET(m_hActiveWeapon);
	CHECK_OFFSET(m_iCrossHairID);
	CHECK_OFFSET(m_iHealth);
	CHECK_OFFSET(m_iTeamNum);
	CHECK_OFFSET(m_iShotsFired);
	CHECK_OFFSET(m_fFlags);
	CHECK_OFFSET(m_lifeState);
	CHECK_OFFSET(m_bSpottedByMask);
	CHECK_OFFSET(m_vecOrigin);
	CHECK_OFFSET(m_vecViewOffset);
	CHECK_OFFSET(m_angEyeAngles);
	CHECK_OFFSET(m_vecVelocity);
	CHECK_OFFSET(m_aimPunchAngle);
	CHECK_OFFSET(m_dwBoneMatrix);
	CHECK_OFFSET(m_flNextPrimaryAttack);
	CHECK_OFFSET(m_nTickBase);
	CHECK_OFFSET(m_iItemDefinitionIndex);
	CHECK_OFFSET(m_iKills);
	CHECK_OFFSET(m_iDeaths);
	CHECK_OFFSET(m_nLastKillerIndex);
	CHECK_OFFSET(m_totalHitsOnServer);
	CHECK_OFFSET(m_LastHitGroup);
	CHECK_OFFSET(m_iClip1);
	CHECK_OFFSET(m_bIsQueuedMatchmaking);
	CHECK_OFFSET(m_totalRoundsPlayed);
	CHECK_OFFSET(m_flSurvivalStartTime);
	CHECK_OFFSET(m_iMatchStats_RoundResults);
	CHECK_OFFSET(m_nSurvivalTeam);
	CHECK_OFFSET(m_iCompetitiveWins);
	CHECK_OFFSET(m_flFlashDuration);
	CHECK_OFFSET(m_flFlashMaxAlpha);
	CHECK_OFFSET(m_flFlashCurrentAlpha);
	
	return result;
}

void Offsets::getOffsets() {
	findOffsets();

	if (!checkOffsets()) {
		printf("Offsets outdated!\n");
		std::system("pause");
		exit(1);
	}
	//std::system("pause");
	Sleep(1000);

	m_dwClientState = memory->Read<DWORD_PTR>(engine->GetImage() + m_dwClientState);
}
