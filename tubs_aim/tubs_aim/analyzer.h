#pragma once

#include "dllmain.h"

#define FIRST_WEAPON_TYPE						0
#define LAST_WEAPON_TYPE						9

#define FIRST_WEAPON_ID							1
#define LAST_WEAPON_ID							64

#define FIRST_WEAPON_INDEX						0
#define LAST_WEAPON_INDEX						34

#define TEAMID_NONE								0
#define TEAMID_SPECTATOR						1
#define TEAMID_TERRORISTS						2
#define TEAMID_COUNTERTERRORISTS				3

#define RESULTID_CT_KILLS_T						1
#define RESULTID_CT_HOSTAGE_RESCUED				2
#define RESULTID_CT_DEFUSE_BOMB					3
#define RESULTID_CT_TIME_IS_OVER				4
#define RESULTID_T_KILLS_CT						5
#define RESULTID_T_DETONATES_BOMB				6
#define RESULTID_T_TIME_IS_OVER					7

#define MAX_ROUNDS_PER_MATCHMAKING				30

#define MM_ENDRESULT_WIN						0
#define MM_ENDRESULT_LOSE						1
#define MM_ENDRESULT_DRAW						2

#define MIN_IMPROVEMENT_DELAY					(60 * 45 * 10)
#define MAX_IMPROVEMENT_DELAY					(60 * 45 * 15)

#define	MIN_SELECTION_VALUE_FOR_ADJUSTMENT		0 // 1
#define MAX_SELECTION_VALUE_FOR_ADJUSTMENT		100 // 10

#define	MIN_PERCENTAGE_FOR_ADJUSTMENT			0.0f
#define	MAX_PERCENTAGE_FOR_ADJUSTMENT			0.01f

#define ANALYZER_RETURN_OK						1
#define ANALYZER_RETURN_NOT_ENOUGH_VALUES		2
#define ANALYZER_RETURN_ERROR					3

#define ANALYZER_ACTION_ABORT					0
#define ANALYZER_ACTION_ADAPT					1
#define ANALYZER_ACTION_ADAPT2					2
#define ANALYZER_ACTION_DEACTIVATE				3
#define ANALYZER_ACTION_IGNORE					4 //this mode is still buggy (need more checks for invalid values)

#define MIN_WON_MATCHMAKINGS_FOR_ANALYZING		3
#define MIN_PLAYED_MATCHMAKINGS_FOR_ANALYZING	15
#define MIN_PLAYTIME_FOR_ANALYZING				(60 * 45 * MIN_PLAYED_MATCHMAKINGS_FOR_ANALYZING) //playtime in seconds (60sec * 45min * played_mm)

#define MIN_NUM_OF_VALUES_FOR_FEATURE_1			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_2			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_3			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_4			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_5			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_6			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_7			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_8			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_9			3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_10		3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_11		1
#define MIN_NUM_OF_VALUES_FOR_FEATURE_12		3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_13		3
#define MIN_NUM_OF_VALUES_FOR_FEATURE_14		3

#define LOWER_BOUND_FOR_FEATURE_1				0.0f
#define UPPER_BOUND_FOR_FEATURE_1				45.0f

#define LOWER_BOUND_FOR_FEATURE_2				0.0f
#define UPPER_BOUND_FOR_FEATURE_2				1.0f

#define LOWER_BOUND_FOR_FEATURE_3				0.0f
#define UPPER_BOUND_FOR_FEATURE_3				1.0f

#define LOWER_BOUND_FOR_FEATURE_4				0
#define UPPER_BOUND_FOR_FEATURE_4				5000
	
#define LOWER_BOUND_FOR_FEATURE_5				0.0f
#define UPPER_BOUND_FOR_FEATURE_5				750.0f

#define LOWER_BOUND_FOR_FEATURE_6				0.0f
#define UPPER_BOUND_FOR_FEATURE_6				1.0f

#define LOWER_BOUND_FOR_FEATURE_7				1
#define UPPER_BOUND_FOR_FEATURE_7				2000

#define LOWER_BOUND_FOR_FEATURE_8				0.0f
#define UPPER_BOUND_FOR_FEATURE_8				1.0f

#define LOWER_BOUND_FOR_FEATURE_9				0.0f
#define UPPER_BOUND_FOR_FEATURE_9				1.0f

#define LOWER_BOUND_FOR_FEATURE_10				0.0f
#define UPPER_BOUND_FOR_FEATURE_10				1.0f

#define LOWER_BOUND_FOR_FEATURE_11				1.0f
#define UPPER_BOUND_FOR_FEATURE_11				500.0f

#define LOWER_BOUND_FOR_FEATURE_12				-1.0f
#define UPPER_BOUND_FOR_FEATURE_12				3.0f

#define LOWER_BOUND_FOR_FEATURE_13				1.0f
#define UPPER_BOUND_FOR_FEATURE_13				20.0f

#define LOWER_BOUND_FOR_FEATURE_14				0.0f
#define UPPER_BOUND_FOR_FEATURE_14				5.0f

class Analyzer
{
public:
	void			start();

private:
	void			analyze();
	void			readFromProfileFile();
	void			readFromConfigFile();
	void			readFromAdjustmentTestFile();
	void			sendCalculationResultMessage(int result, int featureId, int* errorCounter);
	bool			checkRequirements();

	int				calculateFeature1();
	int				calculateFeature2();
	int				calculateFeature3();
	int				calculateFeature4();
	int				calculateFeature5();
	int				calculateFeature6();
	int				calculateFeature7();
	int				calculateFeature8();
	int				calculateFeature9();
	int				calculateFeature10();
	int				calculateFeature11();
	int				calculateFeature12();
	int				calculateFeature13();
	int				calculateFeature14();

	void			setWeaponDivergenceAtFirstShot(int weaponId, float div);
	void			setWeaponAimTimePerDegree(int weaponId, float aimTimePerDegree);
	void			setWeaponDivergenceIfSpotted(int weaponId, float divergenceIfSpotted);
	void			setWeaponPositionOfFirstCriticalHit(int weaponId, float positionOfCriticalHit);
	void			setWeaponHitChanceWhileMoving(int weaponId, float hitChance);
	void			setWeaponKillTime(int weaponId, int killTime);
	void			setWeaponStopAimingDelay(int weaponId, float stopAimingDelay);
	void			setWeaponSwitchChance(int weaponId, float chanceToSwitch);
	void			setWeaponTimeToSwitch(int weaponId, int timeToSwitchWeapon);
	void			setWeaponPrimaryBones(int weaponId, int primaryBones[], int size);
	void			setWeaponChanceForClosestBone(int weaponId, float value);
	void			setWeaponRecoilControlValue(int weaponId, float value);
	void			setWeaponMaxHitAccuracy(int weaponId, float value);
	void			setWeaponMaxHitAccuracyForFirstShot(int weaponId, float value);
	void			activateAimbotWeapon(int weaponId);
	void			deactivateAimbotWeapon(int weaponId);

	EWeaponType		getWeaponType(int weaponId);

	void			initAnalyzerValues();
	void			increasePlayedMatchmakings(int endResultId);
	void			increasePlayedTime(time_t time);
	void			decreaseNextImprovementTime(time_t time);
	void			setNextImprovementTime();
	void			improvePlayerProfile();

	DWORD_PTR		getGameRules();

	bool			isDangerMode();
	bool			isMatchmaking();

	int				getRoundsFinished();
	int				getCurrentRoundNum();
	int				getWinningRoundsByTeam(int teamId);
	int				getOppositeTeamId(int teamId);
	int				randomInt(int a, int b);

	float			randomFloat(float a, float b);

	void			getRoundResults(int results[]);
	void			sortArrayAndGetIndexArray(int inputArray[], const int inputSize, int outputArray[], const int outputSize);

private:
	bool			isAnalyzing;
	bool			isAdjusting;
	bool			recordingMatchmakingStarted;
	bool			aimbotDeactivatedFlag[LAST_WEAPON_ID + 1];
	int				lastRoundNum;

	time_t			lastPlayTime;

	json			profile;
	json			adjustmentTest;
	json			tempConfig;
	int				adjustmentTestCounter;

	int				analyzerWeapons[LAST_WEAPON_INDEX] = { 1, 2, 3, 4, 7, 8, 9, 10, 11, 13, 14, 16, 17, 19, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 38, 39, 40, 60, 61, 63, 64 };
};