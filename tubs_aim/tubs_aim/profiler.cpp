#include "stdafx.h"
#include "dllmain.h"

FeatureData* featureData = new FeatureData[MAX_PLAYERS_NUM + 1]();
EntityProfile* entityProfile = new EntityProfile[MAX_PLAYERS_NUM + 1]();

/**
	Entry point from the profiler module.
	Runs in a loop while the main program is alive.

	The loop repeats every 5 milliseconds. (max: 1000ms/128ticks = 7.8125 ~ 7ms)
	The CS:GO servers have 128 ticks per second. 
	We have to make at least one call between each tick.

	All required values for the profiler are recorded and (pre-)calculated here.

	@return nothing
*/
void Profiler::start() {
	//Console::getInstance().appendToLogFile("start");
	try {
		Profiler::readFromProfileFile();
		Profiler::initProfilerValues();

		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));

			if (config->general.panic_mode)
				continue;

			if (!config->general.profiler_enabled)
				continue;

			if (!Helper::getInstance().isCSGOWindowActive())
				continue;

			if (!game->isInGame()) {
				if (needResetForValues) {
					needResetForValues = false;
					Profiler::initProfilerValues();
				}
				continue;
			}

			if (!game->isTeamSelected())
				continue;

			if (!localEntity.isValidForProfiler())
				continue;

			needResetForValues = true;

			localProfile.index = localEntity.getIndex()-1;

			//Console::getInstance().logln(LogLevel::LOG_ERROR, "initProfilerValues " + std::to_string(localProfile.index));

			localProfile.visibleEnemies = Profiler::getVisibleEnemies(localProfile.index);
			localProfile.weaponId = localEntity.getActiveWeapon();
			localProfile.ammo = localEntity.getActiveWeaponAmmo();

			if (!localEntity.isDead() && localEntity.isActiveWeaponValid()) {
				if (!localEntity.isActiveWeaponPistol()) {
					if (localProfile.ammo < 1 && !featureData[localProfile.index].f6.started) {
						//The player has a primary weapon in his hand and the ammunition is empty.
						Profiler::startFeature_6(localProfile.index);
					} else if (localProfile.ammo > 0 && featureData[localProfile.index].f6.started) {
						//The player reloads the primary weapon after the primary weapon was empty.
						Profiler::stopFeature_6(localProfile.index, true);
					}
				} else {
					if (featureData[localProfile.index].f6.started && !featureData[localProfile.index].f6.saved) {
						//The player picked up a secondary weapon after the primary weapon was empty.
						Profiler::stopFeature_6(localProfile.index, false);
					}
				}
			}

			int hitDifference = Profiler::checkPlayerHits();
			int shotsNum = localEntity.getShotsFired();
			int shotDifference = 0;
			//Console::getInstance().logln(LogLevel::LOG_ALL, "shotsNum " + std::to_string(shotsNum));
			
			if (shotsNum != localProfile.lastShotsNum) {
				shotDifference = shotsNum - localProfile.lastShotsNum;
				if (shotDifference > 0) {
					Profiler::onPlayerWeaponShot(localProfile.index);
				}
				localProfile.lastShotsNum = shotsNum;
			}
			
			//get the target player in the fov of the local player
			int playerInFOV = Profiler::getPlayerIndexInCrosshair();
			if (playerInFOV == -1) {
				playerInFOV = Profiler::getPlayerIndexInFieldOfView();
			}

			for (int iIndex = 0; iIndex <= game->getMaxClients(); iIndex++) {
				//std::this_thread::yield();

				if (!entityList[iIndex].isValidForProfiler())
					continue;

				//get players health
				int entityHealth = 100,
					entityLastHealth = 100,
					healthDifference = 0;

				Profiler::checkPlayerHealth(iIndex, entityHealth, entityLastHealth, healthDifference);

				//check if player is dead
				if ((entityList[iIndex].isDead() || entityList[iIndex].getHealth() < 1)) {
					if (entityProfile[iIndex].isResetAfterDeathNeeded) {
						int killerIndex = entityList[iIndex].getLastKillerIndex(true);

						//Console::getInstance().println("iIndex			" + std::to_string(iIndex));
						//Console::getInstance().println("IsDead			" + std::to_string(entityList[iIndex].isDead()));
						//Console::getInstance().println("entityHealth			" + std::to_string(entityHealth));
						//Console::getInstance().println("killerIndex	" + std::to_string(killerIndex));

						//recheck local player hits, if the hitDifference value is less than 1
						if (hitDifference < 1) {
							hitDifference = Profiler::checkPlayerHits();
						}

						//check if hitDifference is higher than 0
						if (hitDifference > 0) {
							//trigger event 'onPlayerHitTarget'
							int hitGroupId = entityList[iIndex].getLastHitGroup();
							Profiler::onPlayerHitTarget(killerIndex, iIndex, hitGroupId, healthDifference);
							hitDifference = 0;
						}

						//trigger event 'onPlayerGetKilled'
						Profiler::onPlayerGetKilled(killerIndex, iIndex, healthDifference);

						//check who dies to reset the profiler values after death
						if (localProfile.index == iIndex) {
							//the local player dies; reset profiler values from all players
							for (int jIndex = 0; jIndex <= game->getMaxClients(); jIndex++) {
								Profiler::resetPlayerAfterDeath(jIndex);
							}
						} else {
							//another player dies; reset profiler values from this player
							Profiler::resetPlayerAfterDeath(iIndex);
						}
					}
					continue;
				} else {
					if (!entityProfile[iIndex].isResetAfterDeathNeeded) {
						entityProfile[iIndex].isResetAfterDeathNeeded = true;
					}
				}

				//check if player has the same team as the local player; continue if true
				if (entityList[iIndex].getTeamNum() == localEntity.getTeamNum())
					continue;

				//check if player is visible for the local player
				bool isVisible = entityList[iIndex].isVisible();
				if (isVisible) {
					float angleToTarget = Profiler::getAngleToPlayer(iIndex);
					localProfile.lastAngleToTarget = angleToTarget;

					if (localProfile.visibleEnemies <= 1) {
						if (angleToTarget <= START_FOV_FOR_FEATURE_1) {
							//start recording feature 1
							Profiler::startFeature_1(iIndex, angleToTarget);
						}
						if (angleToTarget <= START_FOV_FOR_FEATURE_13) {
							//start/continue recording feature 13
							Profiler::isPlayerMovingMouseToTarget(iIndex, angleToTarget);
						}
					}

					if (angleToTarget <= START_FOV_FOR_FEATURE_8) {
						//start recording feature 8
						Profiler::startFeature_8(iIndex);
					}

					if (angleToTarget <= START_FOV_FOR_FEATURE_11) {
						//start recording feature 11
						Profiler::startFeature_11(iIndex, angleToTarget);
					}

					bool hitDetected = false;
					if (playerInFOV != -1 && playerInFOV == iIndex) {
						//int boneId = GetClosestBoneId_debug(iIndex);
						//Console::getInstance().println("iIndex " + std::to_string(iIndex) + " boneId " + std::to_string(boneId));
						//Console::getInstance().println("playerInFOV			" + std::to_string(playerInFOV));

						//stop recording feature 13
						Profiler::stopRecordSpiralAimAngles(iIndex);

						//recheck local player hits, if the hitDifference value is less than 1
						if (hitDifference < 1) {
							hitDifference = Profiler::checkPlayerHits();
						}

						//recheck player health values, if the healthDifference value is less than 1
						if (healthDifference < 1) {
							Profiler::checkPlayerHealth(iIndex, entityHealth, entityLastHealth, healthDifference);
						}

						//check if hitDifference is higher than 0
						if (hitDifference > 0 && healthDifference > 0) {
							int hitGroupId = entityList[iIndex].getLastHitGroup();
							while (hitDifference > 0) {
								Profiler::onPlayerHitTarget(localProfile.index, iIndex, hitGroupId, (healthDifference / hitDifference));
								hitDifference--;
							}
							hitDetected = true;

							//stop recording feature 11
							Profiler::stopFeature_11(iIndex);
						} else {
							//Console::getInstance().println("hitDifference " + std::to_string(hitDifference) + " healthDifference " + std::to_string(healthDifference));
						}

						//stop recording feature 1
						Profiler::stopFeature_1(iIndex);

						//call event 'onPlayerAimOnTarget'
						if (localEntity.isActiveWeaponValid() && !localEntity.isDead()) {
							Profiler::onPlayerAimOnTarget(iIndex);
						}

					} else {
						//the player is not (no longer) in the field of view of the local player.

						if (featureData[iIndex].f5.started && localEntity.isActiveWeaponValid()) {
							//stop the recording of feature 5
							Profiler::stopFeature_5(iIndex, "NOT_IN_FOV");
						}

						//stop the recording of feature 9
						Profiler::stopFeature_9(iIndex);
					}
					if (hitDifference > 0 && healthDifference > 0 && hitDetected == false) {
						if (angleToTarget < 8 || getVisibleEnemies(iIndex) <= 1) {
							//int boneId = GetClosestBoneId(iIndex);
							int hitGroupId = entityList[iIndex].getLastHitGroup();
							while (hitDifference > 0) {
								Profiler::onPlayerHitTarget(localProfile.index, iIndex, hitGroupId, (healthDifference / hitDifference));
								hitDifference--;
							}
							hitDetected = true;
						} else {
							//Console::getInstance().println("!!!UNDETECTED HIT!!! angle " + std::to_string(angleToTarget));
						}
					}

					//start/continue recording feature 12
					Profiler::calculateRecoilSuppression(iIndex, angleToTarget);
				} else {
					Profiler::resetFeature_8(iIndex);

					//Profiler::resetFeature_14(iIndex);

					Profiler::resetRecoilSuppressionCalculation(iIndex);
				}
			}
			
		}
	}
	catch (...) {
		memory->debuglog("Profiler -> main -> exception");
	}
}

/**
	Invoked when a player aims on a target player.

	@param targetIndex the index of the target
	@return nothing
*/
void Profiler::onPlayerAimOnTarget(int targetIndex) {
	//Console::getInstance().appendToLogFile("onPlayerAimOnTarget");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (localProfile.ammo < 1) {
		if (!featureData[targetIndex].f5.started) {
			//start the recording of feature 5
			Profiler::startFeature_5(targetIndex);
		}
	} else {
		if (featureData[targetIndex].f5.started) {
			//stop the recording of feature 5
			Profiler::stopFeature_5(targetIndex, "RELOAD_CHANGE");
		}
	}

	if (localEntity.isActiveWeaponValid()) {
		//start the recording of feature 9
		Profiler::startFeature_9(targetIndex);
	}
}

/**
	Invoked when a player kills an enemy.

	@param killerIndex the index of the killer
	@param victimIndex the index of the victim
	@param damage the damage of the last hit
	@return nothing
*/
void Profiler::onPlayerGetKilled(int killerIndex, int victimIndex, int damage) {
	//Console::getInstance().appendToLogFile("onPlayerGetKilled");
	if (Profiler::playerIndexInvalid(killerIndex))
		return;

	if (Profiler::playerIndexInvalid(victimIndex))
		return;

	if (killerIndex == victimIndex) 
		return;

	if (killerIndex == localProfile.index) {
		//Console::getInstance().println("onPlayerGetKilled 2 killerIndex: " + std::to_string(killerIndex) + " victimIndex: " + std::to_string(victimIndex), white, red);

		//Trigger the event onPlayerWeaponShot here. In the main class function it is not triggerd, because the function GetShotsFired returns 0, if the player is dead.
		Profiler::onPlayerWeaponShot(killerIndex, victimIndex);

		//call the function isPlayerMovingMouseToTarget here. In the main class function it it not called, because the player is dead.
		Profiler::isPlayerMovingMouseToTarget(victimIndex, 60.0f);

		//feature 2
		Profiler::addFeature_2(featureData[victimIndex].f2.hits, localProfile.weaponId, std::time(0));

		//feature 4
		int timeToKill = static_cast<int>(getCurrentTimeInMilliseconds() - featureData[victimIndex].f1.startTime);
		Profiler::addFeature_4(timeToKill, localProfile.weaponId, std::time(0));

		//stop the recording of feature 9
		Profiler::stopFeature_9(victimIndex);
	} else if (victimIndex == localProfile.index) {
		//Console::getInstance().println("onPlayerGetKilled killerIndex: " + std::to_string(killerIndex) + " victimIndex: " + std::to_string(victimIndex), white, red);

		//feature 2
		for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
			Profiler::resetAllHits(playerIndex);
		}
		
		//feature 3
		int totalHits = localProfile.totalHitsSinceLastDeath,
			hitsWhileMoving = localProfile.hitsWhileMoving;
		Profiler::addFeature_3(hitsWhileMoving, totalHits, localProfile.weaponId, std::time(0));

		//reset values for feature 3
		Profiler::resetFeature_3();

		//reset values for feature 6
		Profiler::resetFeature_6(localProfile.index);

		//reset feature 10 for all players here
		for (int index = 0; index < game->getMaxClients(); index++) {
			Profiler::resetFeature_10(index);
		}
	}
	Profiler::resetAllHits(victimIndex);

	Profiler::resetFeature_8(victimIndex);

	Profiler::resetFeature_11(victimIndex);

	Profiler::resetFeature_14(victimIndex);

	//save recorded values here
	writeToProfileFile();
}

/**
	Invoked when a player hits an enemy. (actually just works for local player)

	@param playerIndex the index of the attacker
	@param targetIndex the index of the target
	@param boneId the id of the bone (body part)
	@param damage the damage
	@return nothing
*/
void Profiler::onPlayerHitTarget(int playerIndex, int targetIndex, int boneId, int damage) {
	//Console::getInstance().appendToLogFile("onPlayerHitTarget");

	if (Profiler::playerIndexInvalid(playerIndex) || Profiler::playerIndexInvalid(targetIndex))
		return;

	if (playerIndex == targetIndex || playerIndex != localProfile.index)
		return;

	bool critical = isHitCritical(boneId);
	Profiler::addHit(targetIndex, boneId, critical, damage, std::time(0));
	//Console::getInstance().println("onPlayerHitTarget playerIndex: " + std::to_string(playerIndex) + " targetIndex: " + std::to_string(targetIndex) + " damage: " + std::to_string(damage) + " boneId: " + std::to_string(boneId) + " critical: " + std::to_string(critical), white, green);

	if (featureData[targetIndex].f8.started && !featureData[targetIndex].f8.saved) {
		//stop recording feature 8
		stopFeature_8(targetIndex);
	}
}

/**
	Invoked when a player fires a weapon. (actually just works for local player)

	@param playerIndex the index of the player
	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::onPlayerWeaponShot(int playerIndex, int targetIndex) {
	//Console::getInstance().appendToLogFile("onPlayerWeaponShot");

	if (Profiler::playerIndexInvalid(playerIndex))
		return;

	//Console::getInstance().println("onPlayerWeaponShot playerIndex: " + std::to_string(playerIndex) + " targetIndex: " + std::to_string(targetIndex), red, white);

	//check if the player is the local player; if the functionality is extended then this does not have to be the case.
	if (localProfile.index == playerIndex) {
		//check if targetIndex is already given; This is the case when a player dies and the event is called.
		if (Profiler::playerIndexInvalid(targetIndex)) {
			//get the closest player and the distance to him.
			float distance = 100.f;
			int closestPlayerIndex = Profiler::getClosestPlayerIndex(distance);
			//Console::getInstance().println("onPlayerWeaponShot closestPlayerIndex: " + std::to_string(closestPlayerIndex) + " distance: " + std::to_string(distance), white, black);

			//check if a player has been found nearby.
			if (!Profiler::playerIndexInvalid(closestPlayerIndex)) {
				if (distance < START_FOV_FOR_FEATURE_10) {
					//start the recording of feature 10
					Profiler::tryStartFeature_10(closestPlayerIndex);
				}
				if (distance <= START_FOV_FOR_FEATURE_14) {
					//start recording feature 14
					Profiler::startFeature_14(closestPlayerIndex, distance);
				}
			}

			//find the player you aim for. (fov based)
			int targetIndex = Profiler::getPlayerIndexInCrosshair();
			if (targetIndex == -1) {
				targetIndex = Profiler::getPlayerIndexInFieldOfView();
			}

			//check if a player has been found and if he is still alive.
			if (!Profiler::playerIndexInvalid(targetIndex) && !entityList[targetIndex].isDead()) {
				//calculate the deviation to the player and the life values of the player.
				float angleToTarget = Profiler::getAngleToPlayer(targetIndex);
				int	entityHealth = entityList[targetIndex].getHealth(),
					healthDifference = entityProfile[targetIndex].nextToLastHealth - entityHealth;
				//check if the deviation to the player is less than 4.f
				if (angleToTarget < 4.f || (healthDifference > 0 && angleToTarget < 10.f)) {
					//update the recording of feature 10
					Profiler::updateFeature_10(targetIndex, true, false);
				} else {
					//Console::getInstance().println("onPlayerWeaponShot | NO HIT | healthDifference: " + std::to_string(healthDifference) + " angleToTarget: " + std::to_string(angleToTarget), red, white);
				}
			} else {
				//Console::getInstance().println("onPlayerWeaponShot | NO TARGET IN FOV | targetIndex: " + std::to_string(targetIndex), red, white);
			}
		} else {
			//check if the player is dead.
			if (entityList[targetIndex].isDead()) {
				//get the health difference; At death, that's just the last value.
				//int healthDifference = entityProfile[targetIndex].nextToLastHealth;
				//Console::getInstance().println("onPlayerWeaponShot healthDifference: " + std::to_string(healthDifference), white, black);

				//try to start the recording of feature 10 (for one click kill; e.g. headshot)
				Profiler::tryStartFeature_10(targetIndex);
				//update the recording of feature 10
				Profiler::updateFeature_10(targetIndex, true, true);
			}
		}
	}
	//Console::getInstance().println("onPlayerWeaponShot playerIndex: " + std::to_string(playerIndex) + " targetIndex: " + std::to_string(targetIndex), white, black);
}

/**
	Reads the current values from the game (memory) for each player and saves them for the profiler.

	@return nothing
*/
void Profiler::initProfilerValues() {
	//Console::getInstance().appendToLogFile("initProfilerValues");

	//Console::getInstance().logln(LogLevel::LOG_ALL, "initProfilerValues");

	localProfile.index = 0;
	localProfile.ammo = 0;
	localProfile.weaponId = 0;
	localProfile.weaponType = 0;
	localProfile.visibleEnemies = 0;

	localProfile.movingMouseToTargetIndex = -1;
	localProfile.lastTargetForFeature10 = -1;

	localProfile.lastShotsNum = localEntity.getShotsFired();
	localProfile.totalHitsOnServer = localEntity.getTotalHitsOnServer(true);
	localProfile.lastTotalHitsOnServer = localProfile.totalHitsOnServer;

	Profiler::resetFeature_3();

	for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
		std::this_thread::yield();

		if (!entityList[playerIndex].isValidForProfiler())
			continue;

		entityProfile[playerIndex].isResetAfterDeathNeeded = false;

		entityProfile[playerIndex].isMovingMouseToTarget = false;
		entityProfile[playerIndex].lastDistanceToTarget = 75.0f;

		entityProfile[playerIndex].lastDeathsNum = entityList[playerIndex].getDeathsNum();
		entityProfile[playerIndex].lastKillsNum = entityList[playerIndex].getKillsNum();

		entityProfile[playerIndex].lastHealth = entityList[playerIndex].getHealth();
		entityProfile[playerIndex].nextToLastHealth = entityProfile[playerIndex].lastHealth;
	}
}

/**
	Try to start the recording of feature 10.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::tryStartFeature_10(int targetIndex) {
	//Console::getInstance().appendToLogFile("tryStartFeature_10");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("tryStartFeature_10 targetIndex: " + std::to_string(targetIndex) + " localProfile.lastTargetForFeature10: " + std::to_string(localProfile.lastTargetForFeature10), white, black);
	if (localProfile.lastTargetForFeature10 != -1 && targetIndex != localProfile.lastTargetForFeature10) {
		if (entityList[localProfile.lastTargetForFeature10].isDead()) {
			//do nothing and wait for stop recording of feature 10.
		} else {
			//reset the values of feature 10 from both players
			Profiler::resetFeature_10(targetIndex);
			Profiler::resetFeature_10(localProfile.lastTargetForFeature10);
			//start the recording of feature 10.
			Profiler::startFeature_10(targetIndex);
		}
	} else {
		//start the recording of feature 10.
		Profiler::startFeature_10(targetIndex);
	}
}

/**
	Starts the recording of feature 10.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_10(int targetIndex) {
	//Console::getInstance().appendToLogFile("startFeature_10");
	if (Profiler::playerIndexInvalid(targetIndex))

		return;
	if (!featureData[targetIndex].f10.started && !featureData[targetIndex].f10.saved) {
		localProfile.lastTargetForFeature10 = targetIndex;

		featureData[targetIndex].f10.started = true;
		featureData[targetIndex].f10.saved = false;
		featureData[targetIndex].f10.shotCounter = 0;
		//Console::getInstance().println("startFeature_10 targetIndex: " + std::to_string(targetIndex), white, black);
	} else if (featureData[targetIndex].f10.started && !featureData[targetIndex].f10.saved) {
		Profiler::updateFeature_10(targetIndex, false, false);
	} else {
		//its not an error; the feature has already been recorded and the player is still alive.
		//Console::getInstance().println("startFeature_10 error targetIndex: " + std::to_string(targetIndex) + " started: " + std::to_string(featureData[targetIndex].f10.started) + " saved: " + std::to_string(featureData[targetIndex].f10.saved), red, white);
	}
}

/**
	Updates the recording of feature 10.

	@param targetIndex the index of the player for which the recording is updated.
	@return nothing
*/
void Profiler::updateFeature_10(int targetIndex, bool hit, bool reset) {
	//Console::getInstance().appendToLogFile("updateFeature_10");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (featureData[targetIndex].f10.started && !featureData[targetIndex].f10.saved) {
		featureData[targetIndex].f10.shotCounter++;
		if (hit) {
			Profiler::stopFeature_10(targetIndex, hit, reset);
		}
		//Console::getInstance().println("updateFeature_10 targetIndex: " + std::to_string(targetIndex) + " hit: " + std::to_string(hit), white, black);
	}
}

/**
	Stops the recording of feature 10.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_10(int targetIndex, bool hit, bool reset) {
	//Console::getInstance().appendToLogFile("stopFeature_10");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (featureData[targetIndex].f10.started && !featureData[targetIndex].f10.saved) {
		featureData[targetIndex].f10.saved = true;
		featureData[targetIndex].f10.started = false;
		if (hit) {
			Profiler::addFeature_10(featureData[targetIndex].f10.shotCounter, localProfile.weaponId, std::time(0));
		}
		//Console::getInstance().println("stopFeature_10 targetIndex: " + std::to_string(targetIndex) + " hit: " + std::to_string(hit) + " reset: " + std::to_string(reset), white, black);
	}
	if (reset) {
		//reset feature 10 for all players here
		for (int index = 0; index < game->getMaxClients(); index++) {
			Profiler::resetFeature_10(index);
		}
	}
}

/**
	Resets the values for feature 10.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_10(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_10");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (targetIndex >= 0 && targetIndex < sizeof(featureData)) {
		featureData[targetIndex].f10.saved = false;
		featureData[targetIndex].f10.started = false;
		featureData[targetIndex].f10.shotCounter = 0;
		//Console::getInstance().println("resetFeature_10 targetIndex: " + std::to_string(targetIndex), white, black);
	}
}

/**
	Adds a feature 10 entry.

	@param shotsUntilHit the number of shot until the first hit
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_10(int shotsUntilHit, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_10");
	//Console::getInstance().println("addFeature_10", white, blue);
	//Console::getInstance().println("shotsUntilHit: " + std::to_string(shotsUntilHit), white, gray);
	profile["f10"] += { { "shotsUntilHit", shotsUntilHit }, { "weaponId", weaponId }, { "time", time }, {"aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 11.

	@param targetIndex the index of the player for which the recording is stopped
	@param angle the deviation at the beginning of the aiming
	@return nothing
*/
void Profiler::startFeature_11(int targetIndex, float angle) {
	//Console::getInstance().appendToLogFile("startFeature_11");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f11.started && !featureData[targetIndex].f11.saved) {
		//Console::getInstance().println("startFeature_11 targetIndex" + std::to_string(targetIndex) + " angle" + std::to_string(angle), white, blue);
		featureData[targetIndex].f11.started = true;
		featureData[targetIndex].f11.saved = false;
		featureData[targetIndex].f11.angle = angle;
		featureData[targetIndex].f11.startTime = getCurrentTimeInMilliseconds();
	}
}

/**
	Stops the recording of feature 11.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_11(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopFeature_11");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (featureData[targetIndex].f11.started && !featureData[targetIndex].f11.saved) {
		//Console::getInstance().println("stopFeature_11 targetIndex" + std::to_string(targetIndex), white, blue);
		featureData[targetIndex].f11.saved = true;
		INT64 aimTime = getCurrentTimeInMilliseconds() - featureData[targetIndex].f11.startTime;
		if (aimTime > 0 && aimTime < 3000) {
			addFeature_11(featureData[targetIndex].f11.angle, aimTime, localProfile.weaponId, std::time(0));
		}
		featureData[targetIndex].f11.started = false;
	}
}

/**
	Resets the values for feature 11.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_11(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_11");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("resetFeature_11 targetIndex" + std::to_string(targetIndex), white, blue);
	featureData[targetIndex].f11.saved = false;
	featureData[targetIndex].f11.started = false;
}

/**
	Adds a feature 11 entry.

	@param angle the deviation at the beginning of the aiming
	@param hit the time to aim at the player
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_11(float angle, INT64 aimTime, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_11");
	//Console::getInstance().println("addFeature_11", white, blue);
	//Console::getInstance().println("angle: " + std::to_string(angle) + " aimTime: " + std::to_string(aimTime), white, red);
	profile["f11"] += { { "angle", angle }, { "aimTime", aimTime }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 1.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_1(int targetIndex, float angle) {
	//Console::getInstance().appendToLogFile("startFeature_1");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (angle == 0.f)
		return;

	INT64 firstEncounterTime = (featureData[targetIndex].f1.startTime / 1000);
	if ((std::time(0) - firstEncounterTime) > RESET_FIRST_ENCOUNTER_TIME) {
		// calc angle from first encounter
		featureData[targetIndex].f1.angle = angle;
		featureData[targetIndex].f1.startTime = getCurrentTimeInMilliseconds();
		featureData[targetIndex].f1.saved = false;
		featureData[targetIndex].f1.timesSeenBefor ++;
		//Console::getInstance().println("startFeature_1 angle (" + std::to_string(angle) + ") for player index " + std::to_string(targetIndex));
	}
}

/**
	Stops the recording of feature 1.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_1(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopFeature_1");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f1.saved) {
		//save angle from first encounter
		featureData[targetIndex].f1.saved = true;
		addFeature_1(featureData[targetIndex].f1.angle, featureData[targetIndex].f1.timesSeenBefor, localProfile.weaponId, std::time(0));
	}
}

/**
	Reset the values of feature 1.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_1(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_1");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f1.saved = false;
	featureData[targetIndex].f1.startTime = 0;
	featureData[targetIndex].f1.angle = 0.0f;
	featureData[targetIndex].f1.timesSeenBefor = -1;
}

/**
	Adds a feature 1 entry.

	@param div the deviation to the enemy at the first encounter
	@param timesSeenBefor the number of times he has seen the player before
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_1(float div, int timesSeenBefor, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_1");
	//Console::getInstance().println("addFeature_1", white, blue);
	//Console::getInstance().println("div: " + std::to_string(div) + " timesSeenBefor: " + std::to_string(timesSeenBefor), white, red);
	profile["f1"] += { { "div", div }, { "timesSeenBefor", timesSeenBefor }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Adds a feature 2 entry.

	@param hits the list of the hits
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_2(json hits, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_2");
	//Console::getInstance().println("addFeature_2", white, blue);
	/*
	int healthSum = 0;
	for (auto& element : hits.items()) {
		int hitNum = stoi(element.key()) + 1;
		auto hit = element.value();
		healthSum += hit["damage"];
		Console::getInstance().logln(LogLevel::LOG_ALL, std::to_string(hitNum) + " : " + std::to_string(hit["critical"]) + " : " + std::to_string(hit["damage"]));
	}
	Console::getInstance().logln(LogLevel::LOG_ALL, "healthSum " + std::to_string(healthSum));
	*/
	profile["f2"] += { { "hits", hits }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Adds a feature 3 entry.

	@param numHitsWhileMoving the number of hits while moving
	@param numTotalHits the number of total hits
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_3(int numHitsWhileMoving, int numTotalHits, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_3");
	//Console::getInstance().println("addFeature_3", white, blue);
	//Console::getInstance().println("numHitsWhileMoving: " + std::to_string(numHitsWhileMoving) + " numTotalHits: " + std::to_string(numTotalHits), white, red);
	profile["f3"] += { { "hitsWhileMoving", numHitsWhileMoving }, { "totalHits", numTotalHits }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Reset the values of feature 3.

	@return nothing
*/
void Profiler::resetFeature_3() {
	//Console::getInstance().appendToLogFile("resetFeature_3");
	localProfile.totalHitsSinceLastDeath = 0;
	localProfile.hitsWhileMoving = 0;
}

/**
	Adds a feature 4 entry.

	@param timeToKill the time to kill
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_4(int timeToKill, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_4");
	//Console::getInstance().println("addFeature_4", white, blue);
	//Console::getInstance().println("timeToKill: " + std::to_string(timeToKill), white, red);
	profile["f4"] += { { "timeToKill", timeToKill }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Adds a feature 5 entry.

	@param aimTime the time a player aims, even though his weapon is empty
	@param reason the reason why the player is no longer aiming at the opponent
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_5(int aimTime, std::string reason, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_5");
	//Console::getInstance().println("addFeature_5", white, blue);
	//Console::getInstance().println("aimTime: " + std::to_string(aimTime) + " reason: " + reason, white, red);
	profile["f5"] += { { "aimTime", aimTime }, { "reason", reason }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 5.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_5(int targetIndex) {
	//Console::getInstance().appendToLogFile("startFeature_5");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("START AIMING ON PLAYER WITH EMPTY WEAPON!!!");
	featureData[targetIndex].f5.started = true;
	featureData[targetIndex].f5.startTime = getCurrentTimeInMilliseconds();
}

/**
	Stops the recording of feature 5.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_5(int targetIndex, std::string reason) {
	//Console::getInstance().appendToLogFile("stopFeature_5");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("STOP AIMING ON PLAYER WITH EMPTY WEAPON!!!");
	//the local player has previously targeted the player with an empty weapon.
	int aimTime = static_cast<int>(Profiler::getCurrentTimeInMilliseconds() - featureData[targetIndex].f5.startTime);
	featureData[targetIndex].f5.started = false;
	Profiler::addFeature_5(aimTime, reason, localProfile.weaponId, std::time(0));
}

/**
	Adds a feature 6 and 7 entry.

	@param timeToSwitch the time to switch to the secondary weapon, if the primary weapon is empty
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_6(int timeToSwitch, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_6");
	//Console::getInstance().println("addFeature_6_and_7", white, blue);
	//Console::getInstance().println("timeToSwitch: " + std::to_string(timeToSwitch), white, red);
	profile["f6"] += { { "timeToSwitch", timeToSwitch }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 6.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_6(int targetIndex) {
	//Console::getInstance().appendToLogFile("startFeature_6");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("startFeature_6_and_7", white, blue);
	featureData[targetIndex].f6.started = true;
	featureData[targetIndex].f6.saved = false;
	featureData[targetIndex].f6.weaponId = localProfile.weaponId;
	featureData[targetIndex].f6.startTime = Profiler::getCurrentTimeInMilliseconds();
}

/**
	Stops the recording of feature 6.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_6(int targetIndex, bool weaponReloaded) {
	//Console::getInstance().appendToLogFile("stopFeature_6");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("startFeature_6_and_7 weaponReloaded " + std::to_string(weaponReloaded), white, blue);
	if (weaponReloaded) {
		if (!featureData[targetIndex].f6.saved) {
			Profiler::addFeature_6(-1, featureData[targetIndex].f6.weaponId, std::time(0));
		}
		featureData[targetIndex].f6.started = false;
	} else {
		int timeToSwitchWeapon = static_cast<int>(Profiler::getCurrentTimeInMilliseconds() - featureData[targetIndex].f6.startTime);
		Profiler::addFeature_6(timeToSwitchWeapon, featureData[targetIndex].f6.weaponId, std::time(0));
	}
	featureData[targetIndex].f6.saved = true;
}

/**
	Reset the values of feature 6.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_6(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_6");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f6.started = false;
	featureData[targetIndex].f6.saved = false;
	featureData[targetIndex].f6.startTime = 0;
}

/**
	Adds a feature 7 entry.

	@param time the current timestamp
	@return nothing
*/
/*
void Profiler::addFeature_7(time_t time) {
	Console::getInstance().println("addFeature_7", white, blue);
	profile["f7"] += { { { "time", time }, {"aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}
*/

/**
	Adds a feature 8 entry.

	@param firstHitBoneId the id of the bone the player shoots first
	@param closestBoneId the id of the bone which is closest to the player
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_8(int firstHitBoneId, int closestBoneId, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_8");
	//Console::getInstance().println("addFeature_8", white, blue);
	//Console::getInstance().println("firstHitBoneId: " + std::to_string(firstHitBoneId) + " closestBoneId: " + std::to_string(closestBoneId), white, red);
	profile["f8"] += { { "firstHitBoneId", firstHitBoneId }, { "closestBoneId", closestBoneId }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 8.

	@param targetIndex the index of the player for which the recording is started
	@return nothing
*/
void Profiler::startFeature_8(int targetIndex) {
	//Console::getInstance().appendToLogFile("startFeature_8");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f8.started && !featureData[targetIndex].f8.saved) {
		int closestBoneId = getClosestBoneId(targetIndex, (START_FOV_FOR_FEATURE_8 + 1.0f));
		if (closestBoneId != -1) {
			featureData[targetIndex].f8.started = true;
			featureData[targetIndex].f8.saved = false;
			featureData[targetIndex].f8.closestBone = closestBoneId;
			//Console::getInstance().println("startFeature_8 closestBoneId " + std::to_string(closestBoneId));
		} else {
			Profiler::resetFeature_8(targetIndex);
		}
	}
}

/**
	Stops the recording of feature 8.

	@param targetIndex the index of the player for which the recording is stopped
	@return nothing
*/
void Profiler::stopFeature_8(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopFeature_8");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	int firstHitBoneId = Profiler::getClosestBoneId(targetIndex);
	if (firstHitBoneId != -1) {
		featureData[targetIndex].f8.saved = true;
		featureData[targetIndex].f8.started = false;
		Profiler::addFeature_8(firstHitBoneId, featureData[targetIndex].f8.closestBone, localProfile.weaponId, std::time(0));
	} else {
		Profiler::resetFeature_8(targetIndex);
	}
}

/**
	Resets the values for feature 8.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_8(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_8");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f8.saved = false;
	featureData[targetIndex].f8.started = false;
	featureData[targetIndex].f8.closestBone = false;
}

/**
	Adds a feature 9 entry.

	@param hitsWhileAiming the number of hits the player made while aiming
	@param shotsWhileAiming the number of shots the player fired while aiming
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_9(int hitsWhileAiming, int shotsWhileAiming, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_9");
	//Console::getInstance().println("addFeature_9", white, blue);
	//Console::getInstance().println("hitsWhileAiming: " + std::to_string(hitsWhileAiming) + " shotsWhileAiming: " + std::to_string(shotsWhileAiming), white, red);
	profile["f9"] += { { "hitsWhileAiming", hitsWhileAiming }, { "shotsWhileAiming", shotsWhileAiming }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts the recording of feature 9.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_9(int targetIndex) {
	//Console::getInstance().appendToLogFile("startFeature_9");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f9.started) {
		//Console::getInstance().println("lastTotalHitsOnServer " + std::to_string(localProfile.lastTotalHitsOnServer));
		//Console::getInstance().println("ammo " + std::to_string(localProfile.ammo));
		featureData[targetIndex].f9.started = true;
		featureData[targetIndex].f9.totalHitsFromLastAimingPeriod = localProfile.lastTotalHitsOnServer;
		featureData[targetIndex].f9.totalAmmoBefor = localProfile.ammo;
	}
}

/**
	Stops the recording of feature 9.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopFeature_9(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopFeature_9");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (featureData[targetIndex].f9.started) {
		//Console::getInstance().println("totalHitsFromLastAimingPeriod " + std::to_string(featureData[targetIndex].f9.totalHitsFromLastAimingPeriod));
		//Console::getInstance().println("totalAmmoBefor " + std::to_string(featureData[targetIndex].f9.totalAmmoBefor));
		int hitsWhileAiming = localProfile.totalHitsOnServer - featureData[targetIndex].f9.totalHitsFromLastAimingPeriod,
			shotsWhileAiming = featureData[targetIndex].f9.totalAmmoBefor - localProfile.ammo;
		if (shotsWhileAiming > 0 && hitsWhileAiming >= 0 && shotsWhileAiming >= hitsWhileAiming) {
			Profiler::addFeature_9(hitsWhileAiming, shotsWhileAiming, localProfile.weaponId, std::time(0));
		}
	}
	featureData[targetIndex].f9.started = false;
	featureData[targetIndex].f9.totalHitsFromLastAimingPeriod = 0;
	featureData[targetIndex].f9.totalAmmoBefor = 0;
}

/**
	Calculates the angle from the local player to the player with the given index.

	@param targetIndex the index of the target player
	@return the angle to the target player
*/
float Profiler::getAngleToPlayer(int targetIndex) {
	//Console::getInstance().appendToLogFile("getAngleToPlayer");
	if (Profiler::playerIndexInvalid(targetIndex))
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
		float tempFov = getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[targetIndex].getBonePosition(boneId));
		if (tempFov < bestFov) {
			bestFov = tempFov;
		}
	}
	
	return bestFov;
}

/**
	Converts an angle into one normalised Vector.
	It points in the same direction as the angle ("forward").

	https://developer.valvesoftware.com/wiki/AngleVectors()

	@param angles
	@param forward
	@return nothing
*/
void Profiler::anglesToVector(Vector angle, Vector& vector) {
	//Console::getInstance().appendToLogFile("anglesToVector");
	float pitch = float(angle[0] * M_PI / 180);
	float yaw = float(angle[1] * M_PI / 180);
	float tmp = float(cos(pitch));
	vector.x = float(-tmp * -cos(yaw));
	vector.y = float(sin(yaw)*tmp);
	vector.z = float(-sin(pitch));
}

/**
	Calculates the distance between the current view angles and the view angles the player must have to look at the target.

	@param viewAngles the view angles of the player
	@param src the position of the player
	@param dst the position of the target
	@return the calculated distance
*/
float Profiler::getFieldOfView(Vector viewAngles, Vector src, Vector dst) {
	Vector aimVector, viewVector;
	aimVector = Profiler::calculateAngles(src, dst);

	pMath.anglesToVector(viewAngles, viewVector);
	pMath.anglesToVector(aimVector, aimVector);

	double mag = viewVector.Length();
	double u_dot_v = viewVector.DotProduct(aimVector);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

/**
	Calculates the view angles to look at the target.

	@param src the position of the player
	@param dst the position of the target
	@return the calculated view angles
*/
Vector Profiler::calculateAngles(Vector src, Vector dst) {
	Vector vDelta = src - dst;
	float fHyp = vDelta.LengthXY();

	float yaw = (atanf(vDelta.y / vDelta.x)) * (180.0f / (float)M_PI);
	float pitch = (atanf(vDelta.z / fHyp)) * (180.0f / (float)M_PI);

	if (vDelta.x >= 0.0f)
		yaw += 180.0f;

	return Vector(pitch, yaw, 0.0f);
}

/**
	Check if a hit is critical.

	@param boneId the id of the bone (body part)
	@return true if hit is critical, otherwise false
*/
bool Profiler::isHitCritical_Bone(int boneId) {
	//Console::getInstance().appendToLogFile("isHitCritical_Bone");
	return boneId == 8 ? true : false; // 8 = HEAD
}

/**
	Check if a hit is critical.

	@param boneId the id of the bone (body part)
	@return true if hit is critical, otherwise false
*/
bool Profiler::isHitCritical(int hitGroupId) {
	//Console::getInstance().appendToLogFile("isHitCritical");
	return hitGroupId == HITGROUP_HEAD ? true : false;
}

/**
	Reads the profile data from the profile.json file.

	@return nothing
*/
void Profiler::readFromProfileFile() {
	//Console::getInstance().appendToLogFile("readFromProfileFile");
	try {
		std::ifstream i("C:\\tubs\\profile.json");
		i >> profile;
	}
	catch (...) {
		//memory->debuglog("Profiler -> readFromProfileFile");
	}
}

/**
	Writes the profile data to the profile.json file.

	@return nothing
*/
void Profiler::writeToProfileFile() {
	//Console::getInstance().appendToLogFile("writeToProfileFile");
	try {
		std::ofstream o("C:\\tubs\\profile.json");
		o << std::setw(4) << profile << std::endl;
	}
	catch (...) {
		memory->debuglog("Profiler -> writeToProfileFile");
	}
}

/**
	Writes the profile data to the profile.json file.

	@return nothing
*/
void Profiler::appendToProfileFile(json j) {
	//Console::getInstance().appendToLogFile("appendToProfileFile");
	try {
		std::ofstream outfile;
		outfile.open("C:\\tubs\\profile_backup.json", std::ios_base::app);
		outfile << std::setw(4) << j << std::endl;
	}
	catch (...) {
		memory->debuglog("Profiler -> appendToProfileFile");
	}
}

/**
	Calculates a random float between two floats.

	@param a min float
	@param b max float
	@return the random float
*/
float Profiler::randomFloat(float a, float b) {
	//Console::getInstance().appendToLogFile("randomFloat");
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

/**
	Calculates the elapsed time in milliseconds since 01/01/1970.

	@return the calculated time in milliseconds
*/
INT64 Profiler::getCurrentTimeInMilliseconds() {
	//Console::getInstance().appendToLogFile("getCurrentTimeInMilliseconds");
	auto now = system_clock::now();
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
	return ms;
}

/**
	Calculates the number of visible enemies.

	@return the number of visible enemies
*/
int Profiler::getVisibleEnemies(int playerIndex) {
	//Console::getInstance().appendToLogFile("getVisibleEnemies");
	if (Profiler::playerIndexInvalid(playerIndex))
		return 0;

	int num = 0;
	for (int index = 0; index < game->getMaxClients(); index++) {
		//std::this_thread::yield();
		if (entityList[index].isValid() && entityList[index].isVisible() && entityList[index].getHealth() > 0 && entityList[index].getTeamNum() != entityList[playerIndex].getTeamNum())
			num++;
	}
	return num;
}

/**
	Search for the index of a player targeted.

	@return the index of the player targeted, otherwise -1
*/
int Profiler::getPlayerIndexInCrosshair() {
	////Console::getInstance().appendToLogFile("getPlayerIndexInCrosshair");

	int playerIndexInCrosshair = localEntity.getCrosshairEnt() -1;

	if (Profiler::playerIndexInvalid(playerIndexInCrosshair))
		return -1;

	if (entityList[playerIndexInCrosshair].getHealth() > 0 && entityList[playerIndexInCrosshair].getTeamNum() != localEntity.getTeamNum()) {
		return playerIndexInCrosshair;
	}

	/*
	for (int index = 0; index < game->getMaxClients(); index++) {
		//std::this_thread::yield();
		if (entityList[index].getIndex() != playerIndexInCrosshair)
			continue;

		if (entityList[index].getHealth() < 1)
			continue;
					
		if (entityList[index].getTeamNum() == localEntity.getTeamNum())
			continue;
				
		return index;
	}
	*/

	return -1;
}

/**
	Look for the closest bone (body part) of a player.

	@param targetIndex the index of the target player
	@return the index of the closest bone from the target player, -1 if nothing was found.
*/
int Profiler::getClosestBoneId(int targetIndex, float maxDistance) {
	//Console::getInstance().appendToLogFile("getClosestBoneId");
	int bestBoneId = -1;

	if (Profiler::playerIndexInvalid(targetIndex))
		return bestBoneId;

	float maxfov = maxDistance;
	const int numBones = 7;
	int bones[numBones] = { 8, 7, 6, 5, 4, 3, 0 };

	Vector viewAngles, punchAngles;
	game->getViewAngles(viewAngles);

	punchAngles = localEntity.getPunchAngles();
	punchAngles.z = 0.0f;
	viewAngles -= punchAngles * 2;

	for (int boneIndex = 0; boneIndex < numBones; boneIndex++) {
		float fov = getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[targetIndex].getBonePosition(bones[boneIndex]));
		if (fov < maxfov) {
			maxfov = fov;
			bestBoneId = bones[boneIndex];
		}
	}
	return bestBoneId;
}

/**
	Look for the closest bone (body part) of a player.

	NOTE: This is just a debug function, as this feature scans all the bones.

	@param targetIndex the index of the target player
	@return the index of the closest bone from the target player, -1 if nothing was found.
*/
int Profiler::getClosestBoneId_debug(int targetIndex, float maxDistance) {
	//Console::getInstance().appendToLogFile("getClosestBoneId_debug");
	int bestBoneId = -1;

	if (Profiler::playerIndexInvalid(targetIndex))
		return bestBoneId;

	float maxfov = maxDistance;
	const int maxBoneId = 100;

	Vector viewAngles, punchAngles;
	game->getViewAngles(viewAngles);

	punchAngles = localEntity.getPunchAngles();
	punchAngles.z = 0.0f;
	viewAngles -= punchAngles * 2;

	for (int boneId = 0; boneId < maxBoneId; boneId++) {
		float fov = getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[targetIndex].getBonePosition(boneId));
		if (fov < maxfov) {
			maxfov = fov;
			bestBoneId = boneId;
		}
	}
	return bestBoneId;
}

/**
	Search for the index of a player in the field of view.

	@return the index of the player in the field of view, otherwise -1
*/
int Profiler::getPlayerIndexInFieldOfView() {
	//Console::getInstance().appendToLogFile("getPlayerIndexInFieldOfView");
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
bool Profiler::isBoneInFieldOfView(Vector &current, int targetIndex, int boneIndex, float fov) {
	//Console::getInstance().appendToLogFile("isBoneInFieldOfView");
	if (Profiler::playerIndexInvalid(targetIndex))
		return false;

	auto direction = entityList[targetIndex].getBonePosition(boneIndex) - localEntity.getEyePosition();
	vectorNormalize(direction);

	auto target = vectorToAngles(direction);
	clampAngles(target);

	Vector v_dist = localEntity.getOrigin() - entityList[targetIndex].getOrigin();
	float distance = v_dist.Length();

	if (canHit(current, target, distance, fov)) {
		return true;
	}

	return false;
}

/**
	Calculated if a shot would hit with the given parameters.

	@param current the current ViewAngles from the local player
	@param target the target angles
	@param distance the distance between the two entities
	@param max the maximum permissible deviation from the target point
	@return true if a hit is possible, otherwise false
*/
bool Profiler::canHit(Vector current, Vector target, float distance, float max) {
	//Console::getInstance().appendToLogFile("canHit");
	auto delta = target - current;
	clampAngles(delta);
	auto xdist = sinf(DEG2RAD(delta.Length())) * distance / 2.f;
	return xdist <= max;
}

/**
	Converts a single vector into a QAngle.
	VectorAngeles: https://developer.valvesoftware.com/wiki/AngleVectors()
	QAngle: https://developer.valvesoftware.com/wiki/QAngle

	@param angles the angles to clamp
	@return the clamped angles
*/
Vector Profiler::vectorToAngles(const Vector& direction) {
	//Console::getInstance().appendToLogFile("vectorToAngles");
	float pitch, yaw;

	if (!direction.x && !direction.y) {
		pitch = direction.z > 0.f ? 270.f : 90.f;
		yaw = 0.f;
	} else {
		pitch = RAD2DEG(atan2f(-direction.z, direction.LengthXY()));
		pitch += pitch < 0.f ? 360.f : 0.f;

		yaw = RAD2DEG(atan2f(direction.y, direction.x));
		yaw += yaw < 0.f ? 360.f : 0.f;
	}

	return Vector(pitch, yaw, 0.f);
}

/**
	Computes the length of the vector and normalizes the vector.

	@param vector the vector to normalize
	@return the length of the given vector
*/
float Profiler::vectorNormalize(Vector& vector) {
	//Console::getInstance().appendToLogFile("vectorNormalize");
	auto length = vector.Length();
	vector = length ? vector / length : Vector();
	return length;
}

/**
	Normalizes the angle. (from -180 to 180)

	@param angle the angle to normalize
	@return the normalized angle
*/
float Profiler::angleNormalize(float angle) {
	//Console::getInstance().appendToLogFile("angleNormalize");
	return isfinite(angle) ? remainder(angle, 360.f) : 0.f;
}

/**
	Clamps the angles. Sets the out of range values to the upper or lower limit.
	Ranges: https://developer.valvesoftware.com/wiki/QAngle

	@param angles the angles to clamp
	@return the clamped angles
*/
void Profiler::clampAngles(Vector& angles) {
	//Console::getInstance().appendToLogFile("clampAngles");
	angles.x = std::max(-89.f, std::min(89.f, angleNormalize(angles.x)));
	angles.y = angleNormalize(angles.y);
	angles.z = 0.f;
}

/**
	Reads the most recent hit values (total hits, last total hits, hit difference) of the local player.

	@return the hit difference between now and the last check
*/
int Profiler::checkPlayerHits() {
	//Console::getInstance().appendToLogFile("checkPlayerHits");
	int speed = localEntity.getSpeed(),
		hitDifference = 0;

	localProfile.totalHitsOnServer = localEntity.getTotalHitsOnServer(true);
	//printf("totalHitsOnServer %d\n", localProfile.totalHitsOnServer);
	if (localProfile.totalHitsOnServer == 0) {
		localProfile.lastTotalHitsOnServer = 0;
		//printf("-> lastTotalHitsOnServer %d\n", localProfile.lastTotalHitsOnServer);
	}

	if (localProfile.totalHitsOnServer > 0 && localProfile.totalHitsOnServer > localProfile.lastTotalHitsOnServer) {
		hitDifference = localProfile.totalHitsOnServer - localProfile.lastTotalHitsOnServer;
		//printf("---> hitDifference %d\n", hitDifference);
		localProfile.lastTotalHitsOnServer = localProfile.totalHitsOnServer;

		//FEATURE_3: increase total hits and hits while moving.
		localProfile.totalHitsSinceLastDeath += hitDifference;
		
		if (speed > 0) {
			localProfile.hitsWhileMoving += hitDifference;
		}
	}

	return hitDifference;
}

/**
	Reads the most recent life values (current health, last health, health difference) of the player.

	@param targetIndex the index of the player
	@param health reference to the health value
	@param lastHealth reference to the last health value
	@param healthDifference reference to the health difference value
	@return nothing
*/
void Profiler::checkPlayerHealth(int targetIndex, int& health, int& lastHealth, int& healthDifference) {
	//Console::getInstance().appendToLogFile("checkPlayerHealth");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//get players health values and calculate healthDifference
	health = entityList[targetIndex].getHealth();
	lastHealth = entityProfile[targetIndex].lastHealth;
	healthDifference = lastHealth - health;

	//store players last health values
	entityProfile[targetIndex].lastHealth = health;
	entityProfile[targetIndex].nextToLastHealth = lastHealth;
}

/**
	Find the index of the player closest to the local player. 
	Depending on where the local player is currently aiming.

	@return the index of the player closest to the local player, otherwise -1
*/
int Profiler::getClosestPlayerIndex(float &bestDistance) {
	//Console::getInstance().appendToLogFile("getClosestPlayerIndex");
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
			float fov = getFieldOfView(viewAngles, localEntity.getEyePosition(), entityList[index].getBonePosition(bones[boneId]));
			if (fov < bestDistance) {
				bestDistance = fov;
				closestPlayerIndex = index;
			}
		}

	}
	return closestPlayerIndex;
}

/**
	Calculates the recoil suppression while shooting on target player.

	@param targetIndex the index of the target player
	@param angleToTarget the current angle to the target player
	@return nothing
*/
void Profiler::calculateRecoilSuppression(int targetIndex, float angleToTarget) {
	//Console::getInstance().appendToLogFile("calculateRecoilSuppression");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	try {
		if (GetAsyncKeyState(config->keys.aimbot_hold) && !featureData[targetIndex].f12.saved && angleToTarget < MAX_FOV_FOR_FEATURE_12 && localProfile.ammo > 0) {
			EWeaponType weaponType = localEntity.getWeaponType();
			if ((weaponType == EWeaponType::WeaponType_LMG || weaponType == EWeaponType::WeaponType_SMG || weaponType == EWeaponType::WeaponType_Rifle)) {
				while (GetAsyncKeyState(config->keys.aimbot_hold) && !entityList[targetIndex].isDead()) {
					//TODO: bad idea to do it in this thread!
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
					Vector ViewAngles;
					game->getViewAngles(ViewAngles);
					if (featureData[targetIndex].f12.started) {
						Vector PunchAngles = localEntity.getPunchAngles();
						//Console::getInstance().logln(LogLevel::LOG_ALL, "calculateRecoilSuppression -> start: " + std::to_string(featureData[targetIndex].f12.startViewAngles[0]) + " now: " + std::to_string(ViewAngles[0]) + " diff: " + std::to_string(featureData[targetIndex].f12.startViewAngles[0] - ViewAngles[0]) + " punch: " + std::to_string(PunchAngles[0]) + " rcs: " + std::to_string((featureData[targetIndex].f12.startViewAngles[0] - ViewAngles[0]) / PunchAngles[0]));
						if (PunchAngles[0] != 0 && PunchAngles[1] != 0) {
							Vector differnce = (featureData[targetIndex].f12.startViewAngles - ViewAngles);
							int angleIndex = featureData[targetIndex].f12.recordedAnglesIndex;
							if (angleIndex >= 0 && angleIndex < MAX_REDCORDED_ANGLES) {
								featureData[targetIndex].f12.recordedAngles[angleIndex] = (differnce[0] != 0) ? (differnce / PunchAngles) : Vector(0, 0, 0);
								featureData[targetIndex].f12.recordedAnglesIndex++;
							}
							if (featureData[targetIndex].f12.recordedAnglesIndex == MAX_REDCORDED_ANGLES) {
								featureData[targetIndex].f12.saved = true;
								Profiler::addFeature_12(featureData[targetIndex].f12.recordedAngles, localProfile.weaponId, std::time(0));
							}
						}
					} else {
						featureData[targetIndex].f12.started = true;
						featureData[targetIndex].f12.saved = false;
						featureData[targetIndex].f12.startViewAngles = ViewAngles;
						featureData[targetIndex].f12.recordedAnglesIndex = 0;
						for (int i = 0; i < MAX_REDCORDED_ANGLES; i++) {
							featureData[targetIndex].f12.recordedAngles[i] = Vector(0, 0, 0);
						}
					}
				}

				if (entityList[targetIndex].isDead()) {
					Profiler::stopRecoilSuppressionCalculation(targetIndex);
				}
			} else {
				Profiler::stopRecoilSuppressionCalculation(targetIndex);
			}
		} else {
			Profiler::stopRecoilSuppressionCalculation(targetIndex);
		}
	}
	catch (const std::exception &e)
	{
		memory->debuglog("Profiler -> calculateRecoilSuppression -> " + std::string(e.what()));
	}
	catch (...) 
	{
		memory->debuglog("Profiler -> calculateRecoilSuppression -> exception");
	}
}

/**
	Stops the recoil suppression calculation.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::stopRecoilSuppressionCalculation(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopRecoilSuppressionCalculation");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	try {
		if (featureData[targetIndex].f12.started && !featureData[targetIndex].f12.saved) {
			//Console::getInstance().println("stopRecoilSuppressionCalculation targetIndex " + std::to_string(targetIndex), white, blue);
			Profiler::addFeature_12(featureData[targetIndex].f12.recordedAngles, localProfile.weaponId, std::time(0));
			featureData[targetIndex].f12.started = false;
			featureData[targetIndex].f12.saved = false;
		}
	}
	catch (const std::exception &e)
	{
		memory->debuglog("Profiler -> stopRecoilSuppressionCalculation -> " + std::string(e.what()));
	}
	catch (...)
	{
		memory->debuglog("Profiler -> stopRecoilSuppressionCalculation -> exception");
	}
}

/**
	Resets the values of feature 12 and stops the recoil suppression calculation.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::resetRecoilSuppressionCalculation(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetRecoilSuppressionCalculation");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	try {
		if (featureData[targetIndex].f12.started) {
			//Console::getInstance().println("resetRecoilSuppressionCalculation targetIndex " + std::to_string(targetIndex), white, blue);
			featureData[targetIndex].f12.started = false;
			featureData[targetIndex].f12.saved = false;
			featureData[targetIndex].f12.startViewAngles = Vector(0, 0, 0);
			featureData[targetIndex].f12.recordedAnglesIndex = 0;
			for (int i = 0; i < MAX_REDCORDED_ANGLES; i++) {
				featureData[targetIndex].f12.recordedAngles[i] = Vector(0, 0, 0);
			}
		}
	}
	catch (const std::exception &e)
	{
		memory->debuglog("Profiler -> resetRecoilSuppressionCalculation -> " + std::string(e.what()));
	}
	catch (...)
	{
		memory->debuglog("Profiler -> resetRecoilSuppressionCalculation -> resetRecoilSuppressionCalculation");
	}
}

/**
	Adds a feature 12 entry.

	@param angles the list of the recorded angles
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_12(Vector angles[], int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_12");
	try {
		//Console::getInstance().println("addFeature_12", white, blue);
		float sum = 0.f;
		int counter = 0;
		for (int i = 0; i < MAX_REDCORDED_ANGLES; i++) {
			if (angles[i][0] || angles[i][0]) {
				sum += angles[i][0];
				counter++;
			}
		}

		//Calculate the value only if enough data is available.
		if (counter > 0 && counter >= MIN_REDCORDS_FOR_FEATURE_12 && isfinite(sum)) {
			float avg = sum / counter;
			//Discard false values. These occur when positions and angles have not yet been updated.
			if (isfinite(avg) && -5.f < avg && avg < 5.f) {
				//Profiler::printFeature12(angles);
				//Console::getInstance().println("avg: " + std::to_string(avg), white, red);
				profile["f12"] += { { "avg", avg }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
				//writeToProfileFile();
			}
		}
	}
	catch (const std::exception &e)
	{
		memory->debuglog("Profiler -> addFeature_12 -> " + std::string(e.what()));
	}
	catch (...)
	{
		memory->debuglog("Profiler -> addFeature_12 -> exception");
	}
}

/**
	Prints the values of feature 12 to the console.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::printFeature12(Vector angles[]) {
	//Console::getInstance().appendToLogFile("printFeature12");
	try {
		for (int i = 0; i < MAX_REDCORDED_ANGLES; i++) {
			if (angles[i][0] || angles[i][1])
				Console::getInstance().println("[" + std::to_string(i) + "] x: " + std::to_string(angles[i][0]) + " y: " + std::to_string(angles[i][1]), white, red);
		}
	}
	catch (const std::exception &e)
	{
		memory->debuglog("Profiler -> printFeature12 -> " + std::string(e.what()));
	}
	catch (...)
	{
		memory->debuglog("Profiler -> printFeature12 -> exception");
	}
}

/**
	Resets the profiler values of the player.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::resetPlayerAfterDeath(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetPlayerAfterDeath");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	entityProfile[targetIndex].isResetAfterDeathNeeded = false;
	entityProfile[targetIndex].lastHealth = entityList[targetIndex].getHealth();

	Profiler::resetFeature_1(targetIndex);

	Profiler::resetFeature_6(targetIndex);

	Profiler::resetFeature_8(targetIndex);

	//reset feature 10 for all players here
	for (int index = 0; index < game->getMaxClients(); index++) {
		Profiler::resetFeature_10(index);
	}
}

/**
	Adds a feature 13 entry.

	@param x the x value for spiral aim
	@param y the y value for spiral aim
	@param aboveCounter the number of points that are above the direction vector
	@param belowCounter the number of points that are below the direction vector
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_13(float offsetX, float offsetY, int aboveCounter, int belowCounter, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_13");
	//Console::getInstance().println("addFeature_13", white, blue);
	//Console::getInstance().println("offsetX: " + std::to_string(offsetX) + " offsetY: " + std::to_string(offsetY) + " aboveCounter: " + std::to_string(aboveCounter) + " belowCounter: " + std::to_string(belowCounter), white, gray);
	profile["f13"] += { { "offsetX", offsetX }, { "offsetY", offsetY }, { "aboveCounter", aboveCounter }, { "belowCounter", belowCounter }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	//writeToProfileFile();
}

/**
	Starts or continue the recording of feature 13.

	@param targetIndex the index of the player for which the recording is started or continued
	@param boneIdToAimAt the index of the bone to which the local player is to aim
	@return nothing
*/
void Profiler::recordSpiralAimAngles(int targetIndex) {
	//Console::getInstance().appendToLogFile("recordSpiralAimAngles");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f13.started) {
		featureData[targetIndex].f13.started = true;
	}
	if (featureData[targetIndex].f13.counter < MAX_REDCORDED_ANGLES) {
		int c = featureData[targetIndex].f13.counter;

		Vector currentViewAngles;
		game->getViewAngles(currentViewAngles);

		Vector AimbotAnglesHead = Profiler::getLinearAimbotAngles(targetIndex, 8);
		Vector AimbotAnglesUpperBody = Profiler::getLinearAimbotAngles(targetIndex, 6);
		Vector AimbotAnglesLowerBody = Profiler::getLinearAimbotAngles(targetIndex, 0);
		
		featureData[targetIndex].f13.viewAngles[c] = currentViewAngles;
		featureData[targetIndex].f13.viewAnglesDelta[c][0] = (AimbotAnglesHead - currentViewAngles);
		featureData[targetIndex].f13.viewAnglesDelta[c][1] = (AimbotAnglesUpperBody - currentViewAngles);
		featureData[targetIndex].f13.viewAnglesDelta[c][2] = (AimbotAnglesLowerBody - currentViewAngles);

		featureData[targetIndex].f13.counter++;
		if (featureData[targetIndex].f13.counter == MAX_REDCORDED_ANGLES) {
			featureData[targetIndex].f13.saved = true;
		}
	}
}

/**
	Stops the recording of feature 13.

	@param targetIndex the index of the player for which the recording is stopped.
	@return nothing
*/
void Profiler::stopRecordSpiralAimAngles(int targetIndex) {
	//Console::getInstance().appendToLogFile("stopRecordSpiralAimAngles");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;
	
	if (featureData[targetIndex].f13.started && !featureData[targetIndex].f13.saved) {
		//Console::getInstance().println("stopRecordSpiralAimAngles targetIndex " + std::to_string(targetIndex));
		featureData[targetIndex].f13.saved = true;
		int		recordIndex = 0,
				closestBoneId = getClosestBoneId(targetIndex),
				c = featureData[targetIndex].f13.counter,
				belowCounter = 0,
				aboveCounter = 0;

		if (closestBoneId == 8) {
			recordIndex = 0;
		} else if (closestBoneId == 6 || closestBoneId == 7) {
			recordIndex = 1;
		} else {
			recordIndex = 2;
		}

		float	sumOffsetX = 0.f,
				sumOffsetY = 0.f;
		
		for (int n = 1; n < c; n++) {
			Vector lastViewAngles = featureData[targetIndex].f13.viewAngles[n - 1];
			Vector lastViewAnglesDelta = featureData[targetIndex].f13.viewAnglesDelta[n-1][recordIndex];
			lastViewAnglesDelta.Normalize();

			Vector currentViewAngles = featureData[targetIndex].f13.viewAngles[n];
			Vector currentViewAnglesDelta = currentViewAngles - lastViewAngles;
			currentViewAnglesDelta.Normalize();

			float smooth = static_cast<float>(c - (n - 1));
			Vector delta = (currentViewAnglesDelta - lastViewAnglesDelta / smooth);
			
			sumOffsetX += delta.x;
			sumOffsetY += delta.y;

			if (featureData[targetIndex].f13.viewAngles[0].x <= currentViewAngles.x) {
				belowCounter++;
			} else {
				aboveCounter++;
			}
		}

		float	avgOffsetX = (sumOffsetX / c),
				avgOffsetY = (sumOffsetY / c);

		Profiler::addFeature_13(avgOffsetX, avgOffsetY, aboveCounter, belowCounter, localProfile.weaponId, std::time(0));
	}
}
//Console::getInstance().println("feature13 delta xy [" + std::to_string(n) + "] x: " + std::to_string(delta.x) + " y: " + std::to_string(delta.y), white, red);
/**
	Resets the values for feature 13.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetRecordSpiralAimAngles(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetRecordSpiralAimAngles");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().println("resetRecordSpiralAimAngles targetIndex " + std::to_string(targetIndex));

	for (int i = 0; i < MAX_REDCORDED_ANGLES; i++) {
		featureData[targetIndex].f13.viewAnglesDelta[i][0] = Vector(0, 0, 0);
		featureData[targetIndex].f13.viewAnglesDelta[i][1] = Vector(0, 0, 0);
		featureData[targetIndex].f13.viewAnglesDelta[i][2] = Vector(0, 0, 0);
		featureData[targetIndex].f13.viewAngles[i] = Vector(0, 0, 0);
	}

	featureData[targetIndex].f13.counter = 0;
	featureData[targetIndex].f13.started = false;
	featureData[targetIndex].f13.saved = false;
}

/**
	Computes the ViewAngles that the local player must target to target the player with the given value.
	The result would be a linear "perfect" movement.

	@param targetIndex the index of the player to which the local player is to aim
	@param boneIdToAimAt the index of the bone to which the local player is to aim
	@return the ViewAngles
*/
Vector Profiler::getLinearAimbotAngles(int targetIndex, int boneIdToAimAt) {
	//Console::getInstance().appendToLogFile("getLinearAimbotAngles");
	if (Profiler::playerIndexInvalid(targetIndex))
		return Vector(0, 0, 0);

	// Check if bones are actually updated
	Vector enemyPos = entityList[targetIndex].getBonePosition(boneIdToAimAt);
	Vector targetOrigin = entityList[targetIndex].getOrigin();

	float check = (float)abs((targetOrigin - enemyPos).Length());

	if (check > 75.0f)
		return Vector(0, 0, 0);

	Vector position = localEntity.getEyePosition();

	// Velocity Compensate
	Vector myVelocity = localEntity.getVelocity();
	Vector targetVelocity = entityList[targetIndex].getVelocity();
	Vector distVec = position - enemyPos;
	float dist = distVec.Length();

	if (dist > 0.001f) {
		enemyPos.x += (targetVelocity.x) / dist;
		enemyPos.y += (targetVelocity.y) / dist;
		enemyPos.z += (targetVelocity.z) / dist;
		enemyPos.x -= (myVelocity.x) / dist;
		enemyPos.y -= (myVelocity.y) / dist;
		enemyPos.z -= (myVelocity.z) / dist;
	}

	// Calculate Angle to look at
	Vector ViewAngles = calculateAngles(position, enemyPos);
	ViewAngles.Normalize();

	Vector PunchAngles = localEntity.getPunchAngles();
	PunchAngles.z = 0.0f;

	ViewAngles -= PunchAngles * 2;
	ViewAngles.z = 0.0f;

	ViewAngles.Normalize();
	return ViewAngles;
}

/**
	Check if local players are moving their mouse towards any player.
	Invokes the events onStartMovingMouseToTarget and onStopMovingMouseToTarget.

	@return nothing
*/
void Profiler::isPlayerMovingMouseToTarget(int targetIndex, float distance) {
	//Console::getInstance().appendToLogFile("isPlayerMovingMouseToTarget");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	float lastDistance = entityProfile[targetIndex].lastDistanceToTarget;
	if (entityProfile[targetIndex].isMovingMouseToTarget && localProfile.movingMouseToTargetIndex == targetIndex) {
		//Console::getInstance().logln(LogLevel::LOG_ALL, "isPlayerMovingMouseToTarget -> " + std::to_string(targetIndex) + " : " + std::to_string(distance) + " | " + std::to_string(lastDistance));
		if ((distance < 2.0f || distance < lastDistance) && !entityList[targetIndex].isDead()) {
			Profiler::onMovingMouseToTarget(targetIndex, distance);
		} else if (distance > (lastDistance + 0.3f) || entityList[targetIndex].isDead()) {
			localProfile.movingMouseToTargetIndex = -1;
			entityProfile[targetIndex].isMovingMouseToTarget = false;
			Profiler::onStopMovingMouseToTarget(targetIndex);
		}
	} else if (localProfile.movingMouseToTargetIndex == -1) {
		if (distance > 3.0f && distance < (lastDistance - 0.1f)) {
			localProfile.movingMouseToTargetIndex = targetIndex;
			entityProfile[targetIndex].isMovingMouseToTarget = true;
			Profiler::onStartMovingMouseToTarget(targetIndex);
		}
	}
	entityProfile[targetIndex].lastDistanceToTarget = distance;
}

/**
	Invoked when the local player moves his mouse towards the target player.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::onMovingMouseToTarget(int targetIndex, float distance) {
	//Console::getInstance().appendToLogFile("onMovingMouseToTarget");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().logln(LogLevel::LOG_ALL, "onMovingMouseToTarget " + std::to_string(targetIndex));
	Profiler::recordSpiralAimAngles(targetIndex);
}

/**
	Invoked when the local player starts to move his mouse towards the target player.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::onStartMovingMouseToTarget(int targetIndex) {
	//Console::getInstance().appendToLogFile("onStartMovingMouseToTarget");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().logln(LogLevel::LOG_ALL, "onStartMovingMouseToTarget " + std::to_string(targetIndex));
}

/**
	Invoked when the local player stops to move his mouse towards the target player.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::onStopMovingMouseToTarget(int targetIndex) {
	//Console::getInstance().appendToLogFile("onStopMovingMouseToTarget");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	//Console::getInstance().logln(LogLevel::LOG_ALL, "onStopMovingMouseToTarget " + std::to_string(targetIndex));
	Profiler::resetRecordSpiralAimAngles(targetIndex);
}

/**
	Adds a recorded hit to the target player.

	@param targetIndex the index of the target player
	@param boneId the id of the bone
	@param critical whether the hit is critical or not
	@param damage the amount of damage
	@param time the current timestamp
	@return nothing
*/
void Profiler::addHit(int targetIndex, int boneId, bool critical, int damage, time_t time) {
	//Console::getInstance().appendToLogFile("addHit");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f2.hits += { { "boneId", boneId }, { "critical", (critical ? 1 : 0) }, { "damage", damage }, { "time", time } };
	//Console::getInstance().println("addHit targetIndex: [" + std::to_string(targetIndex) + "] boneId: " + std::to_string(boneId) + " critical: " + std::to_string(critical) + " damage: " + std::to_string(damage), white, red);
}

/**
	Reset all recorded hits from the target player.

	@param targetIndex the index of the target player
	@return nothing
*/
void Profiler::resetAllHits(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetAllHits");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f2.hits = {};
}

/**
	Check if the given player index is invalid.

	@param targetIndex the index to be tested
	@return true if the index is invalid, otherwise false
*/
bool Profiler::playerIndexInvalid(int targetIndex) {
	//Console::getInstance().appendToLogFile("playerIndexInvalid");
	return (targetIndex < 0 || targetIndex > MAX_PLAYERS_NUM);
}

/**
	Adds a feature 14 entry.

	@param div the deviation to the opponent
	@param weaponId the id of the weapon
	@param time the current timestamp
	@return nothing
*/
void Profiler::addFeature_14(float div, int weaponId, time_t time) {
	//Console::getInstance().appendToLogFile("addFeature_14");
	//Console::getInstance().println("addFeature_14", white, blue);
	//Console::getInstance().println("div: " + std::to_string(div), white, red);
	profile["f14"] += { { "div", div }, { "weaponId", weaponId }, { "time", time }, { "aimbotEnabled", config->general.aimbot_enabled } };
	writeToProfileFile();
}

/**
	Starts the recording of feature 14.

	@param targetIndex the index of the player for which the recording is started.
	@return nothing
*/
void Profiler::startFeature_14(int targetIndex, float div) {
	//Console::getInstance().appendToLogFile("startFeature_14");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	if (!featureData[targetIndex].f14.started) {
		featureData[targetIndex].f14.started = true;
		Profiler::addFeature_14(div, localProfile.weaponId, std::time(0));
	}
}

/**
	Resets the values for feature 14.

	@param targetIndex the index of the player for which the values are to be reset
	@return nothing
*/
void Profiler::resetFeature_14(int targetIndex) {
	//Console::getInstance().appendToLogFile("resetFeature_14");
	if (Profiler::playerIndexInvalid(targetIndex))
		return;

	featureData[targetIndex].f14.started = false;
}