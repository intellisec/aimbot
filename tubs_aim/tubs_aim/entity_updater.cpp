#include "stdafx.h"
#include "dllmain.h"

/**
	Entry point from the entity updater module.

	Here, the update method is called for each Entity Object.
	It also checks if the player presses any keys to trigger actions.

	@return nothing
*/
void EntityUpdater::start() {
	try {
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (config->keys.reload_config != 0 && GetAsyncKeyState(config->keys.reload_config)) {
				config->loadConfig();
				Console::getInstance().printInfo();
				Beep(1000, 200);
			}

			if (config->keys.aimbot_toggle != 0 && GetAsyncKeyState(config->keys.aimbot_toggle)) {
				if (config->general.ready_for_use) {
					config->general.aimbot_enabled = !config->general.aimbot_enabled;
					if (config->general.aimbot_enabled) {
						Beep(1000, 200);
					} else {
						Beep(500, 200);
					}
					Console::getInstance().printInfo();
					Sleep(200);
				} else {
					Console::getInstance().println("Profiling not yet completed. Run Analyzer first.");
				}
			}

			if (config->keys.triggerbot_toggle != 0 && GetAsyncKeyState(config->keys.triggerbot_toggle)) {
				if (config->general.ready_for_use) {
					config->general.triggerbot_enabled = !config->general.triggerbot_enabled;
					if (config->general.triggerbot_enabled) {
						Beep(1000, 200);
					} else {
						Beep(500, 200);
					}
					Console::getInstance().printInfo();
					Sleep(200);
				} else {
					Console::getInstance().println("Profiling not yet completed. Run Analyzer first.");
				}
			}

			static bool is_clicked = false;
			if (config->keys.panic_mode != 0 && GetAsyncKeyState(config->keys.panic_mode)) {
				if (!is_clicked) {
					config->general.panic_mode = true;
					Sleep(50);
					game->forceFullUpdate();
					Beep(1000, 200);

					Console::getInstance().clear();
					Console::getInstance().println("Hold END \nfor exit from panic mode!", red, black);

					unsigned int panicTimer = 0;
					while (panicTimer < 2000) {
						if (GetAsyncKeyState(config->keys.panic_mode) & 0x8000) {
							panicTimer++;
						} else {
							panicTimer = 0;
						}
						Sleep(1);
					}
					Beep(800, 100);
					Beep(800, 100);
					config->general.panic_mode = false;
					Console::getInstance().printInfo();
					is_clicked = true;
				}
			} else {
				is_clicked = false;
			}

			if (!Helper::getInstance().doesCSGOExist()) exit(0);

			if (!Helper::getInstance().checkCSGOWindowState())
				continue;

			if (game->isInGame()) {
				int iLocalIndex = game->getLocalPlayer();
				localEntity.update(iLocalIndex);

				for (int iIndex = 0; iIndex <= game->getMaxClients(); iIndex++) {
					entityList[iIndex].update(iIndex);
				}
			}
		}
	} catch (...) {
		memory->debuglog("EntityUpdateHandler");
	}
}