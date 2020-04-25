#include "stdafx.h"
#include "dllmain.h"

Entity localEntity;
Entity entityList[65];

/**
	Constructor. Creates a new Entity object.

	@return nothing
*/
Entity::Entity() {
	m_iIndex = NULL;

	m_dwPointer = NULL;
	m_dwLocalPointer = NULL;
	m_dwBoneMatrix = NULL;
	m_dwIndex = NULL;

	memset(m_BoneMatrix, NULL, sizeof(Matrix3x4[128]));

	m_vEyeAngles = Vector();
	m_vOrigin = Vector();
	m_vEye = Vector();
	m_vPunch = Vector();
	m_vVelocity = Vector();

	m_iTeamNum = NULL;
	m_iHealth = NULL;
	m_iFlags = NULL;

	m_iCrossHairID = 0;
	m_hActiveWeapon = NULL;
	m_iShotsFired = 0;
	m_iKills = 0;
	m_iDeaths = 0;
	m_nLastKillerIndex = 0;
	m_totalHitsOnServer = 0;
	m_iLastHitGroup = 0;
	m_nSurvivalTeam = 0;

	m_i64SpottedByMask = 0;

	m_bDead = true;
	m_bIsDormant = true;
	m_bVisible = false;
	m_bBSPVisible = false;
	m_bHasBomb = false;
}

/**
	Destructor.

	@return nothing
*/
Entity::~Entity() {

}

/**
	Perform an update on all player values. All values are newly read from the memory.

	@param playerIndex the index of the player
	@return nothing
*/
void Entity::update(int playerIndex) {
	//clear();
	m_iIndex = playerIndex;

	m_dwPointer = memory->Read<DWORD_PTR>(client->GetImage() + offsets->m_dwEntityList + m_iIndex * 0x10);

	if (!m_dwPointer)
		return;

	//    Read Player Chunk    //
	memory->Read(m_dwPointer, chunk, sizeof(chunk));
	//	End Read Player Chunk //

	m_dwIndex = *reinterpret_cast<int *>(chunk + offsets->m_dwIndex);
	m_iHealth = *reinterpret_cast<int *>(chunk + offsets->m_iHealth);

	m_dwBoneMatrix = *reinterpret_cast<DWORD_PTR *>(chunk + offsets->m_dwBoneMatrix);

	if (!m_dwBoneMatrix)
		return;

	memory->Read(m_dwBoneMatrix, m_BoneMatrix, sizeof(Matrix3x4[128]));

	m_vOrigin = *reinterpret_cast<Vector *>(chunk + offsets->m_vecOrigin);
	m_vEye = m_vOrigin + *reinterpret_cast<Vector *>(chunk + offsets->m_vecViewOffset);
	m_vEyeAngles = *reinterpret_cast<Vector *>(chunk + offsets->m_angEyeAngles);
	m_vVelocity = *reinterpret_cast<Vector *>(chunk + offsets->m_vecVelocity);
	m_vPunch = *reinterpret_cast<Vector *>(chunk + offsets->m_aimPunchAngle);
	m_iTeamNum = *reinterpret_cast<int *>(chunk + offsets->m_iTeamNum);
	m_iFlags = *reinterpret_cast<int *>(chunk + offsets->m_fFlags);
	//m_iCrossHairID = *reinterpret_cast<int *>(offsets->m_iCrossHairID);
	m_hActiveWeapon = *reinterpret_cast<int *>(chunk + offsets->m_hActiveWeapon);
	m_iShotsFired = *reinterpret_cast<int *>(chunk + offsets->m_iShotsFired);
	m_bDead = *reinterpret_cast<bool *>(chunk + offsets->m_lifeState);
	m_bIsDormant = *reinterpret_cast<bool *>(chunk + offsets->m_bDormant);
	m_i64SpottedByMask = *reinterpret_cast<INT64 *>(chunk + offsets->m_bSpottedByMask);
	m_iLastHitGroup = *reinterpret_cast<int *>(chunk + offsets->m_LastHitGroup);
	m_nSurvivalTeam = *reinterpret_cast<int *>(chunk + offsets->m_nSurvivalTeam);
	m_totalHitsOnServer = *reinterpret_cast<int *>(chunk + offsets->m_totalHitsOnServer);
	m_nLastKillerIndex = *reinterpret_cast<int *>(chunk + offsets->m_nLastKillerIndex);
	m_flFlashCurrentAlpha = *reinterpret_cast<float *>(chunk + offsets->m_flFlashMaxAlpha - 8);
	//m_flFlashMaxAlpha = *reinterpret_cast<float *>(chunk + offsets->m_flFlashMaxAlpha);
	//m_flFlashDuration = *reinterpret_cast<float *>(chunk + offsets->m_flFlashDuration);

	//DEBUG
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_flFlashCurrentAlpha: " + std::to_string(m_flFlashCurrentAlpha));
	//SOURCE: https://www.unknowncheats.me/forum/counterstrike-global-offensive/185847-m_flflashduration-exact-time-cant-react-due-flashed.html

	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_nSurvivalTeam: " + std::to_string(m_nSurvivalTeam));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_iCrossHairID: " + std::to_string(m_iCrossHairID));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_iLastHitGroup: " + std::to_string(m_iLastHitGroup));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_totalHitsOnServer: " + std::to_string(m_totalHitsOnServer));
	
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " Kills: " + std::to_string(getKillsNum()));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " Deaths: " + std::to_string(getDeathsNum()));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " LastKillerIndex: " + std::to_string(getLastKillerIndex(m_iIndex)));

	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_iHealth: " + std::to_string(m_iHealth));
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " m_i64SpottedByMask: " + std::to_string(m_i64SpottedByMask));
	//INT64 isSpottedByMask = entityList[m_iIndex].isSpottedByMask();
	//Console::getInstance().println("m_iIndex " + std::to_string(m_iIndex) + " IsSpottedByMask: " + std::to_string(isSpottedByMask));

	//Matrix4x4 vm = game->getViewMatrix();
	//Console::getInstance().println("m_dwViewMatrix " + std::to_string(offsets->m_dwViewMatrix));
	//Console::getInstance().println("vm[0][0] " + std::to_string(vm.Matrix[0][0]) + "vm[0][1] " + std::to_string(vm.Matrix[0][1]) + "vm[0][2] " + std::to_string(vm.Matrix[0][2]) + "vm[0][3] " + std::to_string(vm.Matrix[0][3]));
	//Console::getInstance().println("vm[1][0] " + std::to_string(vm.Matrix[1][0]) + "vm[1][1] " + std::to_string(vm.Matrix[1][1]) + "vm[1][2] " + std::to_string(vm.Matrix[1][2]) + "vm[1][3] " + std::to_string(vm.Matrix[1][3]));
	//Console::getInstance().println("vm[2][0] " + std::to_string(vm.Matrix[2][0]) + "vm[2][1] " + std::to_string(vm.Matrix[2][1]) + "vm[2][2] " + std::to_string(vm.Matrix[2][2]) + "vm[2][3] " + std::to_string(vm.Matrix[2][3]));
	//Console::getInstance().println("vm[3][0] " + std::to_string(vm.Matrix[3][0]) + "vm[3][1] " + std::to_string(vm.Matrix[3][1]) + "vm[3][2] " + std::to_string(vm.Matrix[3][2]) + "vm[3][3] " + std::to_string(vm.Matrix[3][3]));

	if (!config->general.bspparsing_enabled) {
		checkVisible();
		//Console::getInstance().println("checkVisible");
	}
}

/**
	Returns the pointer to the memory address where the player data is located.

	@return pointer to memory address
*/
DWORD_PTR Entity::getPointer() {
	return m_dwPointer;
}

/**
	Returns the pointer to the memory address where the local player data is located.

	@return pointer to memory address
*/
DWORD_PTR Entity::getLocalPointer() {
	return m_dwLocalPointer;
}

/**
	Returns the pointer to the memory address where the players bonematrix data is located.

	@return pointer to memory address
*/
DWORD_PTR Entity::getBoneMatrix() {
	return m_dwBoneMatrix;
}

/**
	Returns the matrix of the bone with the given index.

	@param boneIndex the index of the bone
	@return a 3x4 matrix with the bone data
*/
Matrix3x4 Entity::getBoneMatrix(int boneIndex) {
	return m_BoneMatrix[boneIndex];
}

/**
	Returns the players punch angles.

	@return the punch angles as a vector
*/
Vector Entity::getPunchAngles() {
	return m_vPunch;
}

/**
	Returns the players origin position.

	@return the origin position as a vector
*/
Vector Entity::getOrigin() {
	return m_vOrigin;
}

/**
	Returns the players eye angles.

	@return the eye angles as a vector
*/
Vector Entity::getEyeAngles() {
	return m_vEyeAngles;
}

/**
	Returns the players eye position.

	@return the eye position as a vector
*/
Vector Entity::getEyePosition() {
	return m_vEye;
}

/**
	Returns the players velocity.

	@return the velocity as a vector
*/
Vector Entity::getVelocity() {
	return m_vVelocity;
}

/**
	Returns whether the player is flashed.

	@return true if the player is flashed, otherwise false
*/
bool Entity::isFlashed() {
	return (m_flFlashCurrentAlpha > 200.0f);
}

/**
	Returns the players speed.

	@return the speed as an integer
*/
int Entity::getSpeed() {
	Vector velocity = m_vVelocity;
	//Console::getInstance().println("speed " + std::to_string(speed)  + " m_vecVelocity: " + std::to_string(velocity[0]) + " " + std::to_string(velocity[1]) + " " + std::to_string(velocity[2]));
	return static_cast<int>(sqrt(velocity.DotProduct(velocity)));
}

/**
	Returns the position of the given bone.

	@param boneIndex the index of the bone
	@return the bone position as a vector
*/
Vector Entity::getBonePosition(int boneIndex) {
	Vector bonePosition;
	if (boneIndex == -1) {
		/* The bone with the index - 1 does not exist. This is used as a workaround.
		 * We take the position of the bone with the index 0 (center of the player) and subtract in the z-axis (height) 4.f units.
		 */
		bonePosition = Vector(m_BoneMatrix[0].m[0][3], m_BoneMatrix[0].m[1][3], m_BoneMatrix[0].m[2][3]);
		bonePosition[2] -= 4.f;
	} else {
		bonePosition = Vector(m_BoneMatrix[boneIndex].m[0][3], m_BoneMatrix[boneIndex].m[1][3], m_BoneMatrix[boneIndex].m[2][3]);
	}
	return bonePosition;
}

/**
	Returns the players current weapon.

	@return the weapon id
*/
int Entity::getActiveWeapon() {
	short weaponId = memory->Read<short>(getActiveWeaponBase() + offsets->m_iItemDefinitionIndex);
	if (weaponId > 64 || weaponId < 0) // If number is greater than array size return 0 to prevent crash
		weaponId = 42;	// set weapon ID to knife

	return weaponId;
}

/**
	Returns a pointer to the memory address of the players weaponbase.

	@return pointer to the memory address
*/
DWORD_PTR Entity::getActiveWeaponBase() {
	DWORD_PTR dwBaseCombatWeaponIndex = *reinterpret_cast<DWORD_PTR *>(chunk + offsets->m_hActiveWeapon);
	dwBaseCombatWeaponIndex &= 0xFFF;
	return memory->Read<DWORD_PTR>(client->GetImage() + offsets->m_dwEntityList + (dwBaseCombatWeaponIndex - 1) * 0x10);
}

/**
	Checks if the current weapon is NOT a valid firearm.

	@return true if it is not valid, otherwise false
*/
bool Entity::isActiveWeaponNotValid() {
	//int WeaponId = getActiveWeapon();
	//return(WeaponId == WID_KnifeGG || WeaponId == WID_Knife || WeaponId == WID_Flashbang || WeaponId == WID_HEFrag || WeaponId == WID_Smoke || WeaponId == WID_Molly || WeaponId == WID_Decoy || WeaponId == WID_Firebomb || WeaponId == WID_C4);
	EWeaponType weaponType = getWeaponType();
	return(weaponType == EWeaponType::WeaponType_C4Explosive || weaponType == EWeaponType::WeaponType_Grenade || weaponType == EWeaponType::WeaponType_KnifeType);
}

/**
	Checks if the current weapon is a valid firearm.

	@return true if it is valid, otherwise false
*/
bool Entity::isActiveWeaponValid() {
	return (!isActiveWeaponNotValid());
}

/**
	Returns the number of remaining shots in the magazine.

	@return the remaining shots
*/
int Entity::getActiveWeaponAmmo() {
	DWORD_PTR WeaponBase = getActiveWeaponBase();
	return memory->Read<int>(WeaponBase + offsets->m_iClip1);
}

/**
	Checks if the current weapon is a pistol.

	@return true if it is a pistol, otherwise false
*/
bool Entity::isActiveWeaponPistol() {
	int weaponId = getActiveWeapon();
	return(weaponId == WID_Deagle || weaponId == WID_Revolver || weaponId == WID_Dual_Berettas || weaponId == WID_Five_Seven || weaponId == WID_Glock
			|| weaponId == WID_P250 || weaponId == WID_USP || weaponId == WID_Tec9 || weaponId == WID_Zeus || weaponId == WID_P2000);
}

/**
	Checks if the current weapon is a primary weapon. A primary weapon uses the first weaponslot.

	@return true if it is a primary weapon, otherwise false
*/
bool Entity::isActiveWeaponPrimary() {
	int weaponId = getActiveWeapon();
	return(weaponId == WID_AWP || weaponId == WID_G3SG1_Auto || weaponId == WID_SCAR_Auto || weaponId == WID_Scout || weaponId == WID_Negev
			|| weaponId == WID_M249 || weaponId == WID_XM1014 || weaponId == WID_MAG7 || weaponId == WID_Nova || weaponId == WID_SawedOff
			|| weaponId == WID_MAC10 || weaponId == WID_P90 || weaponId == WID_UMP45 || weaponId == WID_PPBizon || weaponId == WID_MP7
			|| weaponId == WID_MP9 || weaponId == WID_AK47 || weaponId == WID_AUG || weaponId == WID_FAMAS || weaponId == WID_M4A4
			|| weaponId == WID_M4A1S || weaponId == WID_Galil || weaponId == WID_SG553);
}

/**
	Returns the type of the current players weapon.

	@return the type of the weapon
*/
EWeaponType Entity::getWeaponType() {
	int weaponId = getActiveWeapon();

	switch (weaponId)
	{
	case WID_C4:
		return EWeaponType::WeaponType_C4Explosive;

	case WID_Zeus:
		return EWeaponType::WeaponType_ZeusGun;

	case WID_Negev:
	case WID_M249:
		return EWeaponType::WeaponType_LMG;

	case WID_AWP:
	case WID_G3SG1_Auto:
	case WID_SCAR_Auto:
	case WID_Scout:
		return EWeaponType::WeaponType_Sniper;

	case WID_XM1014:
	case WID_MAG7:
	case WID_Nova:
	case WID_SawedOff:
		return EWeaponType::WeaponType_Shotgun;

	case WID_Flashbang:
	case WID_Smoke:
	case WID_Firebomb:
	case WID_HEFrag:
	case WID_Molly:
		return EWeaponType::WeaponType_Grenade;

	case WID_MAC10:
	case WID_P90:
	case WID_UMP45:
	case WID_PPBizon:
	case WID_MP7:
	case WID_MP9:
		return EWeaponType::WeaponType_SMG;

	case WID_Deagle:
	case WID_Dual_Berettas:
	case WID_Five_Seven:
	case WID_USP:
	case WID_Glock:
	case WID_Tec9:
	case WID_P2000:
	case WID_P250:
	case WID_CZ75:
	case WID_Revolver:
		return EWeaponType::WeaponType_Pistol;

	case WID_AK47:
	case WID_AUG:
	case WID_FAMAS:
	case WID_M4A4:
	case WID_M4A1S:
	case WID_Galil:
	case WID_SG553:
		return EWeaponType::WeaponType_Rifle;

	case WID_Default_Knife:
	case WID_ButterflyKnife:
	case WID_FlipKnife:
	case WID_HuntsmanKnife:
	case WID_M9BayonetKnife:
	case WID_KarambitKnife:
	case WID_FalchionKnife:
	case WID_ShadowDaggerKnife:
	case WID_BayonetKnife:
	case WID_GutKnife:
	case WID_BowieKnife:
		return EWeaponType::WeaponType_KnifeType;

	default:
		return (EWeaponType)-1;
	}
}

/**
	Returns the number of the players team.

	@return the number of the team
*/
int Entity::getTeamNum() {
	return m_iTeamNum;
}

/**
	Returns the health of the player.

	@return the health
*/
int Entity::getHealth() {
	return m_iHealth;
}

/**
	Returns the index of the player being targeted.

	@return the index of the target player
*/
int Entity::getCrosshairEnt() {
	return memory->Read<int>(localEntity.getPointer() + offsets->m_iCrossHairID);
}

/**
	Returns the number of shots fired during a firing period. Resets when the fire button is no longer pressed.

	@return the number of shots
*/
int Entity::getShotsFired() {
	return m_iShotsFired;
}

/**
	Returns the index of the player.

	@return the index
*/
int Entity::getIndex() {
	return m_dwIndex;
}

/**
	Returns the flags of the player. For example: crouched

	@return the flags
*/
int Entity::getFlags() {
	return m_iFlags;
}

/**
	Check if the player is valid. Several properties are checked.

	@return true if the player is valid, otherwise false
*/
bool Entity::isValid() {
	if (!getPointer())
		return false;

	if (!getBoneMatrix())
		return false;

	if (!getTeamNum())
		return false;

	if (isDead())
		return false;

	if (isDormant())
		return false;

	if (getOrigin().IsZero())
		return false;

	if (getHealth() < 1)
		return false;

	return true;
}

/**
	Check if the player is dead or alive.

	@return true if the player is dead, otherwise false
*/
bool Entity::isDead() {
	return m_bDead;
}

/**
	Check if the player is idle.

	@return true if the player is idle, otherwise false
*/
bool Entity::isDormant() {
	return m_bIsDormant;
}

/**
	Check if the player is jumping.

	@return true if the player is jumping, otherwise false
*/
bool Entity::isJumping() {
	int fhh = m_iFlags;
	if ((fhh & 0x1) == 1 || (fhh & 0x1) == 3 || (fhh & 0x1) == 5 || (fhh & 0x1) == 7)
	{
		return false;
	}
	return true;
}

/**
	Check if the player is spotted by the local player.

	@return 1 if the player is spotted, otherwise 0
*/
INT64 Entity::isSpottedByMask() {
	return (m_i64SpottedByMask & ((UINT64)(static_cast<UINT64>(1)) << (UINT64)(static_cast<UINT64>(game->getLocalPlayer()))));
}

/**
	Check if the player is spotted by the local player. Still update variable.

	@return nothing
*/
void Entity::checkVisible() {
	m_bVisible = isSpottedByMask();
}

/**
	Sets the player visible to the local player.

	@return nothing
*/
void Entity::setVisible(bool isVisible) {
	m_bVisible = isVisible;
}

/**
	Check if the player is visible for the local player.

	@return true if the player is visible, otherwise false
*/
bool Entity::isVisible() {
	return m_bVisible;
}

/**
	Sets the player visible to the local player. Based on the result of the BSP-parse.

	@return nothing
*/
void Entity::setBSPVisible(bool isVisible) {
	m_bBSPVisible = isVisible;
}

/**
	Check if the player is visible for the local player. Based on the result of the BSP-parse.

	@return true if the player is visible, otherwise false
*/
bool Entity::isBSPVisible() {
	return m_bBSPVisible;
}

/**
	Returns the name of the player. (Not implemented yet!)

	@return the playername
*/
std::wstring Entity::getPlayerName() {
	return L"playername"; //TODO: get playername from memory
}

/**
	Resets all variables (values) of the player.

	@return nothing
*/
void Entity::clear() {
	Console::getInstance().println("!!!Clear!!! m_iIndex " + m_iIndex);

	m_iIndex = NULL;

	m_dwPointer = NULL;
	m_dwLocalPointer = NULL;
	m_dwBoneMatrix = NULL;
	m_dwIndex = NULL;

	memset(m_BoneMatrix, NULL, sizeof(Matrix3x4[128]));

	m_vEyeAngles = Vector();
	m_vOrigin = Vector();
	m_vEye = Vector();
	m_vPunch = Vector();
	m_vVelocity = Vector();

	m_iTeamNum = NULL;
	m_iHealth = NULL;
	m_iFlags = NULL;

	m_iCrossHairID = 0;
	m_hActiveWeapon = NULL;
	m_iShotsFired = 0;
	m_iKills = 0;
	m_iDeaths = 0;
	m_nLastKillerIndex = 0;
	m_totalHitsOnServer = 0;
	m_iLastHitGroup = 0;
	m_nSurvivalTeam = 0;

	m_i64SpottedByMask = 0;

	m_bDead = true;
	m_bIsDormant = true;
	m_bVisible = false;
	m_bBSPVisible = false;
	m_bHasBomb = false;
}

/**
	Check if the player is valid for the profile module. Several properties are checked.

	@return true if the player is valid, otherwise false
*/
bool Entity::isValidForProfiler() {
	if (!getPointer())
		return false;

	if (!getBoneMatrix())
		return false;

	if (!getTeamNum())
		return false;

	if (getOrigin().IsZero())
		return false;

	return true;
}

/**
	Returns the pointer to the memory address to the player resources.

	@return the pointer to the memory address
*/
DWORD_PTR Entity::getPlayerResourceBase() {
	return memory->Read<DWORD_PTR>(client->GetImage() + offsets->m_dwPlayerResource);
}

/**
	Returns the number of kills from the player.

	@return the number of kills
*/
int Entity::getKillsNum() {
	return memory->Read<int>(getPlayerResourceBase() + offsets->m_iKills + (m_iIndex + 1) * 4);
}

/**
	Returns the number of deaths from the player.

	@return the number of deaths
*/
int Entity::getDeathsNum() {
	return memory->Read<int>(getPlayerResourceBase() + offsets->m_iDeaths + (m_iIndex + 1) * 4);
}

/**
	Returns the player's last killer.

	@param refresh force to read from memory again.
	@return the last killer
*/
int Entity::getLastKillerIndex(bool refresh) {
	if (refresh) {
		DWORD Entity = memory->Read<DWORD>(client->GetImage() + offsets->m_dwEntityList + m_iIndex * 0x10);
		m_nLastKillerIndex = memory->Read<int>(Entity + offsets->m_nLastKillerIndex);
	}
	return (m_nLastKillerIndex - 1);
}

/**
	Returns the number of player's total hits.

	@param refresh force to read from memory again.
	@return the total hits
*/
int Entity::getTotalHitsOnServer(bool refresh) {
	if (refresh) {
		DWORD Entity = memory->Read<DWORD>(client->GetImage() + offsets->m_dwEntityList + m_iIndex * 0x10);
		m_totalHitsOnServer = memory->Read<int>(Entity + offsets->m_totalHitsOnServer);
	}
	return m_totalHitsOnServer;
}

/**
	Returns the team number of the player. (DangerZone)

	@return the team number
*/
int Entity::getSurvivalTeamNum() {
	return m_nSurvivalTeam;
}

/**
	Returns the number of wins in competition mode.

	@return the number of wins
*/
int Entity::getCompetitiveWins() {
	return memory->Read<int>(getPlayerResourceBase() + offsets->m_iCompetitiveWins + (m_iIndex + 1) * 4);
}

/**
	Returns the id of the hit group the player hit last.

	@return the id of the hit group
*/
int Entity::getLastHitGroup() {
	return m_iLastHitGroup;
}