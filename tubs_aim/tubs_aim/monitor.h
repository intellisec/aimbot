#pragma once

#include "dllmain.h"

#define	MAX_INACCURACY_OFFSET		25
#define	INACCURACY_OFFSET_STEP		1

class Monitor
{
public:
	void			start();

private:
	bool			playerIndexInvalid(int targetIndex);
	bool			isBoneInFieldOfView(Vector &current, int targetIndex, int boneIndex, float fov);

	int				checkPlayerHits();
	int				getClosestPlayerIndex(float &bestDistance);
	int				getPlayerIndexInCrosshair();
	int				getPlayerIndexInFieldOfView();

	float			getAngleToPlayer(int targetIndex);

	void			initValues();
	void			checkPlayerHealth(int targetIndex, int& health, int& lastHealth, int& healthDifference);
	void			checkHitAccuracy(int weaponId);
	void			onPlayerWeaponShot(int playerIndex, int targetIndex = -1);
	void			onPlayerGetKilled(int killerIndex, int victimIndex, int damage);

	void			tryStartFirstHitAccuracyRecord(int targetIndex);
	void			startFirstHitAccuracyRecord(int targetIndex, int weaponId);
	void			updateFirstHitAccuracyRecord(int targetIndex, bool hit, bool reset);
	void			stopFirstHitAccuracyRecord(int targetIndex, bool hit, bool reset);
	void			resetCurrentFirstHitAccuracyRecord(int targetIndex);
	void			updateFirstHitAccuracy(int shotsUntilHit, int weaponId);

private:
	struct localMonitor {
		int			playerIndex;
		int			hitCounter[65];
		int			shotCounter[65];
		int			nextCheck[65];
		int			lastAmmo;
		int			lastWeapon;
		int			totalHitsOnServer;
		int			lastTotalHitsOnServer;
		int			lastShotsNum;

		int			lastHealth[65];
		int			nextToLastHealth[65];

		bool		isResetAfterDeathNeeded[65];
	}localMonitor;

	struct firstHitAccuracyMonitor {
		bool		recordStarted[65];
		bool		recordSaved[65];
		int			shotCounter[65];
		int			weaponId;
		int			lastTarget;

		float		firstHitAccuracy[65];
		int			firstHitAccuracyRecordCounter;
	}firstHitAccuracyMonitor;

	bool needResetForValues = false;
};
