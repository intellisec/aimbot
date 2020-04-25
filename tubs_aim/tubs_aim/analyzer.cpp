#include "stdafx.h"
#include "dllmain.h"

/**
	Entry point from the analyzer module.

	Here, the player profile is created with the recorded values.

	@return nothing
*/
void Analyzer::start() {
	try {
		Analyzer::initAnalyzerValues();
		Analyzer::decreaseNextImprovementTime(0);
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			if (config->general.panic_mode)
				continue;

			//if (adjustmentTestCounter < 50) {
			//	Analyzer::improvePlayerProfile();
			//}

			if (config->keys.profiler_toggle != 0 && GetAsyncKeyState(config->keys.profiler_toggle)) {
				config->general.profiler_enabled = !config->general.profiler_enabled;
				if (config->general.profiler_enabled) {
					//config->general.aimbot_enabled = false;
					//config->general.triggerbot_enabled = false;
					Beep(1000, 200);
				} else {
					Beep(500, 200);
				}
				Console::getInstance().printInfo();
				Sleep(200);
			}

			static bool analyzerKeyPressed = false;
			if (config->keys.start_analyzer != 0 && GetAsyncKeyState(config->keys.start_analyzer)) {
				if (!analyzerKeyPressed) {
					if (!isAnalyzing) {
						Beep(1000, 200);
						Analyzer::analyze();
						Beep(500, 200);
					}
					else {
						Console::getInstance().println("Analyzer is already running! Please wait...");
					}
					Sleep(200);
					analyzerKeyPressed = true;
				}
			} else {
				analyzerKeyPressed = false;
			}

			static bool adjustmentKeyPressed = false;
			if (config->keys.start_adjustment != 0 && GetAsyncKeyState(config->keys.start_adjustment)) {
				if (!adjustmentKeyPressed) {
					if (!isAdjusting) {
						Beep(1000, 200);
						config->analyzer.next_adjustment = 0;
						Analyzer::decreaseNextImprovementTime(0);
						Beep(500, 200);
					} else {
						Console::getInstance().println("Adjustment is already running! Please wait...");
					}
					Sleep(200);
					adjustmentKeyPressed = true;
				}
			} else {
				adjustmentKeyPressed = false;
			}

			if (!Helper::getInstance().isCSGOWindowActive())
				continue;

			if (!game->isInGame())
				continue;

			if (!localEntity.isValidForProfiler())
				continue;

			/*
			Analyzer::increasePlayedTime(100);

			int hitOnBone[9] = { 5, 3, 3, 9, 8, 7, 1, 2, 3 };
			int hitOnBoneIndexes[3] = { };
			Analyzer::sortArrayAndGetIndexArray(hitOnBone, 9, hitOnBoneIndexes, 3);
			for (int i = 0; i < 3; i++) {
				Console::getInstance().log(LogLevel::LOG_ALL, std::to_string(hitOnBoneIndexes[i]) + " ");
			}
			
			Console::getInstance().logln(LogLevel::LOG_ALL, "getCurrentRoundNum " + std::to_string(Analyzer::getCurrentRoundNum()));
			Console::getInstance().logln(LogLevel::LOG_ALL, "isDangerMode " + std::to_string(Analyzer::isDangerMode()));
			Console::getInstance().logln(LogLevel::LOG_ALL, "isMatchmaking " + std::to_string(Analyzer::isMatchmaking()));
			Console::getInstance().logln(LogLevel::LOG_ALL, "getSurvivalTeamNum " + std::to_string(localEntity.getSurvivalTeamNum()));
			Console::getInstance().logln(LogLevel::LOG_ALL, "getCompetitiveWins " + std::to_string(localEntity.getCompetitiveWins()));
			Console::getInstance().logln(LogLevel::LOG_ALL, "getTeamNum " + std::to_string(localEntity.getTeamNum()));
			*/

			if (Analyzer::isMatchmaking()) {
				int currentRoundNum = Analyzer::getCurrentRoundNum();
				if (lastRoundNum < currentRoundNum) {
					if (recordingMatchmakingStarted) {
						int myTeamId = localEntity.getTeamNum();
						int enemyTeamId = Analyzer::getOppositeTeamId(myTeamId);
						int myWins = Analyzer::getWinningRoundsByTeam(myTeamId);
						int enemyWins = Analyzer::getWinningRoundsByTeam(enemyTeamId);
						Console::getInstance().logln(LogLevel::LOG_ERROR, "next round started... (" + std::to_string(myWins) + " - " + std::to_string(enemyWins) + ")");
						if (myWins == 16) {
							recordingMatchmakingStarted = false;
							lastRoundNum = 0;
							Analyzer::increasePlayedMatchmakings(MM_ENDRESULT_WIN);
						} else if (enemyWins == 16) {
							recordingMatchmakingStarted = false;
							lastRoundNum = 0;
							Analyzer::increasePlayedMatchmakings(MM_ENDRESULT_LOSE);
						} else if (myWins == 15 && enemyWins == 15) {
							recordingMatchmakingStarted = false;
							lastRoundNum = 0;
							Analyzer::increasePlayedMatchmakings(MM_ENDRESULT_DRAW);
						} else {
							lastRoundNum = currentRoundNum;
						}
						Analyzer::increasePlayedTime(std::time(0) - lastPlayTime);
						Analyzer::decreaseNextImprovementTime(std::time(0) - lastPlayTime);
						lastPlayTime = std::time(0);
					} else {
						if (currentRoundNum >= 1 && lastRoundNum == 0 && localEntity.isValidForProfiler()) {
							lastRoundNum = currentRoundNum;
							lastPlayTime = std::time(0);
							recordingMatchmakingStarted = true;
						}
					}
				}
			} else {
				if (config->general.profiler_enabled && localEntity.isValid()) {
					config->general.profiler_enabled = false;
				}
			}
		}
	} catch (...) {
		memory->debuglog(__FILE__);
	}
}

/**
	Sets the values of the analyzer to default values at the beginning.

	@return nothing
*/
void Analyzer::initAnalyzerValues() {
	isAnalyzing = false;
	recordingMatchmakingStarted = false;
	lastRoundNum = 0;
	lastPlayTime = 0;
}

/**
	Increases the number of played matchmakings and the number of (won, lost, undecided) matchmakings.

	@param endResultId the id of the result of the matchmaking
	@return nothing
*/
void Analyzer::increasePlayedMatchmakings(int endResultId) {
	config->analyzer.played_matchmakings = config->analyzer.played_matchmakings + 1;
	if (endResultId == MM_ENDRESULT_WIN) {
		config->analyzer.matchmakings_wins = config->analyzer.matchmakings_wins + 1;
	} else if (endResultId == MM_ENDRESULT_LOSE) {
		config->analyzer.matchmakings_loses = config->analyzer.matchmakings_loses + 1;
	} else if (endResultId == MM_ENDRESULT_DRAW) {
		config->analyzer.matchmakings_draws = config->analyzer.matchmakings_draws + 1;
	}
	config->saveConfig();
	Console::getInstance().logln(LogLevel::LOG_ERROR, "increasePlayedMatchmakings endResultId " + std::to_string(endResultId));

	Analyzer::decreaseNextImprovementTime(0);
}

/**
	Increases the number of play time in seconds.

	@param time the time in seconds
	@return nothing
*/
void Analyzer::increasePlayedTime(time_t time) {
	config->analyzer.played_time = config->analyzer.played_time + time;
	config->saveConfig();
	Console::getInstance().logln(LogLevel::LOG_ERROR, "increasePlayedTime time " + std::to_string(time));
}

/**
	Decreases the number of the next improvement delay in seconds.

	@param time the time in seconds
	@return nothing
*/
void Analyzer::decreaseNextImprovementTime(time_t time) {
	config->analyzer.next_adjustment = config->analyzer.next_adjustment - time;
	if (config->analyzer.next_adjustment <= 0 && recordingMatchmakingStarted == false) {
		Analyzer::improvePlayerProfile();
		Analyzer::setNextImprovementTime();
	}
	config->saveConfig();
	Console::getInstance().logln(LogLevel::LOG_ERROR, "decreaseNextImprovementTime time " + std::to_string(time));
}

/**
	Sets the delay for the next improvement.

	@return nothing
*/
void Analyzer::setNextImprovementTime() {
	config->analyzer.next_adjustment = Analyzer::randomInt(MIN_IMPROVEMENT_DELAY, MAX_IMPROVEMENT_DELAY);
	config->saveConfig();
	//Console::getInstance().println("setNextImprovementTime time " + std::to_string(config->analyzer.next_adjustment));
}

/**
	Returns the pointer to the memory for the gamerules.

	@return the pointer
*/
DWORD_PTR Analyzer::getGameRules() {
	return memory->Read<DWORD_PTR>(client->GetImage() + offsets->m_dwGameRulesProxy);
}

/**
	Returns an array of the results of each round.

	@return the array with the results of each round
*/
void Analyzer::getRoundResults(int results[]) {
	DWORD_PTR gameRules = Analyzer::getGameRules();
	memory->Read(gameRules + offsets->m_iMatchStats_RoundResults, results, sizeof(int[MAX_ROUNDS_PER_MATCHMAKING]));
	/*
	for (int i = 0; i < MAX_ROUNDS_PER_MATCHMAKING; i++) {
		Console::getInstance().logln(LogLevel::LOG_ALL, std::to_string(results[i]));
	}
	*/
}

/**
	Checks if the player is in 'danger zone' mode.

	@return true if the player is in the 'danger zone' mode, otherwise false
*/
bool Analyzer::isDangerMode() {
	DWORD_PTR gameRules = Analyzer::getGameRules();
	float result = memory->Read<float>(gameRules + offsets->m_flSurvivalStartTime);
	if (result > 0.f) {
		return true;
	}
	return false;
}

/**
	Returns the number of completed rounds.

	@return the number of completed rounds
*/
int Analyzer::getRoundsFinished() {
	DWORD_PTR gameRules = Analyzer::getGameRules();
	int rounds = memory->Read<int>(gameRules + offsets->m_totalRoundsPlayed);
	return rounds;
}

/**
	Returns the number of the current round.

	@return the number of the current round
*/
int Analyzer::getCurrentRoundNum() {
	return (Analyzer::getRoundsFinished() + 1);
}

/**
	Checks if the player is in 'matchmaking' mode.

	@return true if the player is in the 'matchmaking' mode, otherwise false
*/
bool Analyzer::isMatchmaking() {
	DWORD_PTR gameRules = Analyzer::getGameRules();
	int isMatchMaking = memory->Read<int>(gameRules + offsets->m_bIsQueuedMatchmaking);
	return isMatchMaking;
}

/**
	Returns the number of rounds won by a team.

	@param teamId the id of the team
	@return the number of rounds won by the given team
*/
int Analyzer::getWinningRoundsByTeam(int teamId) {
	int results[MAX_ROUNDS_PER_MATCHMAKING +1] = { };
	Analyzer::getRoundResults(results);

	int currentRoundNum = Analyzer::getCurrentRoundNum();
	int firstHalfTeamId = (currentRoundNum <= MAX_ROUNDS_PER_MATCHMAKING / 2) ? teamId : Analyzer::getOppositeTeamId(teamId);
	int secondHalfTeamId = (currentRoundNum > MAX_ROUNDS_PER_MATCHMAKING / 2) ? teamId : Analyzer::getOppositeTeamId(teamId);
	int wins = 0;

	teamId = firstHalfTeamId;

	for (int i = 0; i < MAX_ROUNDS_PER_MATCHMAKING; i++) {
		if (i == (MAX_ROUNDS_PER_MATCHMAKING / 2)) {
			teamId = secondHalfTeamId;
		}
		if (teamId == TEAMID_TERRORISTS) {
			if (results[i] == RESULTID_T_KILLS_CT || results[i] == RESULTID_T_DETONATES_BOMB || results[i] == RESULTID_T_TIME_IS_OVER) {
				wins++;
			}
		} else if (teamId == TEAMID_COUNTERTERRORISTS) {
			if (results[i] == RESULTID_CT_KILLS_T || results[i] == RESULTID_CT_DEFUSE_BOMB || results[i] == RESULTID_CT_TIME_IS_OVER || results[i] == RESULTID_CT_HOSTAGE_RESCUED) {
				wins++;
			}
		}
	}
	return wins;
}

/**
	Returns the team id of the opposing team.

	@param teamId the own team id
	@return team id of the opposing team if own team is valid, otherwise 0
*/
int Analyzer::getOppositeTeamId(int teamId) {
	if (teamId != TEAMID_COUNTERTERRORISTS && teamId != TEAMID_TERRORISTS)
		return 0;

	return (teamId == TEAMID_COUNTERTERRORISTS) ? TEAMID_TERRORISTS : TEAMID_COUNTERTERRORISTS;
}

/**
	Checks if the requirements are fulfilled.

	@return true if the requirements are fulfilled, otherwise false
*/
bool Analyzer::checkRequirements() {
	if (config->analyzer.played_time < MIN_PLAYTIME_FOR_ANALYZING)
		return false;

	if (config->analyzer.matchmakings_wins < MIN_WON_MATCHMAKINGS_FOR_ANALYZING)
		return false;

	if (config->analyzer.played_matchmakings < MIN_PLAYED_MATCHMAKINGS_FOR_ANALYZING)
		return false;

	return true;
}

/**
	Starts the improvement of the players profile.

	@return nothing
*/
void Analyzer::improvePlayerProfile() {
	//send stop message to console
	Console::getInstance().println("The adjustment of the values was started!");

	int		probabilityDistribution[3] = {60, 30, 10};
	int		nextAdjustmentAction = 0;
	float	multiplier = 0.0f;

	//feature 1
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.divergence_if_spotted * multiplier;
		Analyzer::setWeaponDivergenceIfSpotted(weaponId, newValue);
	}

	//feature 2
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.first_position_of_critical_hit * multiplier;
		Analyzer::setWeaponPositionOfFirstCriticalHit(weaponId, newValue);
	}

	//feature 3
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.hit_chance_while_moving * multiplier;
		Analyzer::setWeaponHitChanceWhileMoving(weaponId, newValue);
	}

	//feature 4
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.time_to_kill * multiplier;
		Analyzer::setWeaponKillTime(weaponId, static_cast<int>(newValue));
	}

	//feature 5
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.delay_to_stop_aiming * multiplier;
		Analyzer::setWeaponStopAimingDelay(weaponId, newValue);
	}

	//feature 6
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.chance_to_switch * multiplier;
		Analyzer::setWeaponSwitchChance(weaponId, newValue);
	}

	//feature 7
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.time_to_switch * multiplier;
		Analyzer::setWeaponTimeToSwitch(weaponId, static_cast<int>(newValue));
	}

	
#if 0
	//feature 8
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];

	}
#endif

	//feature 9
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.max_hit_accuracy * multiplier;
		Analyzer::setWeaponMaxHitAccuracy(weaponId, newValue);
	}

	//feature 10
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot * multiplier;
		Analyzer::setWeaponMaxHitAccuracyForFirstShot(weaponId, newValue);
	}

	//feature 11
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.aim_time_per_degree * multiplier;
		Analyzer::setWeaponAimTimePerDegree(weaponId, newValue);
	}

	//feature 12
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			float tmp_multiplier = (config->weapons[weaponId].aimbot.recoil_scale >= 2.0f) ? (1.0f + (multiplier * -1)) : (1.0f + (multiplier));
			float newValue = config->weapons[weaponId].aimbot.recoil_scale * tmp_multiplier;
			Analyzer::setWeaponRecoilControlValue(weaponId, newValue);
		}
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			float tmp_multiplier = (config->weapons[weaponId].aimbot.recoil_scale >= 2.0f) ? (1.0f + (multiplier)) : (1.0f + (multiplier * -1));
			float newValue = config->weapons[weaponId].aimbot.recoil_scale * tmp_multiplier;
			Analyzer::setWeaponRecoilControlValue(weaponId, newValue);
		}
	}
	
#if 0
	//feature 13
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier * -1);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
	}
#endif

#if 0
	//feature 14
	nextAdjustmentAction = Analyzer::randomInt(MIN_SELECTION_VALUE_FOR_ADJUSTMENT, MAX_SELECTION_VALUE_FOR_ADJUSTMENT);
	multiplier = Analyzer::randomFloat(MIN_PERCENTAGE_FOR_ADJUSTMENT, MAX_PERCENTAGE_FOR_ADJUSTMENT);

	if (nextAdjustmentAction < probabilityDistribution[0]) {
		//Improve
		multiplier = 1.0f + (multiplier * -1);
	} else if (nextAdjustmentAction >= probabilityDistribution[0] && nextAdjustmentAction <= probabilityDistribution[1]) {
		//Worsen
		multiplier = 1.0f + (multiplier);
	} else {
		multiplier = 1.0f;
	}

	for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
		int weaponId = analyzerWeapons[weaponIndex];
		float newValue = config->weapons[weaponId].aimbot.divergence_at_first_shot * multiplier;
		Analyzer::setWeaponDivergenceAtFirstShot(weaponId, newValue);
	}
#endif

	/*
	//simulate and log adjustment process
	Sleep(250);
	Analyzer::readFromConfigFile();
	tempConfig["esp"] = {};
	tempConfig["general"] = {};
	tempConfig["keys"] = {};
	tempConfig["test"] = {};
	adjustmentTest += {adjustmentTestCounter, tempConfig};
	adjustmentTestCounter++;
	std::ofstream o("C:\\tubs\\adjustment_test.json");
	o << std::setw(4) << adjustmentTest << std::endl;
	*/

	//send stop message to console
	Console::getInstance().println("The adjustment of the values has been completed!");
}

/**
	Starts the evaluation of the profiler's recorded values.

	@return nothing
*/
void Analyzer::analyze() {
	if (!Analyzer::checkRequirements()) {
		Console::getInstance().println("The analyzer can not be started because the requirements are not fulfilled.", red, black);
		Console::getInstance().println("(1) playtime:               " + std::to_string(config->analyzer.played_time) + "/" + std::to_string(MIN_PLAYTIME_FOR_ANALYZING) + " sec");
		Console::getInstance().println("(2) played matchmakings:    " + std::to_string(config->analyzer.played_matchmakings) + "/" + std::to_string(MIN_PLAYED_MATCHMAKINGS_FOR_ANALYZING));
		Console::getInstance().println("(3) won matchmakings:       " + std::to_string(config->analyzer.matchmakings_wins) + "/" + std::to_string(MIN_WON_MATCHMAKINGS_FOR_ANALYZING));
	} else if (config->analyzer.action_for_not_enough_values < ANALYZER_ACTION_ABORT || config->analyzer.action_for_not_enough_values > ANALYZER_ACTION_DEACTIVATE) { //ANALYZER_ACTION_IGNORE
		Console::getInstance().println("The analyzer can not be started because the selected mode 'action_for_not_enough_values' is invalid.", red, black);
	} else if (!isAnalyzing) {
		//set analyzer started
		isAnalyzing = true;

		//disable all hacks and the profiler, while analyzing
		config->general.profiler_enabled = false;
		config->general.aimbot_enabled = false;
		config->general.triggerbot_enabled = false;

		//send start message to console
		Console::getInstance().println("Analyzer process has started! Please wait...");

		Console::getInstance().println("Start reading profile.json file...");

		//read the current profile file
		Analyzer::readFromProfileFile();

		Console::getInstance().println("Start calculations...");

		for (int i = 0; i < (LAST_WEAPON_ID + 1); i++) {
			aimbotDeactivatedFlag[i] = false;
		}

		int result = ANALYZER_RETURN_OK,
			errors = 0;

		//calculate each feature
		result = Analyzer::calculateFeature1();
		Analyzer::sendCalculationResultMessage(result, 1, &errors);
		
		result = Analyzer::calculateFeature2();
		Analyzer::sendCalculationResultMessage(result, 2, &errors);

		result = Analyzer::calculateFeature3();
		Analyzer::sendCalculationResultMessage(result, 3, &errors);

		result = Analyzer::calculateFeature4();
		Analyzer::sendCalculationResultMessage(result, 4, &errors);

		result = Analyzer::calculateFeature5();
		Analyzer::sendCalculationResultMessage(result, 5, &errors);

		result = Analyzer::calculateFeature6();
		Analyzer::sendCalculationResultMessage(result, 6, &errors);

		result = Analyzer::calculateFeature7();
		Analyzer::sendCalculationResultMessage(result, 7, &errors);
		
		result = Analyzer::calculateFeature8();
		Analyzer::sendCalculationResultMessage(result, 8, &errors);

		result = Analyzer::calculateFeature9();
		Analyzer::sendCalculationResultMessage(result, 9, &errors);

		result = Analyzer::calculateFeature10();
		Analyzer::sendCalculationResultMessage(result, 10, &errors);

		result = Analyzer::calculateFeature11();
		Analyzer::sendCalculationResultMessage(result, 11, &errors);

		result = Analyzer::calculateFeature12();
		Analyzer::sendCalculationResultMessage(result, 12, &errors);

		result = Analyzer::calculateFeature13();
		Analyzer::sendCalculationResultMessage(result, 13, &errors);

		result = Analyzer::calculateFeature14();
		Analyzer::sendCalculationResultMessage(result, 14, &errors);

		//send stop message to console
		config->general.ready_for_use = (errors == 0) ? true : false;
		config->saveConfig();
		Console::getInstance().println("Analyzing process has been completed! errors: " + std::to_string(errors));

		Analyzer::setNextImprovementTime();

		//set analyzer stopped
		isAnalyzing = false;
	}
}

/**
	Calculates the results of Feature 1.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature1() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f1"];

		float avgDivergenceIfSpotted[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		float avgDivergenceIfSpotted_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();

			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					float div = static_cast<float>(values["div"]);
					if (div > 0.0) {
						avgDivergenceIfSpotted[weaponId] = avgDivergenceIfSpotted[weaponId] + div;
						counter[weaponId] = counter[weaponId] + 1;

						avgDivergenceIfSpotted_t[weaponType] = avgDivergenceIfSpotted[weaponId] + div;
						counter_t[weaponType] = counter[weaponId] + 1;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_1) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_1) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_1) {
						result = ANALYZER_RETURN_OK;
						float divergenceIfSpotted = static_cast<float>(avgDivergenceIfSpotted_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponDivergenceIfSpotted(weaponId, divergenceIfSpotted);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_1/* && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)*/) {
							result = ANALYZER_RETURN_OK;
							float divergenceIfSpotted = static_cast<float>(avgDivergenceIfSpotted[j] / counter[j]);
							Analyzer::setWeaponDivergenceIfSpotted(weaponId, divergenceIfSpotted);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float divergenceIfSpotted = static_cast<float>(avgDivergenceIfSpotted[weaponId] / counter[weaponId]);
				Analyzer::setWeaponDivergenceIfSpotted(weaponId, divergenceIfSpotted);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 2.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature2() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f2"];

		float avgPositionOfCriticalHit[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		float avgPositionOfCriticalHit_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					json hits = values["hits"];
					int numHitsToCritical = 0;
					for (auto& element : hits.items()) {
						auto hit = element.value();
						if (hit["critical"] == 1) {
							numHitsToCritical = stoi(element.key()) + 1;
							break;
						}
					}
					if (numHitsToCritical > 0) {
						avgPositionOfCriticalHit[weaponId] += static_cast<float>(1 / numHitsToCritical);
						avgPositionOfCriticalHit_t[weaponType] += static_cast<float>(1 / numHitsToCritical);
					}
					counter[weaponId]++;
					counter_t[weaponType]++;
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_2) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_2) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_2) {
						result = ANALYZER_RETURN_OK;
						float positionOfCriticalHit = static_cast<float>(avgPositionOfCriticalHit_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponPositionOfFirstCriticalHit(weaponId, positionOfCriticalHit);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_2 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float positionOfCriticalHit = static_cast<float>(avgPositionOfCriticalHit[j] / counter[j]);
							Analyzer::setWeaponPositionOfFirstCriticalHit(weaponId, positionOfCriticalHit);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float positionOfCriticalHit = static_cast<float>(avgPositionOfCriticalHit[weaponId] / counter[weaponId]);
				Analyzer::setWeaponPositionOfFirstCriticalHit(weaponId, positionOfCriticalHit);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 3.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature3() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f3"];

		float avgHitChance[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		float avgHitChance_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int hitsWhileMoving = static_cast<int>(values["hitsWhileMoving"]);
					int totalHits = static_cast<int>(values["totalHits"]);
					if (hitsWhileMoving >= 0 && totalHits > 0 && totalHits >= hitsWhileMoving) {
						avgHitChance[weaponId] += static_cast<float>(hitsWhileMoving / totalHits);
						counter[weaponId]++;

						avgHitChance_t[weaponType] += static_cast<float>(hitsWhileMoving / totalHits);
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_3) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_3) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_3) {
						result = ANALYZER_RETURN_OK;
						float hitChance = static_cast<float>(avgHitChance_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponHitChanceWhileMoving(weaponId, hitChance);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_3 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float hitChance = static_cast<float>(avgHitChance[j] / counter[j]);
							Analyzer::setWeaponHitChanceWhileMoving(weaponId, hitChance);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float hitChance = static_cast<float>(avgHitChance[weaponId] / counter[weaponId]);
				Analyzer::setWeaponHitChanceWhileMoving(weaponId, hitChance);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 4.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature4() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f4"];

		int avgKillTime[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		int avgKillTime_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int timeToKill = static_cast<int>(values["timeToKill"]);
					if (timeToKill > 0 && timeToKill < 20000) {
						avgKillTime[weaponId] += timeToKill;
						counter[weaponId]++;

						avgKillTime_t[weaponType] += timeToKill;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_4) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_4) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_4) {
						result = ANALYZER_RETURN_OK;
						int killTime = static_cast<int>(avgKillTime_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponKillTime(weaponId, killTime);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_4 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							int killTime = static_cast<int>(avgKillTime[j] / counter[j]);
							Analyzer::setWeaponKillTime(weaponId, killTime);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				int killTime = static_cast<int>(avgKillTime[weaponId] / counter[weaponId]);
				Analyzer::setWeaponKillTime(weaponId, killTime);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 5.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature5() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f5"];

		int avgAimTime[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		int avgAimTime_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int aimTime = static_cast<int>(values["aimTime"]);
					if (aimTime > 0 && aimTime < 15000) {
						avgAimTime[weaponId] += aimTime;
						counter[weaponId]++;

						avgAimTime_t[weaponType] += aimTime;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_5) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_5) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_5) {
						result = ANALYZER_RETURN_OK;
						float stopAimingDelay = static_cast<float>(avgAimTime_t[weaponType]) / static_cast<float>(counter_t[weaponType]);
						Analyzer::setWeaponStopAimingDelay(weaponId, stopAimingDelay);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_5/* && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)*/) {
							result = ANALYZER_RETURN_OK;
							float stopAimingDelay = static_cast<float>(avgAimTime[j]) / static_cast<float>(counter[j]);
							Analyzer::setWeaponStopAimingDelay(weaponId, stopAimingDelay);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float stopAimingDelay = static_cast<float>(avgAimTime[weaponId]) / static_cast<float>(counter[weaponId]);
				Analyzer::setWeaponStopAimingDelay(weaponId, stopAimingDelay);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 6.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature6() {
	try {
		json feature = profile["f6"];

		int numWeaponSwitched = 0;
		int numWeaponNotSwitched = 0;
		int counter = 0;

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int timeToSwitch = static_cast<int>(values["timeToSwitch"]);
					if (timeToSwitch < 10000) {
						if (timeToSwitch > 0) {
							numWeaponSwitched ++;
						} else {
							numWeaponNotSwitched ++;
						}
						counter ++;
					}
				}
			}
		}

		if (counter >= MIN_NUM_OF_VALUES_FOR_FEATURE_6) {
			for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
				int weaponId = analyzerWeapons[weaponIndex];
				float chanceToSwitch = static_cast<float>(numWeaponSwitched) / static_cast<float>(counter);
				Analyzer::setWeaponSwitchChance(weaponId, chanceToSwitch);
				Analyzer::activateAimbotWeapon(weaponId);
			}
			return ANALYZER_RETURN_OK;
		} else {
			for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
				int weaponId = analyzerWeapons[weaponIndex];
				Analyzer::setWeaponSwitchChance(weaponId, 0.0f);
				Analyzer::activateAimbotWeapon(weaponId);
			}
			return ANALYZER_RETURN_NOT_ENOUGH_VALUES;
		}
	} catch (...) {
		return ANALYZER_RETURN_ERROR;
	}
}
#if 0
int Analyzer::calculateFeature6() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f6"];

		int numWeaponSwitched[LAST_WEAPON_ID + 1] = { };
		int numWeaponNotSwitched[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		int numWeaponSwitched_t[LAST_WEAPON_TYPE + 1] = { };
		int numWeaponNotSwitched_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int timeToSwitch = static_cast<int>(values["timeToSwitch"]);
					if (timeToSwitch < 10000) {
						if (timeToSwitch > 0) {
							numWeaponSwitched[weaponId] ++;
							numWeaponSwitched_t[weaponType] ++;
						} else {
							numWeaponNotSwitched[weaponId] ++;
							numWeaponNotSwitched_t[weaponType] ++;
						}
						counter[weaponId]++;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_6) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_6) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_6) {
						result = ANALYZER_RETURN_OK;
						float chanceToSwitch = static_cast<float>(numWeaponSwitched_t[weaponType]) / static_cast<float>(counter_t[weaponType]);
						Analyzer::setWeaponSwitchChance(weaponId, chanceToSwitch);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type " + std::to_string(chanceToSwitch) + "=" + std::to_string(numWeaponSwitched_t[weaponType]) + "/" + std::to_string(counter_t[weaponType]));
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_6) { //&& Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)
							result = ANALYZER_RETURN_OK;
							float chanceToSwitch = static_cast<float>(numWeaponSwitched[j]) / static_cast<float>(counter[j]);
							Analyzer::setWeaponSwitchChance(weaponId, chanceToSwitch);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float chanceToSwitch = static_cast<float>(numWeaponSwitched[weaponId]) / static_cast<float>(counter[weaponId]);
				Analyzer::setWeaponSwitchChance(weaponId, chanceToSwitch);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}
#endif

/**
	Calculates the results of Feature 7.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature7() {
	try {
		json feature = profile["f6"]; //f7 does not exist!

		int avgTimeToSwitch = 0;
		int counter = 0;

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int timeToSwitch = static_cast<int>(values["timeToSwitch"]);
					if (timeToSwitch > 0 && timeToSwitch < 10000) {
						avgTimeToSwitch += timeToSwitch;
						counter ++;
					}
				}
			}
		}

		if (counter >= MIN_NUM_OF_VALUES_FOR_FEATURE_6) {
			for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
				int weaponId = analyzerWeapons[weaponIndex];
				int timeToSwitchWeapon = avgTimeToSwitch / counter;
				Analyzer::setWeaponTimeToSwitch(weaponId, timeToSwitchWeapon);
				Analyzer::activateAimbotWeapon(weaponId);
			}
			return ANALYZER_RETURN_OK;
		} else {
			for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
				int weaponId = analyzerWeapons[weaponIndex];
				Analyzer::setWeaponTimeToSwitch(weaponId, 0);
				Analyzer::activateAimbotWeapon(weaponId);
			}
			return ANALYZER_RETURN_NOT_ENOUGH_VALUES;
		}
	} catch (...) {
		return ANALYZER_RETURN_ERROR;
	}
}
/**
	Calculates the results of Feature 7.

	@return the status of whether a calculation is successful or not.
*/
#if 0
int Analyzer::calculateFeature7() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f6"]; //f7 does not exist!

		int avgTimeToSwitch[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		int avgTimeToSwitch_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int timeToSwitch = static_cast<int>(values["timeToSwitch"]);
					if (timeToSwitch > 0 && timeToSwitch < 10000) {
						avgTimeToSwitch[weaponId] += timeToSwitch;
						counter[weaponId]++;

						avgTimeToSwitch_t[weaponType] += timeToSwitch;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_7) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_7) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_7) {
						result = ANALYZER_RETURN_OK;
						int timeToSwitchWeapon = avgTimeToSwitch_t[weaponType] / counter_t[weaponType];
						Analyzer::setWeaponTimeToSwitch(weaponId, timeToSwitchWeapon);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_7) { //&& Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)
							result = ANALYZER_RETURN_OK;
							int timeToSwitchWeapon = avgTimeToSwitch[j] / counter[j];
							Analyzer::setWeaponTimeToSwitch(weaponId, timeToSwitchWeapon);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				int timeToSwitchWeapon = avgTimeToSwitch[weaponId] / counter[weaponId];
				Analyzer::setWeaponTimeToSwitch(weaponId, timeToSwitchWeapon);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}
#endif

/**
	Calculates the results of Feature 8.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature8() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f8"];

		const int numPrimaryBones = 3;
		const int numBones = 9;
		int hitOnBone[LAST_WEAPON_ID + 1][numBones] = { };
		int numOfHitsOnClosestBone[LAST_WEAPON_ID + 1] = { };
		int numOfHitsOnOtherBone[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		int hitOnBone_t[LAST_WEAPON_TYPE + 1][numBones] = { };
		int numOfHitsOnClosestBone_t[LAST_WEAPON_TYPE + 1] = { };
		int numOfHitsOnOtherBone_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int firstHitBoneId = static_cast<int>(values["firstHitBoneId"]);
					int closestBoneId = static_cast<int>(values["closestBoneId"]);
					if (firstHitBoneId >= 0 && firstHitBoneId < numBones && closestBoneId >= 0 && closestBoneId < numBones) {
						if (firstHitBoneId == closestBoneId) {
							numOfHitsOnClosestBone[weaponId] ++;
							numOfHitsOnClosestBone_t[weaponType] ++;
						} else {
							numOfHitsOnOtherBone[weaponId] ++;
							numOfHitsOnOtherBone_t[weaponType] ++;
						}
						hitOnBone[weaponId][firstHitBoneId] ++;
						counter[weaponId]++;

						hitOnBone_t[weaponType][firstHitBoneId] ++;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_8) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_8) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_8) {
						result = ANALYZER_RETURN_OK;
						float chanceForClosestBone = static_cast<float>(numOfHitsOnClosestBone_t[weaponType]) / static_cast<float>(counter_t[weaponType]);
						Analyzer::setWeaponChanceForClosestBone(weaponId, chanceForClosestBone);

						int primaryBones[numPrimaryBones];
						Analyzer::sortArrayAndGetIndexArray(hitOnBone_t[weaponType], numBones, primaryBones, numPrimaryBones);
						Analyzer::setWeaponPrimaryBones(weaponId, primaryBones, numPrimaryBones);

						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_8 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float chanceForClosestBone = static_cast<float>(numOfHitsOnClosestBone[j]) / static_cast<float>(counter[j]);
							Analyzer::setWeaponChanceForClosestBone(weaponId, chanceForClosestBone);

							int primaryBones[numPrimaryBones];
							Analyzer::sortArrayAndGetIndexArray(hitOnBone[j], numBones, primaryBones, numPrimaryBones);
							Analyzer::setWeaponPrimaryBones(weaponId, primaryBones, numPrimaryBones);

							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float chanceForClosestBone = static_cast<float>(numOfHitsOnClosestBone[weaponId]) / static_cast<float>(counter[weaponId]);
				Analyzer::setWeaponChanceForClosestBone(weaponId, chanceForClosestBone);

				int primaryBones[numPrimaryBones];
				Analyzer::sortArrayAndGetIndexArray(hitOnBone[weaponId], numBones, primaryBones, numPrimaryBones);
				Analyzer::setWeaponPrimaryBones(weaponId, primaryBones, numPrimaryBones);

				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 9.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature9() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f9"];

		float avgHitAccuracy[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };
		
		float avgHitAccuracy_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int hitsWhileAiming = static_cast<int>(values["hitsWhileAiming"]);
					int shotsWhileAiming = static_cast<int>(values["shotsWhileAiming"]);
					if (hitsWhileAiming >= 0 && shotsWhileAiming > 0 && shotsWhileAiming >= hitsWhileAiming) {
						avgHitAccuracy[weaponId] += static_cast<float>(hitsWhileAiming / shotsWhileAiming);
						counter[weaponId]++;

						avgHitAccuracy_t[weaponType] += static_cast<float>(hitsWhileAiming / shotsWhileAiming);
						counter_t[weaponType]++;
					}
				}
			}
		}
		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_9) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_9) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_9) {
						result = ANALYZER_RETURN_OK;
						float tmp = (avgHitAccuracy_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponMaxHitAccuracy(weaponId, tmp);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_9 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float tmp = (avgHitAccuracy[j] / counter[j]);
							Analyzer::setWeaponMaxHitAccuracy(weaponId, tmp);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float tmp = (avgHitAccuracy[weaponId] / counter[weaponId]);
				Analyzer::setWeaponMaxHitAccuracy(weaponId, tmp);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 10.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature10() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f10"];

		float avgHitAccuracy[LAST_WEAPON_ID + 1] = { };
		int counter[LAST_WEAPON_ID + 1] = { };

		float avgHitAccuracy_t[LAST_WEAPON_TYPE + 1] = { };
		int counter_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int shotsUntilHit = static_cast<int>(values["shotsUntilHit"]);
					if (shotsUntilHit > 0) {
						avgHitAccuracy[weaponId] += (1.0f / shotsUntilHit);
						counter[weaponId]++;

						avgHitAccuracy_t[weaponType] += (1.0f / shotsUntilHit);
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_10) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_10) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_10) {
						result = ANALYZER_RETURN_OK;
						float tmp = (avgHitAccuracy_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponMaxHitAccuracyForFirstShot(weaponId, tmp);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_10 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float tmp = (avgHitAccuracy[j] / counter[j]);
							Analyzer::setWeaponMaxHitAccuracyForFirstShot(weaponId, tmp);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float tmp = (avgHitAccuracy[weaponId] / counter[weaponId]);
				Analyzer::setWeaponMaxHitAccuracyForFirstShot(weaponId, tmp);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 11.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature11() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f11"];

		int counter[LAST_WEAPON_ID + 1] = { };
		int avgAimTime[LAST_WEAPON_ID + 1] = { };
		float avgAimTimePerDegree[LAST_WEAPON_ID + 1] = { };
		
		int counter_t[LAST_WEAPON_TYPE + 1] = { };
		int avgAimTime_t[LAST_WEAPON_TYPE + 1] = { };
		float avgAimTimePerDegree_t[LAST_WEAPON_TYPE + 1] = { };		

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					int aimTime = static_cast<int>(values["aimTime"]);
					float angle = static_cast<float>(values["angle"]);
					if (aimTime > 0 && aimTime < 20000 && angle > 0.0f && angle < 60.0f) {
						avgAimTime[weaponId] += aimTime;
						avgAimTimePerDegree[weaponId] += (static_cast<float>(aimTime) / angle);
						counter[weaponId]++;

						avgAimTime_t[weaponType] += aimTime;
						avgAimTimePerDegree_t[weaponType] += (static_cast<float>(aimTime) / angle);
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_11) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_11) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_11) {
						result = ANALYZER_RETURN_OK;
						int aimTime = (avgAimTime_t[weaponType] / counter_t[weaponType]);
						float aimTimePerDegree = (avgAimTimePerDegree_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponAimTimePerDegree(weaponId, aimTimePerDegree);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_11/* && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)*/) {
							result = ANALYZER_RETURN_OK;
							int aimTime = (avgAimTime[j] / counter[j]);
							float aimTimePerDegree = (avgAimTimePerDegree[j] / counter[j]);
							Analyzer::setWeaponAimTimePerDegree(weaponId, aimTimePerDegree);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				int aimTime = (avgAimTime[weaponId] / counter[weaponId]);
				float aimTimePerDegree = (avgAimTimePerDegree[weaponId] / counter[weaponId]);
				Analyzer::setWeaponAimTimePerDegree(weaponId, aimTimePerDegree);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 12.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature12() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f12"];

		int counter[LAST_WEAPON_ID + 1] = { };
		float avgRecoilControl[LAST_WEAPON_ID + 1] = { };

		int counter_t[LAST_WEAPON_TYPE + 1] = { };
		float avgRecoilControl_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					float avg = static_cast<float>(values["avg"]);
					if (avg > -5.0f && avg < 5.0f) {
						avgRecoilControl[weaponId] += avg;
						counter[weaponId]++;

						avgRecoilControl_t[weaponType] += avg;
						counter_t[weaponType]++;
					}
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_12) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_12) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_12) {
						result = ANALYZER_RETURN_OK;
						float recoilControl = (avgRecoilControl_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponRecoilControlValue(weaponId, recoilControl);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_12 && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)) {
							result = ANALYZER_RETURN_OK;
							float recoilControl = (avgRecoilControl[j] / counter[j]);
							Analyzer::setWeaponRecoilControlValue(weaponId, recoilControl);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			} 
			if (result == ANALYZER_RETURN_OK) {
				float recoilControl = (avgRecoilControl[weaponId] / counter[weaponId]);
				Analyzer::setWeaponRecoilControlValue(weaponId, recoilControl);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}

/**
	Calculates the results of Feature 13.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature13() {
	try {
		json feature = profile["f13"];
		int counter = 0,
			avgAboveCounter = 0,
			avgBelowCounter = 0;
		float avgOffsetX = 0.0f;

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/ && values["offsetX"] != NULL) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int aboveCounter = static_cast<int>(values["aboveCounter"]);
					int belowCounter = static_cast<int>(values["belowCounter"]);
					float offsetX = static_cast<float>(values["offsetX"]);
					if ((aboveCounter > 0 || belowCounter > 0) && offsetX > -2.0f && offsetX < 2.0f) {
						if (aboveCounter > belowCounter) {
							avgAboveCounter++;
						} else if (belowCounter > aboveCounter) {
							avgBelowCounter++;
						}
						avgOffsetX += fabs(offsetX);
						counter++;
					}
				}
			}
		}

		if (counter >= MIN_NUM_OF_VALUES_FOR_FEATURE_13) {
			avgOffsetX /= counter;

			float spiralAimX = 20.0f - (avgOffsetX * 47.5f); // 90.0f -> 47.5f
			float spiralAimY = (spiralAimX * 3.0f);

			if (spiralAimX >= LOWER_BOUND_FOR_FEATURE_13 && spiralAimX <= UPPER_BOUND_FOR_FEATURE_13) {
				config->aimbot.spiral_aim_x = spiralAimX;
				config->aimbot.spiral_aim_y = spiralAimY;
				config->aimbot.spiral_aim_enabled = true;
			} else {
				config->aimbot.spiral_aim_enabled = false;
				Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(spiralAimX) + " for feature 13 is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_13) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_13) + ").");
			}

			if (avgAboveCounter > 0 && avgBelowCounter > 0) {
				float spiralAimPosRelative = static_cast<float>(avgAboveCounter / avgBelowCounter);
				config->aimbot.spiral_aim_relative_position = spiralAimPosRelative;
			} else {
				config->aimbot.spiral_aim_enabled = false;
			}

			config->saveConfig();

			return ANALYZER_RETURN_OK;
		} else {
			return ANALYZER_RETURN_NOT_ENOUGH_VALUES;
		}
	} catch (...) {
		return ANALYZER_RETURN_ERROR;
	}
}

/**
	Calculates the results of Feature 14.

	@return the status of whether a calculation is successful or not.
*/
int Analyzer::calculateFeature14() {
	int result = ANALYZER_RETURN_OK;
	try {
		json feature = profile["f14"];

		int counter[LAST_WEAPON_ID + 1] = { };
		float avgDiv[LAST_WEAPON_ID + 1] = { };

		int counter_t[LAST_WEAPON_TYPE + 1] = { };
		float avgDiv_t[LAST_WEAPON_TYPE + 1] = { };

		for (auto& element : feature.items()) {
			int index = stoi(element.key());
			auto values = element.value();
			if (values["weaponId"] != NULL/* && values["aimbotEnabled"] != true*/) {
				int weaponId = static_cast<int>(values["weaponId"]);
				if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					avgDiv[weaponId] += static_cast<float>(values["div"]);
					counter[weaponId]++;

					avgDiv_t[weaponType] += static_cast<float>(values["div"]);
					counter_t[weaponType]++;
				}
			}
		}

		for (int weaponIndex = FIRST_WEAPON_INDEX; weaponIndex < LAST_WEAPON_INDEX; weaponIndex++) {
			int weaponId = analyzerWeapons[weaponIndex];
			if (counter[weaponId] < MIN_NUM_OF_VALUES_FOR_FEATURE_14) {
				Console::getInstance().logln(LOG_INFO, "not enough values for weaponId " + std::to_string(weaponId) + " (" + std::to_string(counter[weaponId]) + " < " + std::to_string(MIN_NUM_OF_VALUES_FOR_FEATURE_14) + ")");
				result = ANALYZER_RETURN_NOT_ENOUGH_VALUES;
				if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ABORT) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					break;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT2) {
					int weaponType = Analyzer::getWeaponType(weaponId);
					if (counter_t[weaponType] >= MIN_NUM_OF_VALUES_FOR_FEATURE_14) {
						result = ANALYZER_RETURN_OK;
						float div = (avgDiv_t[weaponType] / counter_t[weaponType]);
						Analyzer::setWeaponDivergenceAtFirstShot(weaponId, div);
						Analyzer::activateAimbotWeapon(weaponId);
						Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from all weapons of this type");
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_ADAPT) {
					for (int i = FIRST_WEAPON_INDEX; i < LAST_WEAPON_INDEX; i++) {
						int j = analyzerWeapons[i];
						if (j != weaponId && counter[j] >= MIN_NUM_OF_VALUES_FOR_FEATURE_14/* && Analyzer::getWeaponType(j) == Analyzer::getWeaponType(weaponId)*/) {
							result = ANALYZER_RETURN_OK;
							float div = (avgDiv[j] / counter[j]);
							Analyzer::setWeaponDivergenceAtFirstShot(weaponId, div);
							Analyzer::activateAimbotWeapon(weaponId);
							Console::getInstance().logln(LOG_INFO, "adapt values for weaponId " + std::to_string(weaponId) + " from weaponId " + std::to_string(j));
							break;
						}
					}
					if (result == ANALYZER_RETURN_OK) {
						continue;
					} else {
						result = ANALYZER_RETURN_OK;
						Console::getInstance().logln(LOG_INFO, "deactivate weapon -> not enough values to adapt to the weaponId " + std::to_string(weaponId));
						Analyzer::deactivateAimbotWeapon(weaponId);
						continue;
					}
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_DEACTIVATE) {
					Analyzer::deactivateAimbotWeapon(weaponId);
					result = ANALYZER_RETURN_OK;
					continue;
				} else if (config->analyzer.action_for_not_enough_values == ANALYZER_ACTION_IGNORE) {
					result = ANALYZER_RETURN_OK;
				}
			}
			if (result == ANALYZER_RETURN_OK) {
				float div = (avgDiv[weaponId] / counter[weaponId]);
				Analyzer::setWeaponDivergenceAtFirstShot(weaponId, div);
				Analyzer::activateAimbotWeapon(weaponId);
			}
		}
	} catch (...) {
		result = ANALYZER_RETURN_ERROR;
	}
	return result;
}
/**
	Sets the aim time per degree for a particular weapon.

	@param weaponId the id of the weapon
	@param aimTimePerDegree the aim time per degree
	@return nothing
*/
void Analyzer::setWeaponDivergenceAtFirstShot(int weaponId, float div) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (div >= LOWER_BOUND_FOR_FEATURE_14 && div <= UPPER_BOUND_FOR_FEATURE_14) {
			config->weapons[weaponId].aimbot.divergence_at_first_shot = div;
		} else {
			config->weapons[weaponId].aimbot.divergence_at_first_shot = (div > UPPER_BOUND_FOR_FEATURE_14) ? UPPER_BOUND_FOR_FEATURE_14 : LOWER_BOUND_FOR_FEATURE_14;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(div) + " for feature 14 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_14) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_14) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the aim time per degree for a particular weapon.

	@param weaponId the id of the weapon
	@param aimTimePerDegree the aim time per degree
	@return nothing
*/
void Analyzer::setWeaponAimTimePerDegree(int weaponId, float aimTimePerDegree) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (aimTimePerDegree >= LOWER_BOUND_FOR_FEATURE_11 && aimTimePerDegree <= UPPER_BOUND_FOR_FEATURE_11) {
			config->weapons[weaponId].aimbot.aim_time_per_degree = aimTimePerDegree;
		} else {
			config->weapons[weaponId].aimbot.aim_time_per_degree = (aimTimePerDegree > UPPER_BOUND_FOR_FEATURE_11) ? UPPER_BOUND_FOR_FEATURE_11 : LOWER_BOUND_FOR_FEATURE_11;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(aimTimePerDegree) + " for feature 11 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_11) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_11) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the position of the first critical hit for a particular weapon.

	@param weaponId the id of the weapon
	@param divergenceIfSpotted the position of the first critical hit
	@return nothing
*/
void Analyzer::setWeaponDivergenceIfSpotted(int weaponId, float divergenceIfSpotted) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (divergenceIfSpotted >= LOWER_BOUND_FOR_FEATURE_1 && divergenceIfSpotted <= UPPER_BOUND_FOR_FEATURE_1) {
			config->weapons[weaponId].aimbot.divergence_if_spotted = divergenceIfSpotted;
		} else {
			config->weapons[weaponId].aimbot.divergence_if_spotted = (divergenceIfSpotted > UPPER_BOUND_FOR_FEATURE_1) ? UPPER_BOUND_FOR_FEATURE_1 : LOWER_BOUND_FOR_FEATURE_1;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(divergenceIfSpotted) + " for feature 1 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_1) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_1) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the position of the first critical hit for a particular weapon.

	@param weaponId the id of the weapon
	@param firstPositionOfCriticalHit the position of the first critical hit
	@return nothing
*/
void Analyzer::setWeaponPositionOfFirstCriticalHit(int weaponId, float firstPositionOfCriticalHit) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (firstPositionOfCriticalHit >= LOWER_BOUND_FOR_FEATURE_2 && firstPositionOfCriticalHit <= UPPER_BOUND_FOR_FEATURE_2) {
			config->weapons[weaponId].aimbot.first_position_of_critical_hit = firstPositionOfCriticalHit;
		} else {
			config->weapons[weaponId].aimbot.first_position_of_critical_hit = (firstPositionOfCriticalHit > UPPER_BOUND_FOR_FEATURE_2) ? UPPER_BOUND_FOR_FEATURE_2 : LOWER_BOUND_FOR_FEATURE_2;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(firstPositionOfCriticalHit) + " for feature 2 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_2) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_2) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the chance to hit while moving for a particular weapon.

	@param weaponId the id of the weapon
	@param hitChance the chance to hit while moving
	@return nothing
*/
void Analyzer::setWeaponHitChanceWhileMoving(int weaponId, float hitChance) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (hitChance >= LOWER_BOUND_FOR_FEATURE_3 && hitChance <= UPPER_BOUND_FOR_FEATURE_3) {
			config->weapons[weaponId].aimbot.hit_chance_while_moving = hitChance;
		} else {
			config->weapons[weaponId].aimbot.hit_chance_while_moving = (hitChance > UPPER_BOUND_FOR_FEATURE_3) ? UPPER_BOUND_FOR_FEATURE_3 : LOWER_BOUND_FOR_FEATURE_3;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(hitChance) + " for feature 3 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_3) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_3) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the time to kill for a particular weapon.

	@param weaponId the id of the weapon
	@param killTime the time to kill
	@return nothing
*/
void Analyzer::setWeaponKillTime(int weaponId, int killTime) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (killTime >= LOWER_BOUND_FOR_FEATURE_4 && killTime <= UPPER_BOUND_FOR_FEATURE_4) {
			config->weapons[weaponId].aimbot.time_to_kill = killTime;
		} else {
			config->weapons[weaponId].aimbot.time_to_kill = (killTime > UPPER_BOUND_FOR_FEATURE_4) ? UPPER_BOUND_FOR_FEATURE_4 : LOWER_BOUND_FOR_FEATURE_4;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(killTime) + " for feature 4 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_4) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_4) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the delay to stop aiming for a particular weapon.

	@param weaponId the id of the weapon
	@param stopAimingDelay the delay to stop aiming
	@return nothing
*/
void Analyzer::setWeaponStopAimingDelay(int weaponId, float stopAimingDelay) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (stopAimingDelay >= LOWER_BOUND_FOR_FEATURE_5 && stopAimingDelay <= UPPER_BOUND_FOR_FEATURE_5) {
			config->weapons[weaponId].aimbot.delay_to_stop_aiming = stopAimingDelay;
		} else {
			config->weapons[weaponId].aimbot.delay_to_stop_aiming = (stopAimingDelay > UPPER_BOUND_FOR_FEATURE_5) ? UPPER_BOUND_FOR_FEATURE_5 : LOWER_BOUND_FOR_FEATURE_5;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(stopAimingDelay) + " for feature 5 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_5) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_5) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the chance to switch the weapon for a particular weapon.

	@param weaponId the id of the weapon
	@param chanceToSwitch the chance to switch the weapon
	@return nothing
*/
void Analyzer::setWeaponSwitchChance(int weaponId, float chanceToSwitch) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (chanceToSwitch >= LOWER_BOUND_FOR_FEATURE_6 && chanceToSwitch <= UPPER_BOUND_FOR_FEATURE_6) {
			config->weapons[weaponId].aimbot.chance_to_switch = chanceToSwitch;
		} else {
			config->weapons[weaponId].aimbot.chance_to_switch = (chanceToSwitch > UPPER_BOUND_FOR_FEATURE_6) ? UPPER_BOUND_FOR_FEATURE_6 : LOWER_BOUND_FOR_FEATURE_6;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(chanceToSwitch) + " for feature 6 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_6) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_6) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the the time to switch the weapon for a particular weapon.

	@param weaponId the id of the weapon
	@param timeToSwitchWeapon the time to switch the weapon
	@return nothing
*/
void Analyzer::setWeaponTimeToSwitch(int weaponId, int timeToSwitchWeapon) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (timeToSwitchWeapon >= LOWER_BOUND_FOR_FEATURE_7 && timeToSwitchWeapon <= UPPER_BOUND_FOR_FEATURE_7) {
			config->weapons[weaponId].aimbot.time_to_switch = timeToSwitchWeapon;
		} else {
			config->weapons[weaponId].aimbot.time_to_switch = (timeToSwitchWeapon > UPPER_BOUND_FOR_FEATURE_7) ? UPPER_BOUND_FOR_FEATURE_7 : LOWER_BOUND_FOR_FEATURE_7;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(timeToSwitchWeapon) + " for feature 7 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_7) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_7) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the primary bones for a particular weapon.

	@param weaponId the id of the weapon
	@param primaryBones the ids of the primary bones
	@param size the size of the primaryBones array
	@return nothing
*/
void Analyzer::setWeaponPrimaryBones(int weaponId, int primaryBones[], int size) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		for (int i = 0; i < size; i++) {
			if (primaryBones[i] >= 0 && primaryBones[i] <= 8) {
				config->weapons[weaponId].aimbot.primary_bones[i] = primaryBones[i];
			} else {
				config->weapons[weaponId].aimbot.primary_bones[i] = 0;
				Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(primaryBones[i]) + " for feature 8.2 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(0) + " - " + std::to_string(8) + ").");
			}
		}
		config->saveConfig();
	}
}

/**
	Sets the chance to hit first the closest bone for a particular weapon.

	@param weaponId the id of the weapon
	@param value the chance to hit first the closest bone
	@return nothing
*/
void Analyzer::setWeaponChanceForClosestBone(int weaponId, float value) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (value >= LOWER_BOUND_FOR_FEATURE_8 && value <= UPPER_BOUND_FOR_FEATURE_8) {
			config->weapons[weaponId].aimbot.chance_hit_closest_bone_first = value;
		} else {
			config->weapons[weaponId].aimbot.chance_hit_closest_bone_first = (value > UPPER_BOUND_FOR_FEATURE_8) ? UPPER_BOUND_FOR_FEATURE_8 : LOWER_BOUND_FOR_FEATURE_8;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(value) + " for feature 8.1 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_8) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_8) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the max hit accuracy value for the first shot for a particular weapon.

	@param weaponId the id of the weapon
	@param value the max hit accuracy value for the first shot
	@return nothing
*/
void Analyzer::setWeaponMaxHitAccuracyForFirstShot(int weaponId, float value) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (value >= LOWER_BOUND_FOR_FEATURE_10 && value <= UPPER_BOUND_FOR_FEATURE_10) {
			config->weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot = value;
		} else {
			config->weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot = (value > UPPER_BOUND_FOR_FEATURE_10) ? UPPER_BOUND_FOR_FEATURE_10 : LOWER_BOUND_FOR_FEATURE_10;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(value) + " for feature 10 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_10) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_10) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the max hit accuracy value for a particular weapon.

	@param weaponId the id of the weapon
	@param value the max hit accuracy value
	@return nothing
*/
void Analyzer::setWeaponMaxHitAccuracy(int weaponId, float value) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (value >= LOWER_BOUND_FOR_FEATURE_9 && value <= UPPER_BOUND_FOR_FEATURE_9) {
			config->weapons[weaponId].aimbot.max_hit_accuracy = value;
		} else {
			config->weapons[weaponId].aimbot.max_hit_accuracy = (value > UPPER_BOUND_FOR_FEATURE_9) ? UPPER_BOUND_FOR_FEATURE_9 : LOWER_BOUND_FOR_FEATURE_9;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(value) + " for feature 9 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_9) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_9) + ").");
		}
		config->saveConfig();
	}
}

/**
	Sets the recoil value for a particular weapon.

	@param weaponId the id of the weapon
	@param value the recoil value
	@return nothing
*/
void Analyzer::setWeaponRecoilControlValue(int weaponId, float value) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		if (value >= LOWER_BOUND_FOR_FEATURE_12 && value <= UPPER_BOUND_FOR_FEATURE_12) {
			config->weapons[weaponId].aimbot.recoil_scale = value;
		} else {
			config->weapons[weaponId].aimbot.recoil_scale = (value > UPPER_BOUND_FOR_FEATURE_12) ? UPPER_BOUND_FOR_FEATURE_12 : LOWER_BOUND_FOR_FEATURE_12;
			Console::getInstance().logln(LOG_WARN, "calculated value " + std::to_string(value) + " for feature 12 and weaponId " + std::to_string(weaponId) + " is out of bounds (" + std::to_string(LOWER_BOUND_FOR_FEATURE_12) + " - " + std::to_string(UPPER_BOUND_FOR_FEATURE_12) + ").");
		}
		config->saveConfig();
	}
}

/**
	Activates the aimbot for a specific weapon.

	@param weaponId the id of the weapon
	@return nothing
*/
void Analyzer::activateAimbotWeapon(int weaponId) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID && !aimbotDeactivatedFlag[weaponId]) {
		config->weapons[weaponId].aimbot.enabled = true;
		config->saveConfig();
	}
}

/**
	Deactivates the aimbot for a specific weapon.

	@param weaponId the id of the weapon
	@return nothing
*/
void Analyzer::deactivateAimbotWeapon(int weaponId) {
	if (weaponId >= FIRST_WEAPON_ID && weaponId <= LAST_WEAPON_ID) {
		aimbotDeactivatedFlag[weaponId] = true;
		config->weapons[weaponId].aimbot.enabled = false;
		config->saveConfig();
	}
}

/**
	Reads the profile data from the profile.json file.

	@return nothing
*/
void Analyzer::readFromProfileFile() {
	try {
		std::ifstream i("C:\\tubs\\profile.json");
		i >> profile;
	} catch (...) {
		//memory->debuglog("Analyzer -> readFromProfileFile");
	}
}

/**
	Reads the adjustment test data from the adjustment_test.json file.

	@return nothing
*/
void Analyzer::readFromAdjustmentTestFile() {
	try {
		std::ifstream i("C:\\tubs\\adjustment_test.json");
		i >> adjustmentTest;
	} catch (...) {
		//memory->debuglog("Analyzer -> readFromAdjustmentTestFile");
	}
}

/**
	Reads the config data from the config.json file.

	@return nothing
*/
void Analyzer::readFromConfigFile() {
	try {
		std::ifstream i("C:\\tubs\\config.json");
		i >> tempConfig;
	} catch (...) {
		//memory->debuglog("Analyzer -> readFromConfigFile");
	}
}

/**
	Returns the type of a weapon based on its id.

	@param weaponId the id of the weapon
	@return the type of weapon
*/
EWeaponType Analyzer::getWeaponType(int weaponId) {
	switch (weaponId) {
		case WID_C4:
			return EWeaponType::WeaponType_C4Explosive;

		case WID_Zeus:
			return EWeaponType::WeaponType_ZeusGun;

		case WID_Negev:
		case WID_M249:
			return EWeaponType::WeaponType_LMG;

		case WID_AWP:
		case WID_G3SG1_Auto:
		case WID_SCAR_Auto:
		case WID_Scout:
			return EWeaponType::WeaponType_Sniper;

		case WID_XM1014:
		case WID_MAG7:
		case WID_Nova:
		case WID_SawedOff:
			return EWeaponType::WeaponType_Shotgun;

		case WID_Flashbang:
		case WID_Smoke:
		case WID_Firebomb:
		case WID_HEFrag:
		case WID_Molly:
			return EWeaponType::WeaponType_Grenade;

		case WID_MAC10:
		case WID_P90:
		case WID_UMP45:
		case WID_PPBizon:
		case WID_MP7:
		case WID_MP9:
			return EWeaponType::WeaponType_SMG;

		case WID_Deagle:
		case WID_Dual_Berettas:
		case WID_Five_Seven:
		case WID_USP:
		case WID_Glock:
		case WID_Tec9:
		case WID_P2000:
		case WID_P250:
		case WID_CZ75:
		case WID_Revolver:
			return EWeaponType::WeaponType_Pistol;

		case WID_AK47:
		case WID_AUG:
		case WID_FAMAS:
		case WID_M4A4:
		case WID_M4A1S:
		case WID_Galil:
		case WID_SG553:
			return EWeaponType::WeaponType_Rifle;

		case WID_Default_Knife:
		case WID_ButterflyKnife:
		case WID_FlipKnife:
		case WID_HuntsmanKnife:
		case WID_M9BayonetKnife:
		case WID_KarambitKnife:
		case WID_FalchionKnife:
		case WID_ShadowDaggerKnife:
		case WID_BayonetKnife:
		case WID_GutKnife:
		case WID_BowieKnife:
			return EWeaponType::WeaponType_KnifeType;

		default:
			return (EWeaponType)-1;
	}
}

/**
	Sorts a given array. From big values to small values. Then return an array of indexes.

	@param inputArray the input array
	@param inputSize the size of the input array
	@param outputArray the output array
	@param outputSize the size of the output array
	@return nothing
*/
void Analyzer::sortArrayAndGetIndexArray(int inputArray[], const int inputSize, int outputArray[], const int outputSize) {
	int tmp[10] = { };
	for (int i = 0; i < inputSize; i++) {
		tmp[i] = inputArray[i];
	}
	std::sort(inputArray, inputArray + inputSize, std::greater<int>());
	for (int i = 0; i < outputSize; i++) {
		outputArray[i] = -1;
		for (int j = 0; j < inputSize; j++) {
			if (tmp[j] != NULL && inputArray[i] == tmp[j]) {
				tmp[j] = NULL;
				outputArray[i] = j;
				break;
			}

		}
	}
}

/**
	Sends a message about the result of the calculation to the console.

	@param result the id of the result
	@param featureId the id of the feature
	@param errorCounter a pointer to the error counter
	@return nothing
*/
void Analyzer::sendCalculationResultMessage(int result, int featureId, int *errorCounter) {
	if (result == ANALYZER_RETURN_ERROR) {
		Console::getInstance().println("ERROR: Calculation of feature " + std::to_string(featureId) + " failed.", red, black);
		*errorCounter = *errorCounter + 1;
	} else if (result == ANALYZER_RETURN_NOT_ENOUGH_VALUES) {
		Console::getInstance().println("ERROR: Not enough values to calculate feature " + std::to_string(featureId) + ".", red, black);
		*errorCounter = *errorCounter + 1;
	} else if (ANALYZER_RETURN_OK) {
		Console::getInstance().println("SUCCESS: Calculation of feature " + std::to_string(featureId) + " successfully completed!", green, black);
	}
}

/**
	Calculates a random int between two int.

	@param a min int
	@param b max int
	@return the random int
*/
int Analyzer::randomInt(int a, int b) {
	std::random_device random;
	std::mt19937 randomGenerator(random());
	std::uniform_int_distribution<int> randomDistribute(a, b);
	return randomDistribute(randomGenerator);
}

/**
	Calculates a random float between two floats.

	@param a min float
	@param b max float
	@return the random float
*/
float Analyzer::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}