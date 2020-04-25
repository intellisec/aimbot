// WriteProcessMemory vs. MoveMouse
// https://www.unknowncheats.me/forum/counterstrike-global-offensive/120544-aimbot-angle-writeprocessmemory-2.html

#pragma once

#include "dllmain.h"

#define MAX_ANGLE_FOR_FIRST_ENCOUNTER		25.0f

#define	INACCURACY_OFFSET_PAUSE_STEP		10
#define	INACCURACY_OFFSET_ANGLES_STEP		0.0001f

#define	INACCURACY_ACTION_PAUSE				0
#define	INACCURACY_ACTION_ANGLES			1
#define INACCURACY_ACTION_STARTBULLET		2

class Aimbot {
	public:
		void		start();

	private:
		void		getAimbotSettings();
		void		dropSelectedTarget();
		void		moveMouseToPosition(float x, float y);

		int			getClosestBoneId(int targetIndex, float maxDistance);
		int			getPlayerClosestToCrosshair(int boneId);
		int			getBoneClosestToCrosshair();
		int			randomInt(int a, int b);

		INT64		getCurrentTimeInMilliseconds();

		bool		isNextShootAllowed();
		bool		isDelayNeeded();

		Vector		getAimbotAngles();
		Vector		getAimbotMousePosition();
		Vector		calculateAngles(Vector src, Vector dst);

		float		getFieldOfView(Vector angle, Vector src, Vector dst);
		float		getAngleToPlayer(int targetIndex);
		float		transformNoise(float noise);
		float		randomFloat(float a, float b);
		float		getSmoothValue(Vector &angles);

	private:
		int			selectedTargetIndex = -1;
		int			lastSelectedTargetIndex = -1;
		int			selectedTargetBoneId = -1;

		Vector		anglesToAimAt;
		Vector		positionToMoveTowards;

		struct aimbotSettings {
			bool	saveTargetAim = false;
			bool	faceitAim = false;
			bool	compensateVelocity = true;

			int		currentWeaponId = -1;
			int		startBullet = 0;
			int		endBullet = 30;
			int		delayAfterKill = 0;
			int		delayToStopAiming = 0;
			int		delayToStartAiming = 0;
			int		inaccuracyAction = -1;
			int		inaccuracyPause = 0;

			INT64	weaponEmptyTime = 0;
			INT64	startToAimTime = 0;

			float	divergenceIfSpotted = 20.0f;
			float	fov = 2.5;
			float	recoilScale = 2.0f;
			float	smooth = 1.0f;
			float	chanceForClosestBone = 0.0f;
			float	aimTimePerDegree = 0.0f;
			float	firstHitIsCriticalAccuracy = 0.0f;
			float	hitAccuracyWhileMoving = 0.0f;
			float	inaccuracyOffset = 0.0f;
			float	spiralAimX = 0.0f;
			float	spiralAimY = 0.0f;
			float	angleFirstEncounter[65];
		}aimbotSettings;

};
