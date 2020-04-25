#include "stdafx.h"
#include "dllmain.h"

Config* config = new Config();

/**
	Reads the settings from the config.json file.

	@return nothing
*/
void Config::loadConfig() {
	try {
		std::ifstream i("C:\\tubs\\config.json");
		i >> config;

		/* WICHTIG:
		 * Datentyp JSON wurde im kompletten Projekt benutzt, allerdings gab es starke Performance-Probleme.
		 * Mit JSON liegt die CPU Auslastung bei ca 10-15% und ohne JSON bei ca 3-5%.
		 * Aus diesem Grund verwende ich JSON nur beim speichern und laden der Konfiguration.
		 */

		//general
		general.debug_mode = config["general"]["debug_mode"];
		general.panic_mode = config["general"]["panic_mode"];
		general.aimbot_enabled = config["general"]["aimbot_enabled"];
		general.triggerbot_enabled = config["general"]["triggerbot_enabled"];
		general.bspparsing_enabled = config["general"]["bspparsing_enabled"];
		general.team_damage = config["general"]["team_damage"];
		general.game_windowtitle = config["general"]["game_windowtitle"];
		general.profiler_enabled = config["general"]["profiler_enabled"];
		general.ready_for_use = config["general"]["ready_for_use"];

		//keys
		keys.ingame_fire = config["keys"]["ingame_fire"];
		keys.aimbot_toggle = config["keys"]["aimbot_toggle"];
		keys.aimbot_hold = config["keys"]["aimbot_hold"];
		keys.triggerbot_hold = config["keys"]["triggerbot_hold"];
		keys.triggerbot_toggle = config["keys"]["triggerbot_toggle"];
		keys.panic_mode = config["keys"]["panic_mode"];
		keys.reload_config = config["keys"]["reload_config"];
		keys.profiler_toggle = config["keys"]["profiler_toggle"];
		keys.start_analyzer = config["keys"]["start_analyzer"];
		keys.start_adjustment = config["keys"]["start_adjustment"];
		keys.ingame_switch_weapon = config["keys"]["ingame_switch_weapon"];

		//aimbot
		aimbot.adaptive_smooth_multiplier = config["aimbot"]["adaptive_smooth_multiplier"];
		aimbot.adaptive_smooth_rifles = config["aimbot"]["adaptive_smooth_rifles"];
		aimbot.adaptive_smooth_pistols = config["aimbot"]["adaptive_smooth_pistols"];
		aimbot.adaptive_smooth_snipers = config["aimbot"]["adaptive_smooth_snipers"];
		aimbot.adaptive_smooth_smgs = config["aimbot"]["adaptive_smooth_smgs"];
		aimbot.adaptive_smooth_shotguns = config["aimbot"]["adaptive_smooth_shotguns"];
		aimbot.adaptive_smooth_lmgs = config["aimbot"]["adaptive_smooth_lmgs"];
		aimbot.spiral_aim_enabled = config["aimbot"]["spiral_aim_enabled"];
		aimbot.spiral_aim_x = config["aimbot"]["spiral_aim_x"];
		aimbot.spiral_aim_y = config["aimbot"]["spiral_aim_y"];
		aimbot.spiral_aim_relative_position = config["aimbot"]["spiral_aim_relative_position"];
		aimbot.spiral_aim_noise = config["aimbot"]["spiral_aim_noise"];
		aimbot.faceit_aim = config["aimbot"]["faceit_aim"];
		aimbot.save_target_aim = config["aimbot"]["save_target_aim"];
		aimbot.chance_for_pre_shot = config["aimbot"]["chance_for_pre_shot"];
		aimbot.fov_noise = config["aimbot"]["fov_noise"];
		aimbot.recoil_scale_noise = config["aimbot"]["recoil_scale_noise"];
		aimbot.smooth_noise = config["aimbot"]["smooth_noise"];
		aimbot.chance_to_switch_noise = config["aimbot"]["chance_to_switch_noise"];
		aimbot.time_to_switch_noise = config["aimbot"]["time_to_switch_noise"];
		aimbot.kill_delay_noise = config["aimbot"]["kill_delay_noise"];
		aimbot.time_to_kill_noise = config["aimbot"]["time_to_kill_noise"];
		aimbot.delay_to_start_aiming_noise = config["aimbot"]["delay_to_start_aiming_noise"];
		aimbot.delay_to_stop_aiming_noise = config["aimbot"]["delay_to_stop_aiming_noise"];
		aimbot.chance_hit_closest_bone_first_noise = config["aimbot"]["chance_hit_closest_bone_first_noise"];
		aimbot.first_position_of_critical_hit_noise = config["aimbot"]["first_position_of_critical_hit_noise"];
		aimbot.hit_chance_while_moving_noise = config["aimbot"]["hit_chance_while_moving_noise"];
		aimbot.max_hit_accuracy_noise = config["aimbot"]["max_hit_accuracy_noise"];
		aimbot.max_hit_accuracy_for_first_shot_noise = config["aimbot"]["max_hit_accuracy_for_first_shot_noise"];
		aimbot.num_primary_bones_choice = config["aimbot"]["num_primary_bones_choice"];
		aimbot.compensate_velocity = config["aimbot"]["compensate_velocity"];
		aimbot.max_inaccuracy_offset = config["aimbot"]["max_inaccuracy_offset"];
		aimbot.divergence_at_first_shot_noise = config["aimbot"]["divergence_at_first_shot_noise"];
		
		//triggerbot
		triggerbot.fov_based = config["triggerbot"]["fov_based"];
		triggerbot.delay = config["triggerbot"]["delay"];

		//analyzer
		analyzer.played_matchmakings = config["analyzer"]["played_matchmakings"];
		analyzer.matchmakings_wins = config["analyzer"]["matchmakings_wins"];
		analyzer.matchmakings_loses = config["analyzer"]["matchmakings_loses"];
		analyzer.matchmakings_draws = config["analyzer"]["matchmakings_draws"];
		analyzer.played_time = config["analyzer"]["played_time"];
		analyzer.action_for_not_enough_values = config["analyzer"]["action_for_not_enough_values"];
		analyzer.next_adjustment = config["analyzer"]["next_adjustment"];

		//esp
		esp.enabled = config["esp"]["Enabled"];
		esp.DrawEnemyBombCarrierText = config["esp"]["DrawEnemyBombCarrierText"];
		esp.DrawEnemyBox = config["esp"]["DrawEnemyBox"];
		esp.DrawEnemyDeathsText = config["esp"]["DrawEnemyDeathsText"];
		esp.DrawEnemyDefuseKitCarrierText = config["esp"]["DrawEnemyDefuseKitCarrierText"];
		esp.DrawEnemyHealthBar = config["esp"]["DrawEnemyHealthBar"];
		esp.DrawEnemyHealthText = config["esp"]["DrawEnemyHealthText"];
		esp.DrawEnemyKDRatioText = config["esp"]["DrawEnemyKDRatioText"];
		esp.DrawEnemyKillsText = config["esp"]["DrawEnemyKillsText"];
		esp.DrawEnemyNameText = config["esp"]["DrawEnemyNameText"];
		esp.DrawEnemySnapLines = config["esp"]["DrawEnemySnapLines"];
		esp.DrawEnemyTargetBoneMarker = config["esp"]["DrawEnemyTargetBoneMarker"];
		esp.DrawFriendlyBombCarrierText = config["esp"]["DrawFriendlyBombCarrierText"];
		esp.DrawFriendlyBox = config["esp"]["DrawFriendlyBox"];
		esp.DrawFriendlyDeathsText = config["esp"]["DrawFriendlyDeathsText"];
		esp.DrawFriendlyDefuseKitCarrierText = config["esp"]["DrawFriendlyDefuseKitCarrierText"];
		esp.DrawFriendlyHealthBar = config["esp"]["DrawFriendlyHealthBar"];
		esp.DrawFriendlyHealthText = config["esp"]["DrawFriendlyHealthText"];
		esp.DrawFriendlyKDRatioText = config["esp"]["DrawFriendlyKDRatioText"];
		esp.DrawFriendlyKillsText = config["esp"]["DrawFriendlyKillsText"];
		esp.DrawFriendlyNameText = config["esp"]["DrawFriendlyNameText"];
		esp.DrawFriendlyRankText = config["esp"]["DrawFriendlyRankText"];
		esp.DrawFriendlySnapLines = config["esp"]["DrawFriendlySnapLines"];
		esp.DrawFriendlyTargetBoneMarker = config["esp"]["DrawFriendlyTargetBoneMarker"];
		esp.DrawRecoilMarker = config["esp"]["DrawRecoilMarker"];
		esp.DrawTargetBoxHighlight = config["esp"]["DrawTargetBoxHighlight"];
		esp.DrawTargetSnapLineHighlight = config["esp"]["DrawTargetSnapLineHighlight"];
		esp.EnemyBoxColor = jsonToColor(config["esp"]["EnemyBoxColor"]);
		esp.EnemyBoxLineWidth = config["esp"]["EnemyBoxLineWidth"];
		esp.EnemyBoxOutlineColor = jsonToColor(config["esp"]["EnemyBoxOutlineColor"]);
		esp.EnemyBoxOutlineWidth = config["esp"]["EnemyBoxOutlineWidth"];
		esp.EnemyHealthBarBackColor = jsonToColor(config["esp"]["EnemyHealthBarBackColor"]);
		esp.EnemyHealthBarFrontColor = jsonToColor(config["esp"]["EnemyHealthBarFrontColor"]);
		esp.EnemyHealthBarOutlineColor = jsonToColor(config["esp"]["EnemyHealthBarOutlineColor"]);
		esp.EnemyHealthBarOutlineWidth = config["esp"]["EnemyHealthBarOutlineWidth"];
		esp.EnemyHealthBarWidth = config["esp"]["EnemyHealthBarWidth"];
		esp.EnemyInfoTextColor = jsonToColor(config["esp"]["EnemyInfoTextColor"]);
		esp.EnemyInfoTextOutlineColor = jsonToColor(config["esp"]["EnemyInfoTextOutlineColor"]);
		esp.EnemySnapLineColor = jsonToColor(config["esp"]["EnemySnapLineColor"]);
		esp.EnemySnapLineOutlineColor = jsonToColor(config["esp"]["EnemySnapLineOutlineColor"]);
		esp.EnemySnapLineOutlineWidth = config["esp"]["EnemySnapLineOutlineWidth"];
		esp.EnemySnapLineWidth = config["esp"]["EnemySnapLineWidth"];
		esp.EnemyTargetBoneMarkerColor = jsonToColor(config["esp"]["EnemyTargetBoneMarkerColor"]);
		esp.EnemyTargetBoneMarkerLineWidth = config["esp"]["EnemyTargetBoneMarkerLineWidth"];
		esp.EnemyTargetBoneMarkerOutlineColor = jsonToColor(config["esp"]["EnemyTargetBoneMarkerOutlineColor"]);
		esp.EnemyTargetBoneMarkerOutlineWidth = config["esp"]["EnemyTargetBoneMarkerOutlineWidth"];
		esp.EnemyTargetBoneMarkerSize = config["esp"]["EnemyTargetBoneMarkerSize"];
		esp.EnemyTargetBoneMarkerType = config["esp"]["EnemyTargetBoneMarkerType"];
		esp.FriendlyBoxColor = jsonToColor(config["esp"]["FriendlyBoxColor"]);
		esp.FriendlyBoxLineWidth = config["esp"]["FriendlyBoxLineWidth"];
		esp.FriendlyBoxOutlineColor = jsonToColor(config["esp"]["FriendlyBoxOutlineColor"]);
		esp.FriendlyBoxOutlineWidth = config["esp"]["FriendlyBoxOutlineWidth"];
		esp.FriendlyHealthBarBackColor = jsonToColor(config["esp"]["FriendlyHealthBarBackColor"]);
		esp.FriendlyHealthBarFrontColor = jsonToColor(config["esp"]["FriendlyHealthBarFrontColor"]);
		esp.FriendlyHealthBarOutlineColor = jsonToColor(config["esp"]["FriendlyHealthBarOutlineColor"]);
		esp.FriendlyHealthBarOutlineWidth = config["esp"]["FriendlyHealthBarOutlineWidth"];
		esp.FriendlyHealthBarWidth = config["esp"]["FriendlyHealthBarWidth"];
		esp.FriendlyInfoTextColor = jsonToColor(config["esp"]["FriendlyInfoTextColor"]);
		esp.FriendlyInfoTextOutlineColor = jsonToColor(config["esp"]["FriendlyInfoTextOutlineColor"]);
		esp.FriendlySnapLineColor = jsonToColor(config["esp"]["FriendlySnapLineColor"]);
		esp.FriendlySnapLineOutlineColor = jsonToColor(config["esp"]["FriendlySnapLineOutlineColor"]);
		esp.FriendlySnapLineOutlineWidth = config["esp"]["FriendlySnapLineOutlineWidth"];
		esp.FriendlySnapLineWidth = config["esp"]["FriendlySnapLineWidth"];
		esp.FriendlyTargetBoneMarkerColor = jsonToColor(config["esp"]["FriendlyTargetBoneMarkerColor"]);
		esp.FriendlyTargetBoneMarkerLineWidth = config["esp"]["FriendlyTargetBoneMarkerLineWidth"];
		esp.FriendlyTargetBoneMarkerOutlineColor = jsonToColor(config["esp"]["FriendlyTargetBoneMarkerOutlineColor"]);
		esp.FriendlyTargetBoneMarkerOutlineWidth = config["esp"]["FriendlyTargetBoneMarkerOutlineWidth"];
		esp.FriendlyTargetBoneMarkerSize = config["esp"]["FriendlyTargetBoneMarkerSize"];
		esp.FriendlyTargetBoneMarkerType = config["esp"]["FriendlyTargetBoneMarkerType"];
		esp.RecoilMarkerColor = jsonToColor(config["esp"]["RecoilMarkerColor"]);
		esp.RecoilMarkerLineWidth = config["esp"]["RecoilMarkerLineWidth"];
		esp.RecoilMarkerOutlineColor = jsonToColor(config["esp"]["RecoilMarkerOutlineColor"]);
		esp.RecoilMarkerOutlineWidth = config["esp"]["RecoilMarkerOutlineWidth"];
		esp.RecoilMarkerSize = config["esp"]["RecoilMarkerSize"];
		esp.RecoilMarkerType = config["esp"]["RecoilMarkerType"];
		esp.TargetEnemyBoxColor = jsonToColor(config["esp"]["TargetEnemyBoxColor"]);
		esp.TargetEnemyBoxLineWidth = config["esp"]["TargetEnemyBoxLineWidth"];
		esp.TargetEnemyBoxOutlineColor = jsonToColor(config["esp"]["TargetEnemyBoxOutlineColor"]);
		esp.TargetEnemyBoxOutlineWidth = config["esp"]["TargetEnemyBoxOutlineWidth"];
		esp.TargetEnemySnapLineColor = jsonToColor(config["esp"]["TargetEnemySnapLineColor"]);
		esp.TargetEnemySnapLineOutlineColor = jsonToColor(config["esp"]["TargetEnemySnapLineOutlineColor"]);
		esp.TargetEnemySnapLineOutlineWidth = config["esp"]["TargetEnemySnapLineOutlineWidth"];
		esp.TargetEnemySnapLineWidth = config["esp"]["TargetEnemySnapLineWidth"];

		//weapons
		for (auto& element : config["weapons"].items()) {
			auto elementKey = element.key();
			auto elementData = element.value();
			int weaponId = elementData[0];
			auto weaponData = elementData[1];

			weapons[weaponId].name = weaponData["name"];
			weapons[weaponId].aimbot.element_key = elementKey;
			weapons[weaponId].aimbot.enabled = weaponData["aimbot"]["enabled"];
			weapons[weaponId].aimbot.start_bullet = weaponData["aimbot"]["start_bullet"];
			weapons[weaponId].aimbot.end_bullet = weaponData["aimbot"]["end_bullet"];
			weapons[weaponId].aimbot.delay = weaponData["aimbot"]["delay"];
			weapons[weaponId].aimbot.fov = weaponData["aimbot"]["fov"];
			weapons[weaponId].aimbot.smooth = weaponData["aimbot"]["smooth"];
			weapons[weaponId].aimbot.recoil_scale = weaponData["aimbot"]["recoil_scale"];
			weapons[weaponId].aimbot.delay_after_kill = weaponData["aimbot"]["delay_after_kill"];
			weapons[weaponId].aimbot.max_hit_accuracy = weaponData["aimbot"]["max_hit_accuracy"];
			weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot = weaponData["aimbot"]["max_hit_accuracy_for_first_shot"];
			weapons[weaponId].aimbot.chance_hit_closest_bone_first = weaponData["aimbot"]["chance_hit_closest_bone_first"];
			weapons[weaponId].aimbot.primary_bones[0] = weaponData["aimbot"]["primary_bones"][0];
			weapons[weaponId].aimbot.primary_bones[1] = weaponData["aimbot"]["primary_bones"][1];
			weapons[weaponId].aimbot.primary_bones[2] = weaponData["aimbot"]["primary_bones"][2];
			weapons[weaponId].aimbot.chance_to_switch = weaponData["aimbot"]["chance_to_switch"];
			weapons[weaponId].aimbot.time_to_switch = weaponData["aimbot"]["time_to_switch"];
			weapons[weaponId].aimbot.delay_to_stop_aiming = weaponData["aimbot"]["delay_to_stop_aiming"];
			weapons[weaponId].aimbot.time_to_kill = weaponData["aimbot"]["time_to_kill"];
			weapons[weaponId].aimbot.hit_chance_while_moving = weaponData["aimbot"]["hit_chance_while_moving"];
			weapons[weaponId].aimbot.first_position_of_critical_hit = weaponData["aimbot"]["first_position_of_critical_hit"];
			weapons[weaponId].aimbot.divergence_if_spotted = weaponData["aimbot"]["divergence_if_spotted"];
			weapons[weaponId].aimbot.aim_time_per_degree = weaponData["aimbot"]["aim_time_per_degree"];
			weapons[weaponId].aimbot.divergence_at_first_shot = weaponData["aimbot"]["divergence_at_first_shot"];
			
			//std::cout << "weaponId " << weaponId << " elementKey " << elementKey << "\n";
			//std::cout << "weaponId " << weaponId << " name " << weapons[weaponId].name << "\n";
		}

		Console::getInstance().println("[SUCCESS] loading config finished!", green, black);
	} catch (...) {
		Console::getInstance().println("[ERROR] loading config failed!", red, black);
		Sleep(3000);
	}
}

/**
	Writes the settings to the config.json file.

	@return nothing
*/
void Config::saveConfig() {
	try {
		std::ofstream os_cb("C:\\tubs\\config_backup.json");
		os_cb << std::setw(4) << config << std::endl;

		/*
		D3DCOLOR test_Color = D3DCOLOR_ARGB(255, 255, 0, 0);
		json testJSON = { "test_color", ColorToJson(test_Color) };
		config["test"] = testJSON;
		*/

		//general
		config["general"]["debug_mode"] = general.debug_mode;
		config["general"]["panic_mode"] = general.panic_mode;
		config["general"]["aimbot_enabled"] = general.aimbot_enabled;
		config["general"]["triggerbot_enabled"] = general.triggerbot_enabled;
		config["general"]["bspparsing_enabled"] = general.bspparsing_enabled;
		config["general"]["team_damage"] = general.team_damage;
		config["general"]["game_windowtitle"] = general.game_windowtitle;
		config["general"]["profiler_enabled"] = general.profiler_enabled;
		config["general"]["ready_for_use"] = general.ready_for_use;

		/*
		//keys
		config["keys"]["ingame_fire"] = keys.ingame_fire;
		config["keys"]["aimbot_toggle"] = keys.aimbot_toggle;
		config["keys"]["aimbot_hold"] = keys.aimbot_hold;
		config["keys"]["triggerbot_hold"] = keys.triggerbot_hold;
		config["keys"]["triggerbot_toggle"] = keys.triggerbot_toggle;
		config["keys"]["panic_mode"] = keys.panic_mode;
		config["keys"]["reload_config"] = keys.reload_config;
		config["keys"]["profiler_toggle"] = keys.profiler_toggle;
		config["keys"]["start_analyzer"] = keys.start_analyzer;
		config["keys"]["start_adjustment"] = keys.start_adjustment;
		config["keys"]["ingame_switch_weapon"] = keys.ingame_switch_weapon;
		*/
		
		//aimbot
		config["aimbot"]["adaptive_smooth_multiplier"] = aimbot.adaptive_smooth_multiplier;
		config["aimbot"]["adaptive_smooth_rifles"] = aimbot.adaptive_smooth_rifles;
		config["aimbot"]["adaptive_smooth_pistols"] = aimbot.adaptive_smooth_pistols;
		config["aimbot"]["adaptive_smooth_snipers"] = aimbot.adaptive_smooth_snipers;
		config["aimbot"]["adaptive_smooth_smgs"] = aimbot.adaptive_smooth_smgs;
		config["aimbot"]["adaptive_smooth_shotguns"] = aimbot.adaptive_smooth_shotguns;
		config["aimbot"]["adaptive_smooth_lmgs"] = aimbot.adaptive_smooth_lmgs;
		config["aimbot"]["spiral_aim_enabled"] = aimbot.spiral_aim_enabled;
		config["aimbot"]["spiral_aim_x"] = aimbot.spiral_aim_x;
		config["aimbot"]["spiral_aim_y"] = aimbot.spiral_aim_y;
		config["aimbot"]["spiral_aim_relative_position"] = aimbot.spiral_aim_relative_position;
		config["aimbot"]["spiral_aim_noise"] = aimbot.spiral_aim_noise;
		config["aimbot"]["faceit_aim"] = aimbot.faceit_aim;
		config["aimbot"]["save_target_aim"] = aimbot.save_target_aim;
		config["aimbot"]["chance_for_pre_shot"] = aimbot.chance_for_pre_shot;
		config["aimbot"]["fov_noise"] = aimbot.fov_noise;
		config["aimbot"]["recoil_scale_noise"] = aimbot.recoil_scale_noise;
		config["aimbot"]["smooth_noise"] = aimbot.smooth_noise;
		config["aimbot"]["chance_to_switch_noise"] = aimbot.chance_to_switch_noise;
		config["aimbot"]["time_to_switch_noise"] = aimbot.time_to_switch_noise;
		config["aimbot"]["kill_delay_noise"] = aimbot.kill_delay_noise;
		config["aimbot"]["time_to_kill_noise"] = aimbot.time_to_kill_noise;
		config["aimbot"]["delay_to_start_aiming_noise"] = aimbot.delay_to_start_aiming_noise;
		config["aimbot"]["delay_to_stop_aiming_noise"] = aimbot.delay_to_stop_aiming_noise;
		config["aimbot"]["chance_hit_closest_bone_first_noise"] = aimbot.chance_hit_closest_bone_first_noise;
		config["aimbot"]["first_position_of_critical_hit_noise"] = aimbot.first_position_of_critical_hit_noise;
		config["aimbot"]["hit_chance_while_moving_noise"] = aimbot.hit_chance_while_moving_noise;
		config["aimbot"]["max_hit_accuracy_noise"] = aimbot.max_hit_accuracy_noise;
		config["aimbot"]["max_hit_accuracy_for_first_shot_noise"] = aimbot.max_hit_accuracy_for_first_shot_noise;
		config["aimbot"]["num_primary_bones_choice"] = aimbot.num_primary_bones_choice;
		config["aimbot"]["compensate_velocity"] = aimbot.compensate_velocity;
		config["aimbot"]["max_inaccuracy_offset"] = aimbot.max_inaccuracy_offset;
		config["aimbot"]["divergence_at_first_shot_noise"] = aimbot.divergence_at_first_shot_noise;
		
		//triggerbot
		config["triggerbot"]["fov_based"] = triggerbot.fov_based;
		config["triggerbot"]["delay"] = triggerbot.delay;

		//weapons
		for (int weaponId = 1; weaponId < 65; weaponId++) {
			if (weapons[weaponId].aimbot.element_key.length() > 0) {
				//std::cout << weaponId << " " << weapons[weaponId].aimbot.element_key << "\n";
				int key = std::stoi(weapons[weaponId].aimbot.element_key);
				config["weapons"][key][1]["aimbot"]["enabled"] = weapons[weaponId].aimbot.enabled;
				config["weapons"][key][1]["aimbot"]["start_bullet"] = weapons[weaponId].aimbot.start_bullet;
				config["weapons"][key][1]["aimbot"]["end_bullet"] = weapons[weaponId].aimbot.end_bullet;
				config["weapons"][key][1]["aimbot"]["delay"] = weapons[weaponId].aimbot.delay;
				config["weapons"][key][1]["aimbot"]["fov"] = weapons[weaponId].aimbot.fov;
				config["weapons"][key][1]["aimbot"]["smooth"] = weapons[weaponId].aimbot.smooth;
				config["weapons"][key][1]["aimbot"]["recoil_scale"] = weapons[weaponId].aimbot.recoil_scale;
				config["weapons"][key][1]["aimbot"]["delay_after_kill"] = weapons[weaponId].aimbot.delay_after_kill;
				config["weapons"][key][1]["aimbot"]["max_hit_accuracy"] = weapons[weaponId].aimbot.max_hit_accuracy;
				config["weapons"][key][1]["aimbot"]["max_hit_accuracy_for_first_shot"] = weapons[weaponId].aimbot.max_hit_accuracy_for_first_shot;
				config["weapons"][key][1]["aimbot"]["chance_hit_closest_bone_first"] = weapons[weaponId].aimbot.chance_hit_closest_bone_first;
				config["weapons"][key][1]["aimbot"]["primary_bones"][0] = weapons[weaponId].aimbot.primary_bones[0];
				config["weapons"][key][1]["aimbot"]["primary_bones"][1] = weapons[weaponId].aimbot.primary_bones[1];
				config["weapons"][key][1]["aimbot"]["primary_bones"][2] = weapons[weaponId].aimbot.primary_bones[2];
				config["weapons"][key][1]["aimbot"]["chance_to_switch"] = weapons[weaponId].aimbot.chance_to_switch;
				config["weapons"][key][1]["aimbot"]["time_to_switch"] = weapons[weaponId].aimbot.time_to_switch;
				config["weapons"][key][1]["aimbot"]["delay_to_stop_aiming"] = weapons[weaponId].aimbot.delay_to_stop_aiming;
				config["weapons"][key][1]["aimbot"]["time_to_kill"] = weapons[weaponId].aimbot.time_to_kill;
				config["weapons"][key][1]["aimbot"]["hit_chance_while_moving"] = weapons[weaponId].aimbot.hit_chance_while_moving;
				config["weapons"][key][1]["aimbot"]["first_position_of_critical_hit"] = weapons[weaponId].aimbot.first_position_of_critical_hit;
				config["weapons"][key][1]["aimbot"]["divergence_if_spotted"] = weapons[weaponId].aimbot.divergence_if_spotted;
				config["weapons"][key][1]["aimbot"]["aim_time_per_degree"] = weapons[weaponId].aimbot.aim_time_per_degree;
				config["weapons"][key][1]["aimbot"]["divergence_at_first_shot"] = weapons[weaponId].aimbot.divergence_at_first_shot;
			}
		}
		
		//analyzer
		config["analyzer"]["played_matchmakings"] = analyzer.played_matchmakings;
		config["analyzer"]["matchmakings_wins"] = analyzer.matchmakings_wins;
		config["analyzer"]["matchmakings_loses"] = analyzer.matchmakings_loses;
		config["analyzer"]["matchmakings_draws"] = analyzer.matchmakings_draws;
		config["analyzer"]["played_time"] = analyzer.played_time;
		config["analyzer"]["action_for_not_enough_values"] = analyzer.action_for_not_enough_values;
		config["analyzer"]["next_adjustment"] = analyzer.next_adjustment;
		
		std::ofstream o("C:\\tubs\\config.json");
		o << std::setw(4) << config << std::endl;

		//Console::getInstance().println("[SUCCESS] saving config finished!", green, black);
	} catch (...) {
		Console::getInstance().println("[ERROR] saving config failed!", red, black);
	}
}

/**
	Converts a color from D3DCOLOR to json.

	@param color the color in D3DCOLOR format
	@return the color in json format
*/
json Config::colorToJson(D3DCOLOR color) {
	BYTE a = color >> 24;
	BYTE r = (color >> 16) & 0xFF;
	BYTE g = (color >> 8) & 0xFF;
	BYTE b = color & 0xFF;
	json colorJSON = {{"a", a}, {"r", r}, {"g", g}, {"b", b}};
	return colorJSON;
}

/**
	Converts a color from json to D3DCOLOR.

	@param color the color in json format
	@return the color in D3DCOLOR format
*/
D3DCOLOR Config::jsonToColor(json color) {
	return D3DCOLOR_ARGB(color["a"], color["r"], color["g"], color["b"]);
}

/**
	Returns the weapon type of a given weapon.

	@param weaponId the id of the weapon
	@return the type of the given weapon as a string
*/
const char* Config::weaponType(int weaponId) {
	switch (weaponId) {
		case WID_Negev:
		case WID_M249:
			return "(LMG)";

		case WID_AWP:
		case WID_G3SG1_Auto:
		case WID_SCAR_Auto:
		case WID_Scout:
			return "Snipers";

		case WID_XM1014:
		case WID_MAG7:
		case WID_Nova:
		case WID_SawedOff:
			return "Shotguns";

		case WID_MAC10:
		case WID_P90:
		case WID_UMP45:
		case WID_PPBizon:
		case WID_MP7:
		case WID_MP9:
			return "SMGs";

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
		case WID_Zeus:
			return "Pistols";

		case WID_AK47:
		case WID_AUG:
		case WID_FAMAS:
		case WID_M4A4:
		case WID_M4A1S:
		case WID_Galil:
		case WID_SG553:
			return "Rifles";

		default:
			return "null";
	}
}