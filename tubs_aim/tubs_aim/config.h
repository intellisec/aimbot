#pragma once

#define MAX_WEAPON_ID			(64+1)

class Config {
public:
	void	saveConfig();
	void	loadConfig();

	struct analyzer {
		int				played_matchmakings;
		int				matchmakings_wins;
		int				matchmakings_loses;
		int				matchmakings_draws;
		time_t			played_time;
		int				action_for_not_enough_values;
		time_t			next_adjustment;
	}analyzer;

	struct general {
		bool			debug_mode;
		bool			panic_mode;
		bool			aimbot_enabled;
		bool			triggerbot_enabled;
		bool			bspparsing_enabled;
		bool			team_damage;
		std::string		game_windowtitle;
		std::string		cheat_windowname;
		bool			profiler_enabled;
		bool			ready_for_use;
	}general;

	struct keys {
		int				ingame_fire;
		int				aimbot_toggle;
		int				aimbot_hold;
		int				triggerbot_hold;
		int				triggerbot_toggle;
		int				panic_mode;
		int				reload_config;
		int				profiler_toggle;
		int				start_analyzer;
		int				start_adjustment;
		int				ingame_switch_weapon;
	}keys;

	struct aimbot {
		bool			compensate_velocity;
		float			adaptive_smooth_multiplier;
		bool			adaptive_smooth_rifles;
		bool			adaptive_smooth_pistols;
		bool			adaptive_smooth_snipers;
		bool			adaptive_smooth_smgs;
		bool			adaptive_smooth_shotguns;
		bool			adaptive_smooth_lmgs;
		bool			faceit_aim;
		bool			save_target_aim;
		bool			spiral_aim_enabled;
		float			spiral_aim_x;
		float			spiral_aim_y;
		float			spiral_aim_relative_position;
		float			spiral_aim_noise;
		float			chance_for_pre_shot;
		float			fov_noise;
		float			recoil_scale_noise;
		float			smooth_noise;
		float			chance_to_switch_noise;
		float			time_to_switch_noise;
		float			kill_delay_noise;
		float			time_to_kill_noise;
		float			delay_to_start_aiming_noise;
		float			delay_to_stop_aiming_noise;
		float			chance_hit_closest_bone_first_noise;
		float			first_position_of_critical_hit_noise;
		float			hit_chance_while_moving_noise;
		float			max_hit_accuracy_noise;
		float			max_hit_accuracy_for_first_shot_noise;
		int				num_primary_bones_choice;
		float			max_inaccuracy_offset;
		float			divergence_at_first_shot_noise;
	}aimbot;

	struct triggerbot {
		bool			fov_based;
		int				delay;
	}triggerbot;

	struct esp {
		bool enabled;

		bool DrawEnemyBox;
		bool DrawTargetBoxHighlight;
		float TargetEnemyBoxLineWidth;
		D3DCOLOR TargetEnemyBoxColor;
		float TargetEnemyBoxOutlineWidth;
		D3DCOLOR TargetEnemyBoxOutlineColor;
		float EnemyBoxLineWidth;
		D3DCOLOR EnemyBoxColor;
		float EnemyBoxOutlineWidth;
		D3DCOLOR EnemyBoxOutlineColor;

		bool DrawFriendlyBox;
		float FriendlyBoxLineWidth;
		D3DCOLOR FriendlyBoxColor;
		float FriendlyBoxOutlineWidth;
		D3DCOLOR FriendlyBoxOutlineColor;

		bool DrawEnemySnapLines;
		bool DrawTargetSnapLineHighlight;
		float TargetEnemySnapLineWidth;
		D3DCOLOR TargetEnemySnapLineColor;
		float TargetEnemySnapLineOutlineWidth;
		D3DCOLOR TargetEnemySnapLineOutlineColor;
		float EnemySnapLineWidth;
		D3DCOLOR EnemySnapLineColor;
		float EnemySnapLineOutlineWidth;
		D3DCOLOR EnemySnapLineOutlineColor;

		bool DrawFriendlySnapLines;
		float FriendlySnapLineWidth;
		D3DCOLOR FriendlySnapLineColor;
		float FriendlySnapLineOutlineWidth;
		D3DCOLOR FriendlySnapLineOutlineColor;

		bool DrawEnemyHealthBar;
		float EnemyHealthBarWidth;
		D3DCOLOR EnemyHealthBarBackColor;
		float EnemyHealthBarOutlineWidth;
		D3DCOLOR EnemyHealthBarOutlineColor;
		D3DCOLOR EnemyHealthBarFrontColor;

		bool DrawFriendlyHealthBar;
		float FriendlyHealthBarWidth;
		D3DCOLOR FriendlyHealthBarBackColor;
		float FriendlyHealthBarOutlineWidth;
		D3DCOLOR FriendlyHealthBarOutlineColor;
		D3DCOLOR FriendlyHealthBarFrontColor;

		D3DCOLOR EnemyInfoTextColor;
		D3DCOLOR EnemyInfoTextOutlineColor;
		bool DrawEnemyNameText;
		bool DrawEnemyHealthText;
		bool DrawEnemyKillsText;
		bool DrawEnemyDeathsText;
		bool DrawEnemyKDRatioText;
		bool DrawEnemyRankText;
		bool DrawEnemyBombCarrierText;
		bool DrawEnemyDefuseKitCarrierText;

		D3DCOLOR FriendlyInfoTextColor;
		D3DCOLOR FriendlyInfoTextOutlineColor;
		bool DrawFriendlyNameText;
		bool DrawFriendlyHealthText;
		bool DrawFriendlyKillsText;
		bool DrawFriendlyDeathsText;
		bool DrawFriendlyKDRatioText;
		bool DrawFriendlyRankText;
		bool DrawFriendlyBombCarrierText;
		bool DrawFriendlyTargetBoneMarker;
		bool DrawFriendlyDefuseKitCarrierText;

		int EnemyTargetBoneMarkerType;
		float EnemyTargetBoneMarkerSize;
		float EnemyTargetBoneMarkerLineWidth;
		D3DCOLOR EnemyTargetBoneMarkerColor;
		float EnemyTargetBoneMarkerOutlineWidth;
		D3DCOLOR EnemyTargetBoneMarkerOutlineColor;

		bool DrawEnemyTargetBoneMarker;
		int FriendlyTargetBoneMarkerType;
		float FriendlyTargetBoneMarkerSize;
		float FriendlyTargetBoneMarkerLineWidth;
		D3DCOLOR FriendlyTargetBoneMarkerColor;
		float FriendlyTargetBoneMarkerOutlineWidth;
		D3DCOLOR FriendlyTargetBoneMarkerOutlineColor;

		bool DrawRecoilMarker;
		int	RecoilMarkerType;
		float RecoilMarkerSize;
		float RecoilMarkerLineWidth;
		D3DCOLOR RecoilMarkerColor;
		float RecoilMarkerOutlineWidth;
		D3DCOLOR RecoilMarkerOutlineColor;
	}esp;

	struct weapons {
		std::string name;
		struct aimbot {
			std::string		element_key;
			bool			enabled = false;
			int				start_bullet = 0;
			int				end_bullet = 0;
			int				delay = 0;
			float			fov = 0.0f;
			float			smooth = 0.0f;
			float			recoil_scale = 0.0f;
			int				delay_after_kill = 0;
			float			max_hit_accuracy = 0.0f;
			float			max_hit_accuracy_for_first_shot = 0.0f;
			float			chance_hit_closest_bone_first = 0.0f;
			int				primary_bones[3] = {6, 8, 4};
			float			chance_to_switch = 0.5f;
			int				time_to_switch = 250;
			float			delay_to_stop_aiming = 25.0f;
			int				time_to_kill = 1000;
			float			hit_chance_while_moving = 0.5f;
			float			first_position_of_critical_hit = 0.5f;
			float			divergence_if_spotted = 20.0f;
			float			aim_time_per_degree = 50.0f;
			float			divergence_at_first_shot = 8.0f;
			int				inaccuracy_offset = 0;
			int				inaccuracy_offset_for_first_shot = 0;
		}aimbot;
	}weapons[MAX_WEAPON_ID];

private:
	D3DCOLOR		jsonToColor(json color);
	json			colorToJson(D3DCOLOR color);

	json			config;
	const char*		weaponType(int weaponId);

	const char*		configFilePath = "C:\\tubs\\";
};

extern Config* config;