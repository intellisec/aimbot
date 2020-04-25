#include "stdafx.h"
#include "dllmain.h"

std::unique_ptr< Valve::BSPParser > g_pBSPParser = std::make_unique< Valve::BSPParser >();
std::string g_GameDirectory;
std::string m_pMapDirectory;

/**
	Entry point from the visibility module.

	At this point it is checked which players are visible to the local player.

	@return nothing
*/
void Visibility::start() {

	try {
		g_GameDirectory = game->getGameDirectory();
		for (;;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(5));

			if (!config->general.bspparsing_enabled)
				continue;

			m_pMapDirectory = game->getMapDirectory();

			if (m_pMapDirectory.empty() || g_GameDirectory.empty())
				continue;

			g_pBSPParser->parse_map(g_GameDirectory, m_pMapDirectory);
			
			if (!game->isInGame())
				continue;

			if (!game->isTeamSelected())
				continue;

			if (!localEntity.isValid())
				continue;

			for (int playerIndex = 0; playerIndex <= game->getMaxClients(); playerIndex++) {
				std::this_thread::yield();

				if (!entityList[playerIndex].isValid())
					continue;

				if (!config->general.team_damage && entityList[playerIndex].getTeamNum() == localEntity.getTeamNum())
					continue;

				static Vector localPosition, entityPosition;
				static Valve::Vector3 localPosition_v3, entityPosition_v3;
				static bool isVisible;
				static INT64 isSpottedByMask;

				localPosition = localEntity.getEyePosition();
				entityPosition = entityList[playerIndex].getEyePosition();
				localPosition_v3 = { localPosition.x, localPosition.y, localPosition.z };
				entityPosition_v3 = { entityPosition.x, entityPosition.y, entityPosition.z };

				isVisible = g_pBSPParser->is_visible(localPosition_v3, entityPosition_v3);

				isSpottedByMask = entityList[playerIndex].isSpottedByMask();

				//Console::getInstance().println("playerIndex " + std::to_string(playerIndex) + " isVisible: " + std::to_string(isVisible));
				//Console::getInstance().println("playerIndex " + std::to_string(playerIndex) + " isSpottedByMask: " + std::to_string(isSpottedByMask));

				entityList[playerIndex].setBSPVisible(isVisible);
				entityList[playerIndex].setVisible(((isSpottedByMask != 0 && isVisible == true) ? true : false));
			}
		}
	} catch (...) {
		memory->debuglog("VisibilityCheckHandler");
	}
}