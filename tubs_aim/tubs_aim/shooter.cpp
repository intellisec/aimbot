#include "stdafx.h"
#include "dllmain.h"

/**
	Entry point from the shooter module.

	Here, the shooting of the player is controlled.

	@return nothing
*/
void Shooter::start() {
	try {
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

			if (!Shooter::canShoot())
				continue;

			switch (localEntity.getWeaponType()) {
			case EWeaponType::WeaponType_Rifle:
			case EWeaponType::WeaponType_Pistol:
			case EWeaponType::WeaponType_SMG:
			case EWeaponType::WeaponType_Sniper:
			case EWeaponType::WeaponType_Shotgun:
			case EWeaponType::WeaponType_LMG:
				if (GetAsyncKeyState(config->keys.aimbot_hold)) {
					Sleep(config->weapons[localEntity.getActiveWeapon()].aimbot.delay);
					while (GetAsyncKeyState(config->keys.aimbot_hold)) {
						Shooter::fire();
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}

					if (!GetAsyncKeyState(config->keys.aimbot_hold)) {
						game->pressAttackKey();
						std::this_thread::sleep_for(std::chrono::milliseconds(5));
						game->releaseAttackKey();
						break;
					}
				}
				break;
			default:
				Shooter::fire();
				break;
			}
			Shooter::fire();

		}
	}
	catch (...) {
		memory->debuglog("Shooter -> main");
	}
	
}

/**
	Checks if the player is allowed to shoot. 

	The game must be open in the foreground.
	And the mouse position may not be in the range of the buy menu.

	@return nothing
*/
bool Shooter::canShoot() {
	//prevent shots when the game is not in the foreground.
	if (!FindWindow(0, "Counter-Strike: Global Offensive")) {
		//Console::getInstance().println("window not found");
		return false;
	}

	//prevent shots in the purchase menu
	HWND hwnd = memory->GetWindow();
	RECT rect, rectClient;
	if (GetWindowRect(hwnd, &rect) && GetClientRect(hwnd, &rectClient))	{
		POINT cursorPosition;
		GetCursorPos(&cursorPosition);

		int difference = 15;

		int borderThickness = ((rect.right - rect.left) - rectClient.right) / 2;
		int navbarThickness = ((rect.bottom - rect.top) - rectClient.bottom) - borderThickness;

		int width = rect.right - rect.left - (2 * borderThickness); // (Berechnung ist 6 größer als Auslösung im Spiel)
		int height = rect.bottom - rect.top - borderThickness - navbarThickness; // (Berechnung ist 29 größer als Auslösung im Spiel)

		int gameWindowCenterX = rect.left + (width / 2) + borderThickness;
		int gameWindowCenterY = rect.top + (height / 2) + navbarThickness;

		//Console::getInstance().println("border_thickness: " + std::to_string(border_thickness) + " navbar_thickness: " + std::to_string(navbar_thickness));
		//Console::getInstance().println("w: " + std::to_string(width) + " h: " + std::to_string(height) + " cx: " + std::to_string(centerx2) + " cy: " + std::to_string(centery2) + " cp.x: " + std::to_string(CurPos.x) + " cp.y: " + std::to_string(CurPos.y));
		
		if (cursorPosition.x < gameWindowCenterX - difference || cursorPosition.x > gameWindowCenterX + difference) {
			return false;
		}
		if (cursorPosition.y < gameWindowCenterY - difference || cursorPosition.y > gameWindowCenterY + difference) {
			return false;
		}
	} else {
		return false;
	}
	return true;
}

/**
	Fires the players weapon. Presses and releases the fire key.

	@return nothing
*/
void Shooter::fire() {
	INPUT    Input = { 0 };

	if (!GetAsyncKeyState(config->keys.aimbot_hold) && GetAsyncKeyState(config->keys.ingame_fire)) {
		//Console::getInstance().println("Release Fire Key");
		//Release Fire Key
		ZeroMemory(&Input, sizeof(INPUT));
		WORD vkey = config->keys.ingame_fire;

		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_KEYUP | KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));
	}

	if (GetAsyncKeyState(config->keys.aimbot_hold) && !GetAsyncKeyState(config->keys.ingame_fire)) {
		//Console::getInstance().println("Press Fire Key");
		//Press Fire Key
		ZeroMemory(&Input, sizeof(INPUT));
		WORD vkey = config->keys.ingame_fire;

		Input.type = INPUT_KEYBOARD;
		Input.ki.wScan = MapVirtualKey(vkey, MAPVK_VK_TO_VSC);
		Input.ki.time = 0;
		Input.ki.dwExtraInfo = 0;
		Input.ki.wVk = vkey;
		Input.ki.dwFlags = KEYEVENTF_SCANCODE;
		SendInput(1, &Input, sizeof(INPUT));
	}
}
