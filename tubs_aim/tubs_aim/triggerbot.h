#pragma once

#include "dllmain.h"

#define MAX_ANGLE_FOR_FIRST_ENCOUNTER		25.0f

#define MIN_LEGIT_DELAY						1
#define MAX_LEGIT_DELAY						100

class Triggerbot
{
public:
	void		start();

private:
	int			getPlayerIndexAimingOn();
	int			getPlayerInFieldOfView();
	int			randomInt(int a, int b);

	INT64		getCurrentTimeInMilliseconds();

	float		getAngleToPlayer(int targetIndex);
	float		getFieldOfView(Vector viewAngles, Vector src, Vector dst);
	float		transformNoise(float noise);
	float		randomFloat(float a, float b);

	Vector		calculateAngles(Vector src, Vector dst);

	bool		isBoneInFieldOfView(Vector &current, int m_target, int bone, float fov);
	bool		canHit(Vector current, Vector target, float distance, float max);

private:
	struct triggerbotSettings {
		INT64	timeToKill[65] = {0};
	}triggerbotSettings;

};
