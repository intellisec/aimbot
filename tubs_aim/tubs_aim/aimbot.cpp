#include "stdafx.h"
#include "dllmain.h"

/**
	This is the entry point of the Aimbots. Here, the automatic targeting is calculated and performed.

	@return nothing
*/
void Aimbot::start() {
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

			for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
				if (!entityList[playerIndex].isValid())
					continue;

				if (entityList[playerIndex].getTeamNum() == localEntity.getTeamNum())
					continue;

				if (entityList[playerIndex].isVisible() && aimbotSettings.angleFirstEncounter[playerIndex] == 0.f) {
					float angleToTarget = Aimbot::getAngleToPlayer(playerIndex);
					if (angleToTarget < MAX_ANGLE_FOR_FIRST_ENCOUNTER) {
						aimbotSettings.angleFirstEncounter[playerIndex] = angleToTarget;
					}
				} else {
					aimbotSettings.angleFirstEncounter[playerIndex] = 0.0f;
				}
			}

			if (!GetAsyncKeyState(config->keys.aimbot_hold)) {
				Aimbot::dropSelectedTarget();
				continue;
			}

			Aimbot::getAimbotSettings();

			anglesToAimAt = Aimbot::getAimbotAngles();

			if (Aimbot::isDelayNeeded()) {
				lastSelectedTargetIndex = -1;
				continue;
			}

			if (anglesToAimAt == Vector(0, 0, 0)) {
				continue;
			}	

			lastSelectedTargetIndex = selectedTargetIndex;

			int shotsFired = localEntity.getShotsFired();

			if (aimbotSettings.startBullet <= shotsFired && shotsFired <= aimbotSettings.endBullet) {
				if (aimbotSettings.faceitAim) {
					positionToMoveTowards = Aimbot::getAimbotMousePosition();
					Aimbot::moveMouseToPosition(positionToMoveTowards.x, positionToMoveTowards.y);
				} else {
					game->setViewAngles(anglesToAimAt);
				}
			}
		}
	} catch (...) {
		memory->debuglog(__FILE__);
	}
}

/**
	Calculates the settings of the Aimbot.

	@return nothing
*/
void Aimbot::getAimbotSettings() {
	float	noise = 1.0f;
	int		weaponId = localEntity.getActiveWeapon(),
			speed = localEntity.getSpeed();

	aimbotSettings.currentWeaponId = weaponId;

	if (speed > 0) {
		noise = Aimbot::transformNoise(config->aimbot.hit_chance_while_moving_noise);
		aimbotSettings.hitAccuracyWhileMoving = (config->weapons[weaponId].aimbot.hit_chance_while_moving * noise);
	} else {
		aimbotSettings.hitAccuracyWhileMoving = 1.0f;
	}

	aimbotSettings.compensateVelocity = config->aimbot.compensate_velocity;
	aimbotSettings.saveTargetAim = config->aimbot.save_target_aim;
	aimbotSettings.faceitAim = config->aimbot.faceit_aim;

	aimbotSettings.startBullet = config->weapons[weaponId].aimbot.start_bullet;
	aimbotSettings.endBullet = config->weapons[weaponId].aimbot.end_bullet;
	aimbotSettings.delayAfterKill = config->weapons[weaponId].aimbot.delay_after_kill;

	noise = Aimbot::transformNoise(config->aimbot.chance_hit_closest_bone_first_noise);
	aimbotSettings.chanceForClosestBone = (config->weapons[weaponId].aimbot.chance_hit_closest_bone_first * noise);

	noise = Aimbot::transformNoise(config->aimbot.first_position_of_critical_hit_noise);
	aimbotSettings.firstHitIsCriticalAccuracy = (config->weapons[weaponId].aimbot.first_position_of_critical_hit * noise);

	if (randomFloat(0.0f, 1.0f) > aimbotSettings.hitAccuracyWhileMoving) {
		aimbotSettings.firstHitIsCriticalAccuracy = (aimbotSettings.firstHitIsCriticalAccuracy * aimbotSettings.hitAccuracyWhileMoving); // not tested!!!
	}

	if (selectedTargetBoneId == -1) {
		if (randomFloat(0.0f, 1.0f) <= aimbotSettings.firstHitIsCriticalAccuracy) {
			selectedTargetBoneId = 8;
		} else if (randomFloat(0.0f, 1.0f) > aimbotSettings.chanceForClosestBone) {
			if (config->aimbot.num_primary_bones_choice >= 1 && config->aimbot.num_primary_bones_choice <= 3) {
				int randomBone = Aimbot::randomInt(0, 100);
				int boneIndex = 0;
				if (randomBone > 50 && randomBone <= 75 && config->aimbot.num_primary_bones_choice >= 2) {
					boneIndex = 1;
				} else if(randomBone > 75 && config->aimbot.num_primary_bones_choice >= 3) {
					boneIndex = 2;
				}
				//int boneIndex = Aimbot::randomInt(0, (config->aimbot.num_primary_bones_choice - 1));
				int boneId = config->weapons[weaponId].aimbot.primary_bones[boneIndex];
				if (boneId >= 0 && boneId <= 10) {
					selectedTargetBoneId = boneId;
				}
			}
		}
	}

	noise = Aimbot::transformNoise(config->aimbot.delay_to_stop_aiming_noise);
	aimbotSettings.delayToStopAiming = static_cast<int>(config->weapons[weaponId].aimbot.delay_to_stop_aiming * noise);

	noise = Aimbot::transformNoise(config->aimbot.divergence_at_first_shot_noise);
	//aimbotSettings.fov = (Aimbot::randomFloat(config->weapons[weaponId].aimbot.divergence_at_first_shot, (config->weapons[weaponId].aimbot.divergence_at_first_shot * 2)) * noise);
	aimbotSettings.fov = ((config->weapons[weaponId].aimbot.divergence_at_first_shot * 2) * noise);

	//noise = Aimbot::transformNoise(config->aimbot.fov_noise);
	//aimbotSettings.fov = (config->weapons[weaponId].aimbot.fov * noise);

	noise = Aimbot::transformNoise(config->aimbot.recoil_scale_noise);
	//aimbotSettings.recoilScale = (config->weapons[weaponId].aimbot.recoil_scale * noise);
	aimbotSettings.recoilScale = (config->weapons[weaponId].aimbot.recoil_scale * noise) - (0.1f * aimbotSettings.hitAccuracyWhileMoving); // not tested!!!

	noise = Aimbot::transformNoise(config->aimbot.spiral_aim_noise);
	aimbotSettings.spiralAimX = (config->aimbot.spiral_aim_x * noise);
	aimbotSettings.spiralAimY = (config->aimbot.spiral_aim_y * noise);

	noise = Aimbot::transformNoise(config->aimbot.divergence_at_first_shot_noise);
	aimbotSettings.divergenceIfSpotted = (config->weapons[weaponId].aimbot.divergence_if_spotted * noise);

	noise = Aimbot::transformNoise(config->aimbot.delay_to_start_aiming_noise);
	aimbotSettings.delayToStartAiming = static_cast<int>(config->weapons[weaponId].aimbot.divergence_if_spotted * noise);

	noise = Aimbot::transformNoise(config->aimbot.time_to_kill_noise); 
	aimbotSettings.aimTimePerDegree = (config->weapons[weaponId].aimbot.aim_time_per_degree * noise);
	aimbotSettings.smooth = aimbotSettings.aimTimePerDegree / 10;

	//noise = Aimbot::transformNoise(config->aimbot.smooth_noise);
	//aimbotSettings.smooth = (config->weapons[weaponId].aimbot.smooth * noise);

	if (aimbotSettings.inaccuracyAction == INACCURACY_ACTION_PAUSE) {
		if (aimbotSettings.inaccuracyPause == 0) {
			aimbotSettings.inaccuracyPause = (config->weapons[weaponId].aimbot.inaccuracy_offset * INACCURACY_OFFSET_PAUSE_STEP);
		}
		aimbotSettings.inaccuracyOffset = 0.0f;
	} else if (aimbotSettings.inaccuracyAction == INACCURACY_ACTION_ANGLES) {
		aimbotSettings.inaccuracyPause = 0;
		aimbotSettings.inaccuracyOffset = (config->weapons[weaponId].aimbot.inaccuracy_offset * INACCURACY_OFFSET_ANGLES_STEP);
	} else if (aimbotSettings.inaccuracyAction == INACCURACY_ACTION_STARTBULLET) {
		aimbotSettings.inaccuracyPause = 0;
		aimbotSettings.inaccuracyOffset = 0.0f;
		if (config->weapons[weaponId].aimbot.inaccuracy_offset) {
			aimbotSettings.startBullet = aimbotSettings.startBullet + 1 + (config->weapons[weaponId].aimbot.inaccuracy_offset / 5);
		}
	}

	aimbotSettings.startBullet = aimbotSettings.startBullet + 1 + (config->weapons[weaponId].aimbot.inaccuracy_offset_for_first_shot / 5);

	//aimbotSettings.inaccuracyOffset = aimbotSettings.inaccuracyOffset + ((config->aimbot.max_inaccuracy_offset > 0.0001f && config->aimbot.max_inaccuracy_offset < 0.05f) ? (config->aimbot.max_inaccuracy_offset * (1.0f - aimbotSettings.hitAccuracyWhileMoving)) : 0.0f);
}

/**
	Determines the closest player based on the given bone Id.

	@param boneId the id of the given bone
	@return the index of the closest player
*/
int Aimbot::getPlayerClosestToCrosshair(int boneId) {
	if (aimbotSettings.saveTargetAim) {
		if (selectedTargetIndex != -1 && entityList[selectedTargetIndex].isVisible()) {
			return selectedTargetIndex;
		}
	}

	int targetIndex = -1;
	float bestFov = aimbotSettings.fov;

	for (int i = 0; i < game->getMaxClients(); i++) {
		if (!entityList[i].isValid())
			continue;

		if (!config->general.team_damage) {
			if (entityList[i].getTeamNum() == localEntity.getTeamNum())
				continue;

			if (!entityList[i].isVisible())
				continue;
		} else {
			if (entityList[i].getIndex() == localEntity.getIndex())
				continue;

			if (!entityList[i].isBSPVisible())
				continue;
		}

		Vector ViewAngles;
		game->getViewAngles(ViewAngles);

		float tempFov = getFieldOfView(ViewAngles, localEntity.getEyePosition(), entityList[i].getBonePosition(boneId));
		if (tempFov < bestFov) {
			bestFov = tempFov;
			targetIndex = i;
		}
	}
	return targetIndex;
}

/**
	Determines the closest player and his bone.
	Saves the id of the bone in a global variable.

	@return the index of the closest player
*/
int Aimbot::getBoneClosestToCrosshair() {
	if (aimbotSettings.saveTargetAim) {
		if (selectedTargetIndex != -1 && entityList[selectedTargetIndex].isVisible()) {
			return selectedTargetIndex;
		}
	}

	int targetIndex = -1;
	int bestBoneId = -1;

	float maxFov = aimbotSettings.fov;
	const int size = 6;
	int boneIds[size] = { 8, 7, 6, 5, 4, 3 };

	for (int i = 0; i < game->getMaxClients(); i++) {

		if (!entityList[i].isValid())
			continue;

		if (!config->general.team_damage) {
			if (entityList[i].getTeamNum() == localEntity.getTeamNum())
				continue;

			if (!entityList[i].isVisible())
				continue;
		} else {
			if (entityList[i].getIndex() == localEntity.getIndex())
				continue;

			if (!entityList[i].isBSPVisible())
				continue;
		}

		//Console::getInstance().logln(LogLevel::LOG_ALL, "GetBoneClosestToCrosshair -> " + std::to_string(i));

		Vector ViewAngles, PunchAngles;
		game->getViewAngles(ViewAngles);
		PunchAngles = localEntity.getPunchAngles();
		PunchAngles.z = 0.0f;
		ViewAngles -= PunchAngles * 2;

		for (int j = 0; j < size; j++) {
			float tempFov = getFieldOfView(ViewAngles, localEntity.getEyePosition(), entityList[i].getBonePosition(boneIds[j]));
			if (tempFov < maxFov) {
				maxFov = tempFov;
				bestBoneId = boneIds[j];
				targetIndex = i;
			}
		}
	}

	if (targetIndex != -1) {
		if (selectedTargetBoneId == -1) {
			selectedTargetBoneId = bestBoneId;
		}
	} else {
		selectedTargetBoneId = -1;
	}
	return targetIndex;
}

/**
	Deletes the current memorized target.

	@return nothing.
*/
void Aimbot::dropSelectedTarget() {
	selectedTargetIndex = -1;
	selectedTargetBoneId = -1;
	aimbotSettings.inaccuracyAction = -1;
	aimbotSettings.inaccuracyPause = 0;
	aimbotSettings.startToAimTime = 0;
}

/**
	Calculates if a next attack is allowed.

	@return true if the next attack is allowed, otherwise false.
*/
bool Aimbot::isNextShootAllowed() {
	DWORD weapBase = localEntity.getActiveWeaponBase();
	float nextPrimaryAttack = memory->Read<float>(weapBase + offsets->m_flNextPrimaryAttack);
	float tickBase = memory->Read<float>(localEntity.getPointer() + offsets->m_nTickBase);
	float intervalPerTick = game->getIntervalPerTick();
	float flServerTime = tickBase * intervalPerTick;
	return (!(nextPrimaryAttack > flServerTime));
}

/**
	Calculates the smooth value based on the given aimbot angles.
	The smooth value divides the entire movement into many small movements.

	@param aimbotAngles the calculated aimbot angles
	@return the smooth value
*/
float Aimbot::getSmoothValue(Vector &aimbotAngles) {
	bool adaptive_smooth_enable = false;
	float smoothValue = aimbotSettings.smooth;
	int weaponTypeId = localEntity.getWeaponType();

	switch (weaponTypeId) {
		case EWeaponType::WeaponType_Rifle:
			if (config->aimbot.adaptive_smooth_rifles)
				adaptive_smooth_enable = true;
			break;
		case EWeaponType::WeaponType_Pistol:
			if (config->aimbot.adaptive_smooth_pistols)
				adaptive_smooth_enable = true;
			break;
		case EWeaponType::WeaponType_SMG:
			if (config->aimbot.adaptive_smooth_smgs)
				adaptive_smooth_enable = true;
			break;
		case EWeaponType::WeaponType_Sniper:
			if (config->aimbot.adaptive_smooth_snipers)
				adaptive_smooth_enable = true;
			break;
		case EWeaponType::WeaponType_Shotgun:
			if (config->aimbot.adaptive_smooth_shotguns)
				adaptive_smooth_enable = true;
			break;
		case EWeaponType::WeaponType_LMG:
			if (config->aimbot.adaptive_smooth_lmgs)
				adaptive_smooth_enable = true;
			break;
		default:
			break;
	}

	if (adaptive_smooth_enable)	{
		Vector currentViewAngles; game->getViewAngles(currentViewAngles);
		Vector viewAnglesDelta = currentViewAngles - aimbotAngles;
		float adaptiveSmooth = abs(viewAnglesDelta.LengthXY()) * config->aimbot.adaptive_smooth_multiplier;
		smoothValue += adaptiveSmooth;
	}

	return smoothValue;
}

/**
	Calculates the angles on which the aimbot should aim automatically.

	@return the angles as a vector
*/
Vector Aimbot::getAimbotAngles() {
	if (selectedTargetIndex != -1 && !entityList[selectedTargetIndex].isValid()) {
		Aimbot::dropSelectedTarget();
	}

	if (!localEntity.getActiveWeaponAmmo()) {
		if (aimbotSettings.weaponEmptyTime == 0) {
			aimbotSettings.weaponEmptyTime = Aimbot::getCurrentTimeInMilliseconds() + aimbotSettings.delayToStopAiming;
		} else {
			if (Aimbot::getCurrentTimeInMilliseconds() > aimbotSettings.weaponEmptyTime) {
				aimbotSettings.weaponEmptyTime = 0;
				return Vector(0, 0, 0);
			}
		}
	} else {
		aimbotSettings.weaponEmptyTime = 0;
	}
		
	if (selectedTargetBoneId >= 0) {
		selectedTargetIndex = getPlayerClosestToCrosshair(selectedTargetBoneId);

		int closestBoneId = Aimbot::getClosestBoneId(selectedTargetIndex, 1.0);
		if (selectedTargetBoneId != closestBoneId) {
			selectedTargetBoneId = closestBoneId;
		}

		aimbotSettings.inaccuracyAction = Aimbot::randomInt(INACCURACY_ACTION_PAUSE, INACCURACY_ACTION_STARTBULLET);
	} else {
		selectedTargetIndex = getBoneClosestToCrosshair();
		aimbotSettings.inaccuracyAction = Aimbot::randomInt(INACCURACY_ACTION_PAUSE, INACCURACY_ACTION_STARTBULLET);
	}

	if (aimbotSettings.inaccuracyPause > 1) {
		aimbotSettings.inaccuracyPause--;
		return Vector(0, 0, 0);
	}
	
	if (selectedTargetIndex == -1) {
		return Vector(0, 0, 0);
	}

	if (aimbotSettings.angleFirstEncounter[selectedTargetIndex] > aimbotSettings.divergenceIfSpotted) {
		//selectedTargetBoneId = 4; //TODO: Kann man aktivieren, wenn man es für sinnvoll hält.
		if (aimbotSettings.startToAimTime == 0) {
			aimbotSettings.startToAimTime = (Aimbot::getCurrentTimeInMilliseconds() + static_cast<INT64>(aimbotSettings.angleFirstEncounter[selectedTargetIndex] - aimbotSettings.delayToStartAiming));
		}
		if (Aimbot::getCurrentTimeInMilliseconds() < aimbotSettings.startToAimTime) {
			return Vector(0, 0, 0);
		}
	}
		
	// Check if bones are actually updated
	Vector enemyPosition = entityList[selectedTargetIndex].getBonePosition(selectedTargetBoneId);
	Vector targetOrigin = entityList[selectedTargetIndex].getOrigin();

	float check = (float)abs((targetOrigin - enemyPosition).Length());

	if (check > 75.0f) {
		return Vector(0, 0, 0);
	}

	Vector position = localEntity.getEyePosition();

	// Compensate Velocity 
	if (aimbotSettings.compensateVelocity) {
		Vector distVec = position - enemyPosition;
		float dist = distVec.Length();
		if (dist > 0.001f) {
			Vector myVelocity = localEntity.getVelocity();
			Vector targetVelocity = entityList[selectedTargetIndex].getVelocity();
			enemyPosition.x += (targetVelocity.x) / dist;
			enemyPosition.y += (targetVelocity.y) / dist;
			enemyPosition.z += (targetVelocity.z) / dist;
			enemyPosition.x -= (myVelocity.x) / dist;
			enemyPosition.y -= (myVelocity.y) / dist;
			enemyPosition.z -= (myVelocity.z) / dist;
		}
	}

	// Calculate Angle to look at
	Vector ViewAngles = Aimbot::calculateAngles(position, enemyPosition);
	//printf("ViewAngles: %f %f %f\n", ViewAngles.x, ViewAngles.y, ViewAngles.z);

	ViewAngles.Normalize();
	//printf("ViewAngles: %f %f %f\n", ViewAngles.x, ViewAngles.y, ViewAngles.z);

	// RCS (Recoil Control System)

	/* Recoil Control System
	 * NoRecoil and Recoil Control System is almost the same. For RCS you smooth down your angles - for norecoil not
	 * Source: https://www.unknowncheats.me/forum/counterstrike-global-offensive/157776-recoil-recoil-control-system.html
	 */

	Vector PunchAngles = localEntity.getPunchAngles();
	PunchAngles.z = 0.0f;
	//printf("PunchAngles: %f %f %f\n", PunchAngles.x, PunchAngles.y, PunchAngles.z);

	ViewAngles -= PunchAngles * aimbotSettings.recoilScale;
	//printf("ViewAngles: %f %f %f\n", ViewAngles.x, ViewAngles.y, ViewAngles.z);

	ViewAngles.Normalize();
	//printf("ViewAngles: %f %f %f\n", ViewAngles.x, ViewAngles.y, ViewAngles.z);

	/* Spiral-Aim
	 * Makes an aimbot less linear; This will basically create a spiral by rotating your viewangle delta by 90 degrees, scaling it down and then adding it back to the viewangles.
	 * SOURCE https://www.unknowncheats.me/forum/counterstrike-global-offensive/182226-spiral-aim.html
	 */

	float SMOOTH_VALUE = Aimbot::getSmoothValue(ViewAngles);

	Vector currentViewAngles;
	game->getViewAngles(currentViewAngles);

	Vector viewAnglesDelta = ViewAngles - currentViewAngles;
	viewAnglesDelta.Normalize();

	//printf("(1) vector {%f %f %f} length %f\n", viewAnglesDelta[0], viewAnglesDelta[1], viewAnglesDelta[2], viewAnglesDelta.LengthXY());

	Vector temp = viewAnglesDelta;
	temp.Norm();

	//printf("(2) vector {%f %f %f} length %f\n", temp[0], temp[1], temp[2], temp.LengthXY());

	if (config->aimbot.spiral_aim_enabled) {
		if (config->aimbot.spiral_aim_relative_position >= 1.0f) {
			if (viewAnglesDelta.y >= 0.0f) {
				viewAnglesDelta -= {viewAnglesDelta.y / aimbotSettings.spiralAimX, viewAnglesDelta.x / aimbotSettings.spiralAimY, 0.0f};
			} else {
				viewAnglesDelta += {viewAnglesDelta.y / aimbotSettings.spiralAimX, viewAnglesDelta.x / aimbotSettings.spiralAimY, 0.0f};
			}
		} else {
			if (viewAnglesDelta.y >= 0.0f) {
				viewAnglesDelta += {viewAnglesDelta.y / aimbotSettings.spiralAimX, viewAnglesDelta.x / aimbotSettings.spiralAimY, 0.0f};
			} else {
				viewAnglesDelta -= {viewAnglesDelta.y / aimbotSettings.spiralAimX, viewAnglesDelta.x / aimbotSettings.spiralAimY, 0.0f};
			}
		}
		viewAnglesDelta.Normalize();
	}

	//printf("(2) vector {%f %f %f} length %f\n", viewAnglesDelta[0], viewAnglesDelta[1], viewAnglesDelta[2], viewAnglesDelta.LengthXY());

	viewAnglesDelta /= SMOOTH_VALUE;
	viewAnglesDelta.Normalize();

	//ViewAngles = currentViewAngles + viewAnglesDelta;
	ViewAngles.x = currentViewAngles.x + viewAnglesDelta.x;// +(viewAnglesDelta.x * aimbotSettings.inaccuracyOffset);
	ViewAngles.y = currentViewAngles.y + viewAnglesDelta.y;// +(viewAnglesDelta.y * aimbotSettings.inaccuracyOffset);
	ViewAngles.z = 0.0f;

	//printf("aimbotSettings.inaccuracyOffset %f\n", aimbotSettings.inaccuracyOffset);

	ViewAngles.Normalize();
	return ViewAngles;
}

/**
	Calculates the x and y position to move the mouse to.

	@return the x and y position to move the mouse to.
*/
Vector Aimbot::getAimbotMousePosition() {
	static Vector anglesDelta;
	game->getViewAngles(anglesDelta);
	anglesDelta -= anglesToAimAt;

	float	x = anglesDelta.y / (0.022f * 2 * 1.0f),
			y = -anglesDelta.x / (0.022f * 2 * 1.0f);

	static unsigned int nextMoveOnX = 0,
						nextMoveOnY = 0;

	if (abs(x) < 1.f && GetTickCount() > nextMoveOnX) {
		x = (x > 0) ? 1 : ((x < 0) ? -1 : x);
		nextMoveOnX = GetTickCount() + 1;
	}

	if (abs(y) < 1.f && GetTickCount() > nextMoveOnY) {
		y = (y > 0) ? 1 : ((y < 0) ? -1 : y);
		nextMoveOnY = GetTickCount() + 1;
	}
	return Vector(x, y, 0.0f);
}

/**
	Checks if the Aimbot should be paused for a given time. That is the case when the last target is dead.

	@return true if the aimbot should be pause, otherwise false
*/
bool Aimbot::isDelayNeeded() {
	if (lastSelectedTargetIndex != -1 && !entityList[lastSelectedTargetIndex].isValid()) {
		Sleep(aimbotSettings.delayAfterKill);
		return true;
	}
	return false;
}

/**
	Sets the position of the cursor to the specified points.

	@param x the x value
	@param y the y value
	@return nothing
*/
void Aimbot::moveMouseToPosition(float x, float y) {
	INPUT input = { 0 };
	input.mi.mouseData = 0;
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	input.type = INPUT_MOUSE;
	input.mi.dx = static_cast<long>(roundf(x));
	input.mi.dy = static_cast<long>(roundf(y));
	input.mi.dwExtraInfo = 0;
	input.mi.time = 0;
	SendInput(1, &input, sizeof(INPUT));
}

/**
	Transforms the noise in percent into a float.

	@param noise the noise in percent
	@return the noise as a float
*/
float Aimbot::transformNoise(float noise) {
	return 1.0f + (randomFloat(-noise, noise) / 100);
}

/**
	Calculates a random float between two floats.

	@param a min float
	@param b max float
	@return the random float
*/
float Aimbot::randomFloat(float a, float b) {
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
int Aimbot::randomInt(int a, int b) {
	std::random_device random;
	std::mt19937 randomGenerator(random());
	std::uniform_int_distribution<int> randomDistribute(a, b);
	return randomDistribute(randomGenerator);
}

/**
	Calculates the elapsed time in milliseconds since 01/01/1970.

	@return the calculated time in milliseconds
*/
INT64 Aimbot::getCurrentTimeInMilliseconds() {
	auto now = system_clock::now();
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
	return ms;
}

/**
	Calculates the distance between the current view angles and the view angles the player must have to look at the target.

	@param viewAngles the view angles of the player
	@param src the position of the player
	@param dst the position of the target
	@return the calculated distance
*/
float Aimbot::getFieldOfView(Vector viewAngles, Vector src, Vector dst) {
	Vector aimVector, viewVector;
	aimVector = Aimbot::calculateAngles(src, dst);

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
Vector Aimbot::calculateAngles(Vector src, Vector dst) {
	Vector vDelta = src - dst;
	float fHyp = vDelta.LengthXY();

	float yaw = (atanf(vDelta.y / vDelta.x)) * (180.0f / (float)M_PI);
	float pitch = (atanf(vDelta.z / fHyp)) * (180.0f / (float)M_PI);

	if (vDelta.x >= 0.0f)
		yaw += 180.0f;

	return Vector(pitch, yaw, 0.0f);
}

/**
	Calculates the angle from the local player to the player with the given index.

	@param targetIndex the index of the target player
	@return the angle to the target player
*/
float Aimbot::getAngleToPlayer(int targetIndex) {
	if (!entityList[targetIndex].isValid())
		return NULL;

	if (!entityList[targetIndex].isVisible())
		return NULL;

	Vector ViewAngles, PunchAngles;
	game->getViewAngles(ViewAngles); //Gets the direction the player is looking in.
	PunchAngles = localEntity.getPunchAngles();
	PunchAngles.z = 0.0f;
	ViewAngles -= PunchAngles * 2;

	const int numBones = 4;
	int bones[numBones] = { -1, 0, 6, 8 };
	float bestFov = 1000.f;
	for (int boneId = 0; boneId < numBones; boneId++) {
		float tempFov = getFieldOfView(ViewAngles, localEntity.getEyePosition(), entityList[targetIndex].getBonePosition(boneId));
		if (tempFov < bestFov) {
			bestFov = tempFov;
		}
	}

	return bestFov;
}

/**
	Look for the closest bone (body part) of a player.

	@param targetIndex the index of the target player
	@return the index of the closest bone from the target player, -1 if nothing was found.
*/
int Aimbot::getClosestBoneId(int targetIndex, float maxDistance) {
	//Console::getInstance().appendToLogFile("getClosestBoneId");
	int bestBoneId = -1;

	if (targetIndex < 0 || targetIndex > 64)
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