#include "stdafx.h"
#include "dllmain.h"

/**
	Entry point from the monitor module.

	Here the current game state is monitored.
	If adjustments need to be made to the assistive systems, this will happen from here.

	@return nothing
*/
void Monitor::start() {
	try {
		Monitor::initValues();
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (config->general.panic_mode)
				continue;

			if (!config->general.aimbot_enabled)
				continue;

			if (!Helper::getInstance().isCSGOWindowActive())
				continue;

			if (!game->isInGame()) {
				if (needResetForValues) {
					needResetForValues = false;
					Monitor::initValues();
				}
				continue;
			}

			if (!game->isTeamSelected())
				continue;

			if (!localEntity.isValidForProfiler())
				continue;

			needResetForValues = true;

			int hitDifference = Monitor::checkPlayerHits();
			int shotDifference = (localMonitor.lastAmmo - localEntity.getActiveWeaponAmmo());
			int shotsNum = localEntity.getShotsFired();

			if (shotsNum != localMonitor.lastShotsNum) {
				shotDifference = shotsNum - localMonitor.lastShotsNum;
				if (shotDifference > 0) {
					Monitor::onPlayerWeaponShot(localMonitor.playerIndex);
				}
				localMonitor.lastShotsNum = shotsNum;
			}

			if (localEntity.getActiveWeapon() != localMonitor.lastWeapon) {
				//switch weapon
				shotDifference = 0;
			} else {
				if (localMonitor.lastAmmo < localEntity.getActiveWeaponAmmo()) {
					//reload weapon
					shotDifference = localMonitor.lastAmmo;
				}
			}
			localMonitor.lastWeapon = localEntity.getActiveWeapon();
			localMonitor.lastAmmo = localEntity.getActiveWeaponAmmo();

			if (shotDifference > 0 || hitDifference > 0) {
				localMonitor.hitCounter[localMonitor.lastWeapon] += hitDifference;
				localMonitor.shotCounter[localMonitor.lastWeapon] += shotDifference;
				if (localMonitor.shotCounter[localMonitor.lastWeapon] > localMonitor.nextCheck[localMonitor.lastWeapon]) {
					Monitor::checkHitAccuracy(localMonitor.lastWeapon);
				}
			}

			for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
				std::this_thread::yield();

				if (!entityList[playerIndex].isValidForProfiler())
					continue;

				//get players health
				int entityHealth = 100,
					entityLastHealth = 100,
					healthDifference = 0;

				Monitor::checkPlayerHealth(playerIndex, entityHealth, entityLastHealth, healthDifference);

				//check if player is dead
				if ((entityList[playerIndex].isDead() || entityList[playerIndex].getHealth() < 1)) {
					if (localMonitor.isResetAfterDeathNeeded[playerIndex]) {
						int killerIndex = entityList[playerIndex].getLastKillerIndex(true);

						//trigger event 'onPlayerGetKilled'
						Monitor::onPlayerGetKilled(killerIndex, playerIndex, 0);

						localMonitor.isResetAfterDeathNeeded[playerIndex] = false;

						for (int j = 0; j < game->getMaxClients(); j++) {
							Monitor::resetCurrentFirstHitAccuracyRecord(j);
						}
					}
					continue;
				} else {
					if (!localMonitor.isResetAfterDeathNeeded[playerIndex]) {
						localMonitor.isResetAfterDeathNeeded[playerIndex] = true;
					}
				}
			}

		}
	} catch (...) {
		memory->debuglog("Monitor -> main");
	}
}

/**
	Sets the variables from the monitor to default values.

	@return nothing
*/
void Monitor::initValues() {
	for (int i = 0; i < 65; i++) {
		localMonitor.hitCounter[i] = 0;
		localMonitor.shotCounter[i] = 0;
		localMonitor.nextCheck[i] = 15;
	}
	localMonitor.lastAmmo = 0;
	localMonitor.lastWeapon = 0;
	localMonitor.totalHitsOnServer = 0;
	localMonitor.lastTotalHitsOnServer = 0;
	localMonitor.lastShotsNum = 0;
}

/**
	Checks the hit accuracy of the local player. 

	@param weaponId the id of the weapon to be checked
	@return nothing
*/
void Monitor::checkHitAccuracy(int weaponId) {
	if (localMonitor.shotCounter[weaponId] > 0) {
		//check hit accuracy
		float accuracy = static_cast<float>(localMonitor.hitCounter[weaponId] / localMonitor.shotCounter[weaponId]);
		if (accuracy > config->weapons[weaponId].aimbot.max_hit_accuracy) {
			if (config->weapons[weaponId].aimbot.inaccuracy_offset < MAX_INACCURACY_OFFSET) {
				config->weapons[weaponId].aimbot.inaccuracy_offset += INACCURACY_OFFSET_STEP;
			}
		} else {
			if (config->weapons[weaponId].aimbot.inaccuracy_offset > 0) {
				config->weapons[weaponId].aimbot.inaccuracy_offset -= INACCURACY_OFFSET_STEP;
			}
		}
		
		//check first hit accuracy
		if (firstHitAccuracyMonitor.firstHitAccuracy[weaponId] > config->weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot) {
			if (config->weapons[weaponId].aimbot.inaccuracy_offset_for_first_shot < MAX_INACCURACY_OFFSET) {
				config->weapons[weaponId].aimbot.inaccuracy_offset_for_first_shot += INACCURACY_OFFSET_STEP;
			}
		} else {
			if (config->weapons[weaponId].aimbot.inaccuracy_offset_for_first_shot > 0) {
				config->weapons[weaponId].aimbot.inaccuracy_offset_for_first_shot -= INACCURACY_OFFSET_STEP;
			}
		}

		//set next check
		localMonitor.nextCheck[weaponId] = localMonitor.shotCounter[weaponId] + 3;
	}
}

/**
	Reads the most recent hit values (total hits, last total hits, hit difference) of the local player.

	@return the hit difference between now and the last check
*/
int Monitor::checkPlayerHits() {
	int hitDifference = 0;
	localMonitor.totalHitsOnServer = localEntity.getTotalHitsOnServer(true);
	if (localMonitor.totalHitsOnServer > 0 && localMonitor.totalHitsOnServer > localMonitor.lastTotalHitsOnServer) {
		hitDifference = localMonitor.totalHitsOnServer - localMonitor.lastTotalHitsOnServer;
	}
	localMonitor.lastTotalHitsOnServer = localMonitor.totalHitsOnServer;
	return hitDifference;
}

/**
	Invoked when a player kills an enemy.

	@param killerIndex the index of the killer
	@param victimIndex the index of the victim
	@param damage the damage of the last hit
	@return nothing
*/
void Monitor::onPlayerGetKilled(int killerIndex, int victimIndex, int damage) {
	if (Monitor::playerIndexInvalid(killerIndex))
		return;

	if (Monitor::playerIndexInvalid(victimIndex))
		return;

	if (killerIndex == victimIndex)
		return;

	if (killerIndex == localMonitor.playerIndex) {
		//Trigger the event onPlayerWeaponShot here. In the main class function it is not triggerd, because the function GetShotsFired returns 0, if the player is dead.
		Monitor::onPlayerWeaponShot(killerIndex, victimIndex);
	} else if (victimIndex == localMonitor.playerIndex) {
		for (int index = 0; index < game->getMaxClients(); index++) {
			Monitor::resetCurrentFirstHitAccuracyRecord(index);
		}
	}
}

/**
	Invoked when a player fires a weapon. (actually just works for local player)

	@param playerIndex the index of the player
	@param targetIndex the index of the target player
	@return nothing
*/
void Monitor::onPlayerWeaponShot(int playerIndex, int targetIndex) {
	if (Monitor::playerIndexInvalid(playerIndex))
		return;

	//check if the player is the local player; if the functionality is extended then this does not have to be the case.
	if (localMonitor.playerIndex == playerIndex) {
		//check if targetIndex is already given; This is the case when a player dies and the event is called.
		if (Monitor::playerIndexInvalid(targetIndex)) {
			//get the closest player and the distance to him.
			float distance = 100.f;
			int closestPlayerIndex = Monitor::getClosestPlayerIndex(distance);

			//check if a player has been found nearby.
			if (!Monitor::playerIndexInvalid(closestPlayerIndex)) {
				if (distance < START_FOV_FOR_FEATURE_10) {
					//start the first hit accuracy record
					Monitor::tryStartFirstHitAccuracyRecord(closestPlayerIndex);
				}
			}

			//find the player you aim for. (fov based)
			int targetIndex = Monitor::getPlayerIndexInCrosshair();
			if (targetIndex == -1) {
				targetIndex = Monitor::getPlayerIndexInFieldOfView();
			}

			//check if a player has been found and if he is still alive.
			if (!Monitor::playerIndexInvalid(targetIndex) && !entityList[targetIndex].isDead()) {
				//calculate the deviation to the player and the life values of the player.
				float	angleToTarget = Monitor::getAngleToPlayer(targetIndex);

				int		entityHealth = entityList[targetIndex].getHealth(),
						healthDifference = localMonitor.nextToLastHealth[targetIndex] - entityHealth;

				//check if the deviation to the player is less than 4.f
				if (angleToTarget < 4.f || (healthDifference > 0 && angleToTarget < 10.f)) {
					//update the first hit accuracy record
					Monitor::updateFirstHitAccuracyRecord(targetIndex, true, false);
				} else {
					//Console::getInstance().println("onPlayerWeaponShot | NO HIT | healthDifference: " + std::to_string(healthDifference) + " angleToTarget: " + std::to_string(angleToTarget), red, white);
				}
			} else {
				//Console::getInstance().println("onPlayerWeaponShot | NO TARGET IN FOV | targetIndex: " + std::to_string(targetIndex), red, white);
			}
		} else {
			//check if the player is dead.
			if (entityList[targetIndex].isDead()) {
				//try to start the first hit accuracy record (for one click kill; e.g. headshot)
				Monitor::tryStartFirstHitAccuracyRecord(targetIndex);
				//update the first hit accuracy record
				Monitor::updateFirstHitAccuracyRecord(targetIndex, true, true);
			}
		}
	}
}

/**
	Find the index of the player closest to the local player.
	Depending on where the local player is currently aiming.

	@return the index of the player closest to the local player, otherwise -1
*/
int Monitor::getClosestPlayerIndex(float &bestDistance) {
	Vector viewAngles, punchAngles;
	game->getViewAngles(viewAngles);

	punchAngles = localEntity.getPunchAngles();
	punchAngles.z = 0.0f;
	viewAngles -= punchAngles * 2;

	const int numBones = 3;
	int bones[numBones] = { 0, 6, 8 };

	int closestPlayerIndex = -1;

	for (int index = 0; index < game->getMaxClients(); index++) {
		if (entityList[index].getTeamNum() == localEntity.getTeamNum())
			continue;

		if (!entityList[index].isValid())
			continue;

		for (int boneId = 0; boneId < numBones; boneId++) {
			float fov = pMath.getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[index].getBonePosition(bones[boneId]));
			if (fov < bestDistance) {
				bestDistance = fov;
				closestPlayerIndex = index;
			}
		}

	}
	return closestPlayerIndex;
}

/**
	Calculates the angle from the local player to the player with the given index.

	@param targetIndex the index of the target player
	@return the angle to the target player
*/
float Monitor::getAngleToPlayer(int targetIndex) {
	//Console::getInstance().appendToLogFile("getAngleToPlayer");
	if (Monitor::playerIndexInvalid(targetIndex))
		return NULL;

	if (!entityList[targetIndex].isValid())
		return NULL;

	if (!entityList[targetIndex].isVisible())
		return NULL;

	Vector viewAngles, punchAngles;
	game->getViewAngles(viewAngles); //Gets the direction the player is looking in.
	punchAngles = localEntity.getPunchAngles();
	punchAngles.z = 0.0f;
	viewAngles -= punchAngles * 2;

	const int numBones = 4;
	int bones[numBones] = { -1, 0, 6, 8 };
	float bestFov = 1000.f;
	for (int boneId = 0; boneId < numBones; boneId++) {
		float tempFov = pMath.getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[targetIndex].getBonePosition(boneId));
		if (tempFov < bestFov) {
			bestFov = tempFov;
		}
	}

	return bestFov;
}

/**
	Search for the index of a player targeted.

	@return the index of the player targeted, otherwise -1
*/
int Monitor::getPlayerIndexInCrosshair() {
	int playerIndexInCrosshair = localEntity.getCrosshairEnt() - 1;

	if (Monitor::playerIndexInvalid(playerIndexInCrosshair))
		return -1;

	if (entityList[playerIndexInCrosshair].getHealth() > 0 && entityList[playerIndexInCrosshair].getTeamNum() != localEntity.getTeamNum()) {
		return playerIndexInCrosshair;
	}
	return -1;
}

/**
	Search for the index of a player in the field of view.

	@return the index of the player in the field of view, otherwise -1
*/
int Monitor::getPlayerIndexInFieldOfView() {
	Vector viewAngles, punchAngles;
	game->getViewAngles(viewAngles);

	punchAngles = localEntity.getPunchAngles();
	punchAngles.z = 0.0f;
	viewAngles -= punchAngles * 2;

	for (int index = 0; index < game->getMaxClients(); index++) {
		if (entityList[index].getTeamNum() == localEntity.getTeamNum())
			continue;

		if (!entityList[index].isValid())
			continue;

		if (isBoneInFieldOfView(viewAngles, index, 8, 3)) {
			return index;
		}

		if (isBoneInFieldOfView(viewAngles, index, 6, 5)) {
			return index;
		}

		if (isBoneInFieldOfView(viewAngles, index, 4, 4)) {
			return index;
		}

		if (isBoneInFieldOfView(viewAngles, index, -1, 4)) {
			return index;
		}
	}
	return -1;
}

/**
	Checks if the bone (body part) of a target player is in the field of view of the local player.

	@param current the current ViewAngles from the local player
	@param targetIndex the index of the target player
	@param boneIndex the index of the target bone
	@param fov the field of view in which the target must be located. (Permissible deviation)
	@return true if the bone (body part) is in fov, otherwise false
*/
bool Monitor::isBoneInFieldOfView(Vector &current, int targetIndex, int boneIndex, float fov) {
	//Console::getInstance().appendToLogFile("isBoneInFieldOfView");
	if (Monitor::playerIndexInvalid(targetIndex))
		return false;

	auto direction = entityList[targetIndex].getBonePosition(boneIndex) - localEntity.getEyePosition();
	pMath.vectorNormalize(direction);

	auto target = pMath.vectorToAngles(direction);
	pMath.clampAngles(target);

	Vector v_dist = localEntity.getOrigin() - entityList[targetIndex].getOrigin();
	float distance = v_dist.Length();

	if (pMath.canHit(current, target, distance, fov)) {
		return true;
	}

	return false;
}

/**
	Check if the given player index is invalid.

	@param targetIndex the index to be tested
	@return true if the index is invalid, otherwise false
*/
bool Monitor::playerIndexInvalid(int targetIndex) {
	//Console::getInstance().appendToLogFile("playerIndexInvalid");
	return (targetIndex < 0 || targetIndex > MAX_PLAYERS_NUM);
}

/**
	Reads the most recent life values (current health, last health, health difference) of the player.

	@param targetIndex the index of the player
	@param health reference to the health value
	@param lastHealth reference to the last health value
	@param healthDifference reference to the health difference value
	@return nothing
*/
void Monitor::checkPlayerHealth(int targetIndex, int& health, int& lastHealth, int& healthDifference) {
	//Console::getInstance().appendToLogFile("checkPlayerHealth");
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	//get players health values and calculate healthDifference
	health = entityList[targetIndex].getHealth();
	lastHealth = localMonitor.lastHealth[targetIndex];
	healthDifference = lastHealth - health;

	//store players last health values
	localMonitor.lastHealth[targetIndex] = health;
	localMonitor.nextToLastHealth[targetIndex] = lastHealth;
}

/**
	Try to start the recording of the first hit accuracy.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Monitor::tryStartFirstHitAccuracyRecord(int targetIndex) {
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("tryStartFirstHitAccuracyRecord targetIndex: " + std::to_string(targetIndex) + " firstHitAccuracyMonitor.lastTarget: " + std::to_string(firstHitAccuracyMonitor.lastTarget), white, black);
	if (firstHitAccuracyMonitor.lastTarget != -1 && targetIndex != firstHitAccuracyMonitor.lastTarget) {
		if (entityList[firstHitAccuracyMonitor.lastTarget].isDead()) {
			//do nothing and wait for stop recording of first hit accuracy.
		} else {
			//reset the values of the first hit accuracy record from both players
			Monitor::resetCurrentFirstHitAccuracyRecord(targetIndex);
			Monitor::resetCurrentFirstHitAccuracyRecord(firstHitAccuracyMonitor.lastTarget);
			//start the recording of the first hit accuracy.
			Monitor::startFirstHitAccuracyRecord(targetIndex, localMonitor.lastWeapon);
		}
	} else {
		//start the recording of the first hit accuracy.
		Monitor::startFirstHitAccuracyRecord(targetIndex, localMonitor.lastWeapon);
	}
}

/**
	Starts the recording of the first hit accuracy.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Monitor::startFirstHitAccuracyRecord(int targetIndex, int weaponId) {
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	if (!firstHitAccuracyMonitor.recordStarted[targetIndex] && !firstHitAccuracyMonitor.recordSaved[targetIndex]) {
		firstHitAccuracyMonitor.lastTarget = targetIndex;
		firstHitAccuracyMonitor.weaponId = weaponId;
		firstHitAccuracyMonitor.recordStarted[targetIndex] = true;
		firstHitAccuracyMonitor.recordSaved[targetIndex] = false;
		firstHitAccuracyMonitor.shotCounter[targetIndex] = 0;
		//Console::getInstance().println("startFirstHitAccuracyRecord targetIndex: " + std::to_string(targetIndex), white, black);
	} else if (firstHitAccuracyMonitor.recordStarted[targetIndex] && !firstHitAccuracyMonitor.recordSaved[targetIndex]) {
		Monitor::updateFirstHitAccuracyRecord(targetIndex, false, false);
	} else {
		//its not an error; the first hit accuracy has already been recorded and the player is still alive.
		//Console::getInstance().println("startFirstHitAccuracyRecord error targetIndex: " + std::to_string(targetIndex) + " started: " + std::to_string(firstHitAccuracyMonitor.recordStarted[targetIndex]) + " saved: " + std::to_string(firstHitAccuracyMonitor.recordSaved[targetIndex]), red, white);
	}
}

/**
	Updates the recording of the first hit accuracy.

	@param targetIndex the index of the player for which the recording is updated.
	@return nothing
*/
void Monitor::updateFirstHitAccuracyRecord(int targetIndex, bool hit, bool reset) {
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	if (firstHitAccuracyMonitor.recordStarted[targetIndex] && !firstHitAccuracyMonitor.recordSaved[targetIndex]) {
		firstHitAccuracyMonitor.shotCounter[targetIndex] ++;
		if (hit) {
			Monitor::stopFirstHitAccuracyRecord(targetIndex, hit, reset);
		}
		//Console::getInstance().println("updateFirstHitAccuracyRecord targetIndex: " + std::to_string(targetIndex) + " hit: " + std::to_string(hit), white, black);
	}
}

/**
	Stops the recording of the first hit accuracy.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Monitor::stopFirstHitAccuracyRecord(int targetIndex, bool hit, bool reset) {
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	if (firstHitAccuracyMonitor.recordStarted[targetIndex] && !firstHitAccuracyMonitor.recordSaved[targetIndex]) {
		firstHitAccuracyMonitor.recordSaved[targetIndex] = true;
		firstHitAccuracyMonitor.recordStarted[targetIndex] = false;
		if (hit) {
			Monitor::updateFirstHitAccuracy(firstHitAccuracyMonitor.shotCounter[targetIndex], firstHitAccuracyMonitor.weaponId);
		}
		//Console::getInstance().println("stopCurrentFirstHitAccuracyRecord targetIndex: " + std::to_string(targetIndex) + " hit: " + std::to_string(hit) + " reset: " + std::to_string(reset), white, black);
	}
	if (reset) {
		//reset current first hit accuracy record for all players here
		for (int index = 0; index < game->getMaxClients(); index++) {
			Monitor::resetCurrentFirstHitAccuracyRecord(index);
		}
	}
}

/**
	Resets the values for the current first hit accuracy record.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Monitor::resetCurrentFirstHitAccuracyRecord(int targetIndex) {
	if (Monitor::playerIndexInvalid(targetIndex))
		return;

	if (targetIndex >= 0 && targetIndex < sizeof(firstHitAccuracyMonitor.recordSaved)) {
		firstHitAccuracyMonitor.recordSaved[targetIndex] = false;
		firstHitAccuracyMonitor.recordStarted[targetIndex] = false;
		firstHitAccuracyMonitor.shotCounter[targetIndex] = 0;
		//Console::getInstance().println("resetCurrentFirstHitAccuracyRecord targetIndex: " + std::to_string(targetIndex), white, black);
	}
}

/**
	Updates the hit accuracy for the first hit.

	@param shotsUntilHit the number of shot until the first hit
	@param weaponId the id of the weapon
	@return nothing
*/
void Monitor::updateFirstHitAccuracy(int shotsUntilHit, int weaponId) {
	float hitAccuracy = static_cast<float>(1.0f / shotsUntilHit);
	float tempHitAccuracy = static_cast<float>(firstHitAccuracyMonitor.firstHitAccuracy[weaponId] * firstHitAccuracyMonitor.firstHitAccuracyRecordCounter);
	firstHitAccuracyMonitor.firstHitAccuracyRecordCounter++;
	firstHitAccuracyMonitor.firstHitAccuracy[weaponId] = (tempHitAccuracy + hitAccuracy) * (1.0f / firstHitAccuracyMonitor.firstHitAccuracyRecordCounter);
	Console::getInstance().println("updateFirstHitAccuracy localMonitor.firstHitAccuracy: " + std::to_string(firstHitAccuracyMonitor.firstHitAccuracy[weaponId]) + " weaponId: " + std::to_string(weaponId), white, black);
}