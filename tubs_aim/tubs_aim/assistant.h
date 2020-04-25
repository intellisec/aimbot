#pragma once

#include "dllmain.h"

class Assistant
{
public:
	void		start();

private:
	void		switchToSecondaryWeapon();
	void		getAssistantSettings();

	float		transformNoise(float noise);
	float		randomFloat(float a, float b);

	int			randomInt(int a, int b);

	INT64		getCurrentTimeInMilliseconds();

private:
	struct assistantSettings {
		int		timeToSwitch = 0;

		float	chanceToSwitch = 0.0f;

		bool	triedToSwitch = false;
		bool	switchedToSecondaryWeapon = false;
	}assistantSettings;
};