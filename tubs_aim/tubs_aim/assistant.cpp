#include "stdafx.h"
#include "dllmain.h"

/**
	This is the entry point of the Assistant. Here the assisting processes are executed.

	@return nothing
*/
void Assistant::start() {
	try {
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (config->general.panic_mode)
				continue;

			if (!Helper::getInstance().isCSGOWindowActive())
				continue;

			if (!config->general.aimbot_enabled)
				continue;

			if (!config->weapons[localEntity.getActiveWeapon()].aimbot.enabled)
				continue;

			if (!game->isInGame())
				continue;

			if (localEntity.isDead())
				continue;

			if (localEntity.isFlashed())
				continue;

			if (GetAsyncKeyState(config->keys.aimbot_hold)) {
				continue;
			}

			if (localEntity.isActiveWeaponPrimary()) {
				if (!localEntity.getActiveWeaponAmmo()) {
					if (!assistantSettings.triedToSwitch) {
						Assistant::getAssistantSettings();
						assistantSettings.triedToSwitch = true;
						if (randomFloat(0.0f, 1.0f) < assistantSettings.chanceToSwitch) {
							Sleep(assistantSettings.timeToSwitch);
							Assistant::switchToSecondaryWeapon();
						}
					}
				} else {
					assistantSettings.triedToSwitch = false;
				}
			}

		}
	} catch (...) {
		memory->debuglog(__FILE__);
	}
}

/**
	Switch to the secondary weapon.

	@return nothing
*/
void Assistant::switchToSecondaryWeapon() {
	if (config->keys.ingame_switch_weapon != 0 && localEntity.isActiveWeaponPrimary() && !localEntity.getActiveWeaponAmmo()) {
		INPUT Input = { 0 };
		WORD vkey = config->keys.ingame_switch_weapon;

		ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));

		Sleep(1);

		ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));
	}
}

/**
	Determines the settings for the assisting processes.

	@return nothing
*/
void Assistant::getAssistantSettings() {
	float noise = 1.0f;
	int weaponId = localEntity.getActiveWeapon();

	noise = Assistant::transformNoise(config->aimbot.time_to_switch_noise);
	assistantSettings.timeToSwitch = static_cast<int>(config->weapons[weaponId].aimbot.time_to_switch * noise);

	noise = Assistant::transformNoise(config->aimbot.chance_to_switch_noise);
	assistantSettings.chanceToSwitch = (config->weapons[weaponId].aimbot.chance_to_switch * noise);
}

/**
	Transforms the noise in percent into a float.

	@param noise the noise in percent
	@return the noise as a float
*/
float Assistant::transformNoise(float noise) {
	return 1.0f + (randomFloat(-noise, noise) / 100);
}

/**
	Calculates a random float between two floats.

	@param a min float
	@param b max float
	@return the random float
*/
float Assistant::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

/**
	Calculates a random int between two int.

	@param a min int
	@param b max int
	@return the random int
*/
int Assistant::randomInt(int a, int b) {
	std::random_device random;
	std::mt19937 randomGenerator(random());
	std::uniform_int_distribution<int> randomDistribute(a, b);
	return randomDistribute(randomGenerator);
}

/**
	Calculates the elapsed time in milliseconds since 01/01/1970.

	@return the calculated time in milliseconds
*/
INT64 Assistant::getCurrentTimeInMilliseconds() {
	auto now = system_clock::now();
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
	return ms;
}