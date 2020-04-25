#include "stdafx.h"

/**
	Entry point from the triggerbot module.
	Runs in a loop while the main program is alive.

	The loop repeats every 1 millisecond.

	The triggerbot is triggered here when the user presses the designated key and is on a target.

	@return nothing
*/
void Triggerbot::start() {
	try {
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (config->general.panic_mode)
				continue;

			if (!config->general.triggerbot_enabled)
				continue;

			if (!game->isInGame())
				continue;

			if (localEntity.isFlashed())
				continue;

			if (localEntity.isDead())
				continue;
			
			int weaponId = localEntity.getActiveWeapon();

			for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
				if (!entityList[playerIndex].isValid())
					continue;

				if (entityList[playerIndex].getTeamNum() == localEntity.getTeamNum())
					continue;

				if (entityList[playerIndex].isVisible()) {
					if (triggerbotSettings.timeToKill[playerIndex] == 0) {
						float angleToTarget = Triggerbot::getAngleToPlayer(playerIndex);
						if (angleToTarget < MAX_ANGLE_FOR_FIRST_ENCOUNTER) {
							triggerbotSettings.timeToKill[playerIndex] = Triggerbot::getCurrentTimeInMilliseconds();
						}
					}
				} else {
					triggerbotSettings.timeToKill[playerIndex] = 0;
				}
			}

			if (GetAsyncKeyState(config->keys.triggerbot_hold) & 0x8000) {
				int playerIndex = (config->triggerbot.fov_based) ? Triggerbot::getPlayerInFieldOfView() : Triggerbot::getPlayerIndexAimingOn();
				//Console::getInstance().println("Triggerbot:	playerIndex: " + std::to_string(playerIndex));
				if (playerIndex != -1) {
					float noise = Triggerbot::transformNoise(config->aimbot.time_to_kill_noise);
					INT64 delay = static_cast<INT64>(config->weapons[weaponId].aimbot.time_to_kill * noise) - (Triggerbot::getCurrentTimeInMilliseconds() - triggerbotSettings.timeToKill[playerIndex]);
					if (delay < MIN_LEGIT_DELAY || delay > MAX_LEGIT_DELAY) {
						delay = static_cast<INT64>(config->triggerbot.delay * noise);
						if (delay < MIN_LEGIT_DELAY || delay > MAX_LEGIT_DELAY) {
							delay = Triggerbot::randomInt(MIN_LEGIT_DELAY, MAX_LEGIT_DELAY);
						}
					}
					Sleep(static_cast<DWORD>(delay));
					game->pressAttackKey();
					Sleep(10);
					game->releaseAttackKey();
					Sleep(5);
				}
			}
		}
	}
	catch (...) {
		memory->debuglog(__FILE__);
	}
}

/**
	Finds the player targeted by the local player.

	@return the index of the player to which the local player is aiming, otherwise -1
*/
int Triggerbot::getPlayerIndexAimingOn() {
	int targetIndex = memory->Read<int>(localEntity.getPointer() + offsets->m_iCrossHairID);
	if (targetIndex != 0 && targetIndex != localEntity.getIndex()) {
		for (int i = 0; i < game->getMaxClients(); i++) {
			//std::this_thread::yield();
			if (entityList[i].getIndex() == targetIndex) {
				if (entityList[i].getHealth() > 0 && (config->general.team_damage || entityList[i].getTeamNum() != localEntity.getTeamNum()))
					return i;
			}
		}
	}
	return -1;
}

/**
	Checks if a player is in the field of view of the local player.

	@return the index of the player if there is a player in the field of view, otherwise -1
*/
int Triggerbot::getPlayerInFieldOfView() {
	Vector current, PunchAngles;
	game->getViewAngles(current);

	PunchAngles = localEntity.getPunchAngles();
	PunchAngles.z = 0.0f;
	current -= PunchAngles * 2;

	for (int i = 0; i < game->getMaxClients(); i++) {
		if (!config->general.team_damage && entityList[i].getTeamNum() == localEntity.getTeamNum())
			continue;

		if (entityList[i].getIndex() == localEntity.getIndex())
			continue;

		if (!entityList[i].isValid())
			continue;

		if (isBoneInFieldOfView(current, i, 8, 3))
			return i;

		if (isBoneInFieldOfView(current, i, 6, 5))
			return i;

		if (isBoneInFieldOfView(current, i, 4, 4))
			return i;

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
bool Triggerbot::isBoneInFieldOfView(Vector &current, int m_target, int bone, float fov) {
	auto direction = entityList[m_target].getBonePosition(bone) - localEntity.getEyePosition();
	pMath.vectorNormalize(direction);

	auto target = pMath.vectorToAngles(direction);
	pMath.clampAngles(target);

	Vector v_dist = localEntity.getOrigin() - entityList[m_target].getOrigin();
	float distance = v_dist.Length();

	// modify 20.f as needed, depending on bone, random atm
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
bool Triggerbot::canHit(Vector current, Vector target, float distance, float max) {
	auto delta = target - current;
	pMath.clampAngles(delta);
	auto xdist = sinf(DEG2RAD(delta.Length())) * distance / 2.f;
	return xdist <= max;
}

/**
	Calculates the angle from the local player to the player with the given index.

	@param targetIndex the index of the target player
	@return the angle to the target player
*/
float Triggerbot::getAngleToPlayer(int targetIndex) {
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
	Calculates the distance between the current view angles and the view angles the player must have to look at the target.

	@param viewAngles the view angles of the player
	@param src the position of the player
	@param dst the position of the target
	@return the calculated distance
*/
float Triggerbot::getFieldOfView(Vector viewAngles, Vector src, Vector dst) {
	Vector aimVector, viewVector;
	aimVector = Triggerbot::calculateAngles(src, dst);

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
Vector Triggerbot::calculateAngles(Vector src, Vector dst) {
	Vector vDelta = src - dst;
	float fHyp = vDelta.LengthXY();

	float yaw = (atanf(vDelta.y / vDelta.x)) * (180.0f / (float)M_PI);
	float pitch = (atanf(vDelta.z / fHyp)) * (180.0f / (float)M_PI);

	if (vDelta.x >= 0.0f)
		yaw += 180.0f;

	return Vector(pitch, yaw, 0.0f);
}

/**
	Calculates the elapsed time in milliseconds since 01/01/1970.

	@return the calculated time in milliseconds
*/
INT64 Triggerbot::getCurrentTimeInMilliseconds() {
	auto now = system_clock::now();
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
	return ms;
}

/**
	Transforms the noise in percent into a float.

	@param noise the noise in percent
	@return the noise as a float
*/
float Triggerbot::transformNoise(float noise) {
	return 1.0f + (randomFloat(-noise, noise) / 100);
}

/**
	Calculates a random float between two floats.

	@param a min float
	@param b max float
	@return the random float
*/
float Triggerbot::randomFloat(float a, float b) {
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
int Triggerbot::randomInt(int a, int b) {
	std::random_device random;
	std::mt19937 randomGenerator(random());
	std::uniform_int_distribution<int> randomDistribute(a, b);
	return randomDistribute(randomGenerator);
}