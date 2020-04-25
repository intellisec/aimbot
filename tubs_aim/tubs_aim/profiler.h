#pragma once

#include "dllmain.h"

#define MAX_PLAYERS_NUM				64

#define RESET_FIRST_ENCOUNTER_TIME	10
#define MAX_REDCORDED_ANGLES		150

#define START_FOV_FOR_FEATURE_1		40.0f
#define START_FOV_FOR_FEATURE_8		10.0f
#define	START_FOV_FOR_FEATURE_10	30.0f
#define START_FOV_FOR_FEATURE_11	20.0f
#define MAX_FOV_FOR_FEATURE_12		12.0f
#define	START_FOV_FOR_FEATURE_13	40.0f
#define START_FOV_FOR_FEATURE_14	8.0f

#define MIN_REDCORDS_FOR_FEATURE_12 5 //the lower bound for this value is 1 and the upper bound is MAX_REDCORDED_ANGLES

class Profiler
{
public:
	void			start();

private:
	//events
	void			onPlayerGetKilled(int killerIndex, int victimIndex, int damage);
	void			onPlayerHitTarget(int playerIndex, int targetIndex, int boneId, int damage);
	void			onPlayerWeaponShot(int playerIndex, int targetIndex = -1);

	void			readFromProfileFile();
	void			writeToProfileFile();
	void			appendToProfileFile(json j);

	//features
	void			addFeature_1(float div, int timesSeenBefor, int weaponId, time_t time);
	void			startFeature_1(int targetIndex, float angle);
	void			stopFeature_1(int targetIndex);
	void			resetFeature_1(int targetIndex);

	void			addFeature_2(json hits, int weaponId, time_t time);

	void			addFeature_3(int allHits, int hitsWhileMoving, int weaponId, time_t time);
	void			resetFeature_3();

	void			addFeature_4(int timeToKill, int weaponId, time_t time);

	void			addFeature_5(int aimTime, std::string reason, int weaponId, time_t time);
	void			startFeature_5(int targetIndex);
	void			stopFeature_5(int targetIndex, std::string reason);

	void			addFeature_6(int timeToSwitch, int weaponId, time_t time);
	void			startFeature_6(int targetIndex);
	void			stopFeature_6(int targetIndex, bool weaponReloaded);
	void			resetFeature_6(int targetIndex);

	//void			addFeature_7(int weaponId, time_t time); //can be calculated from feature 6

	void			addFeature_8(int firstHitBoneId, int closestBoneId, int weaponId, time_t time);
	void			startFeature_8(int targetIndex);
	void			stopFeature_8(int targetIndex);
	void			resetFeature_8(int targetIndex);

	void			addFeature_9(int hitsWhileAiming, int shotsWhileAiming, int weaponId, time_t time);
	void			startFeature_9(int targetIndex);
	void			stopFeature_9(int targetIndex);

	void			tryStartFeature_10(int targetIndex);
	void			startFeature_10(int targetIndex);
	void			updateFeature_10(int targetIndex, bool hit, bool reset);
	void			stopFeature_10(int targetIndex, bool hit, bool reset);
	void			resetFeature_10(int targetIndex);
	void			addFeature_10(int shotsUntilHit, int weaponId, time_t time);

	void			addFeature_11(float angle, INT64 aimTime, int weaponId, time_t time);
	void			startFeature_11(int targetIndex, float angle);
	void			stopFeature_11(int targetIndex);
	void			resetFeature_11(int targetIndex);

	void			addFeature_14(float div, int weaponId, time_t time);
	void			startFeature_14(int targetIndex, float div);
	void			resetFeature_14(int targetIndex);

	void			initProfilerValues();
	int				getClosestBoneId(int boneId, float maxDistance = 8.0f);
	int				getClosestBoneId_debug(int boneId, float maxDistance = 8.0f);
	float			getAngleToPlayer(int targetIndex);
	int				getClosestPlayerIndex(float& distance);
	int				getPlayerIndexInCrosshair();
	int				getPlayerIndexInFieldOfView();
	bool			isBoneInFieldOfView(Vector &current, int targetIndex, int bone, float fov);
	bool			canHit(Vector current, Vector target, float distance, float max);

	
	float			getFieldOfView(Vector angle, Vector src, Vector dst);
	Vector			calculateAngles(Vector src, Vector dst);
	Vector			vectorToAngles(const Vector& direction);
	void			anglesToVector(Vector angle, Vector& vector);
	float			vectorNormalize(Vector& vector);
	void			clampAngles(Vector& angles);
	float			angleNormalize(float angle);

	//utils
	int				checkPlayerHits();
	void			checkPlayerHealth(int targetIndex, int& health, int& lastHealth, int& healthDifference);
	int				getVisibleEnemies(int targetIndex);
	bool			isHitCritical(int hitGroupId);
	bool			isHitCritical_Bone(int boneId);
	float			randomFloat(float a, float b);
	INT64			getCurrentTimeInMilliseconds();

private:
	json			profile;

	bool needResetForValues = false;

	struct localProfile_struct {
		int		index;

		int		ammo;
		int		weaponId;
		int		weaponType;
		int		visibleEnemies;

		int		movingMouseToTargetIndex;

		int		lastShotsNum;
		int		totalHitsOnServer;
		int		lastTotalHitsOnServer;

		int		totalHitsSinceLastDeath;
		int		hitsWhileMoving;

		int		lastTargetForFeature10;

		float	lastAngleToTarget;
	};
	localProfile_struct localProfile;

private:
	void		addHit(int targetIndex, int boneId, bool critical, int damage, time_t time);
	void		resetAllHits(int targetIndex);

	void		onPlayerAimOnTarget(int targetIndex);

	void		isPlayerMovingMouseToTarget(int targetIndex, float distance);
	void		onStartMovingMouseToTarget(int targetIndex);
	void		onStopMovingMouseToTarget(int targetIndex);
	void		onMovingMouseToTarget(int targetIndex, float distance);
	void		recordSpiralAimAngles(int targetIndex);
	void		stopRecordSpiralAimAngles(int targetIndex);
	void		resetRecordSpiralAimAngles(int targetIndex);
	Vector		getLinearAimbotAngles(int targetIndex, int boneIdToAimAt);
	void		addFeature_13(float offsetX, float offsetY, int aboveCounter, int belowCounter, int weaponId, time_t time);

	void		calculateRecoilSuppression(int targetIndex, float angleToTarget);
	void		stopRecoilSuppressionCalculation(int targetIndex);
	void		resetRecoilSuppressionCalculation(int targetIndex);
	void		printFeature12(Vector angles[]);
	void		addFeature_12(Vector angles[], int weaponId, time_t time);

	void		resetPlayerAfterDeath(int targetIndex);
	bool		playerIndexInvalid(int targetIndex);
};