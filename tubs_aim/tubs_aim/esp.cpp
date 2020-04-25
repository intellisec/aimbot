#include "stdafx.h"
#include "dllmain.h"

bool tESP::tESPFunctions::OnScreen(D3DXVECTOR2 ScreenPosition)
{
	if (ScreenPosition.x >= pOverlayWindow.X && ScreenPosition.x <= pOverlayWindow.Width && ScreenPosition.y >= pOverlayWindow.Y && ScreenPosition.y <= pOverlayWindow.Height)
	{
		return true;
	}
	return false;
}

D3DXVECTOR2 tESP::tESPFunctions::WorldToScreen(Vector EnemyPosition, Matrix4x4 ViewMatrix)
{
	D3DXVECTOR2 ScreenPosition;
	if (ViewMatrix.Matrix)
	{
		ScreenPosition.x = ViewMatrix.Matrix[0][0] * EnemyPosition.x + ViewMatrix.Matrix[0][1] * EnemyPosition.y + ViewMatrix.Matrix[0][2] * EnemyPosition.z + ViewMatrix.Matrix[0][3];
		ScreenPosition.y = ViewMatrix.Matrix[1][0] * EnemyPosition.x + ViewMatrix.Matrix[1][1] * EnemyPosition.y + ViewMatrix.Matrix[1][2] * EnemyPosition.z + ViewMatrix.Matrix[1][3];
		float w = ViewMatrix.Matrix[3][0] * EnemyPosition.x + ViewMatrix.Matrix[3][1] * EnemyPosition.y + ViewMatrix.Matrix[3][2] * EnemyPosition.z + ViewMatrix.Matrix[3][3];
		if (w > 0.01f)
		{
			float invw = 1.0f / w;
			ScreenPosition.x *= invw;
			ScreenPosition.y *= invw;
			float x = pOverlayWindow.Width / 2.f;
			float y = pOverlayWindow.Height / 2.f;
			x += 0.5f * ScreenPosition.x * pOverlayWindow.Width + 0.5f;
			y -= 0.5f * ScreenPosition.y * pOverlayWindow.Height + 0.5f;
			ScreenPosition.x = x + pOverlayWindow.X;
			ScreenPosition.y = y + pOverlayWindow.Y;
		}
		else
		{
			ScreenPosition.x = -1.f;
			ScreenPosition.y = -1.f;
		}
	}
	return ScreenPosition;
}

void tESP::tESPFunctions::DrawBox(int pIndex)
{
	if (config->esp.DrawEnemyBox)
	{
		if (localEntity.getTeamNum() != entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			Vector OriginPlusHeight = entityList[pIndex].getOrigin();
			OriginPlusHeight.z += 80.f;
			D3DXVECTOR2 TopPosition = pESP.Functions.WorldToScreen(OriginPlusHeight, game->getViewMatrix());
			if (config->esp.DrawTargetBoxHighlight && localEntity.getCrosshairEnt() - 1 == pIndex)
			{
				float x, y, h, w;
				y = TopPosition.y;
				h = BottomPosition.y - TopPosition.y;
				w = h * (45.f / 80.f);
				x = TopPosition.x - w / 2.f;
				pD3D.Functions.DrawRect(x, y, w, h, config->esp.TargetEnemyBoxLineWidth, config->esp.TargetEnemyBoxColor, false, true, config->esp.TargetEnemyBoxOutlineWidth, config->esp.TargetEnemyBoxOutlineColor);
			}
			else
			{
				float x, y, h, w;
				y = TopPosition.y;
				h = BottomPosition.y - TopPosition.y;
				w = h * (45.f / 80.f);
				x = TopPosition.x - w / 2.f;
				D3DXVECTOR2 ScreenPosition;
				ScreenPosition.x = x;
				ScreenPosition.y = y;
				if (OnScreen(ScreenPosition))
				{
					pD3D.Functions.DrawRect(x, y, w, h, config->esp.EnemyBoxLineWidth, config->esp.EnemyBoxColor, false, true, config->esp.EnemyBoxOutlineWidth, config->esp.EnemyBoxOutlineColor);
				}
			}
		}
	}
	if (config->esp.DrawFriendlyBox)
	{
		if (localEntity.getTeamNum() == entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			Vector OriginPlusHeight = entityList[pIndex].getOrigin();
			OriginPlusHeight.z += 80.f;
			D3DXVECTOR2 TopPosition = pESP.Functions.WorldToScreen(OriginPlusHeight, game->getViewMatrix());
			float x, y, h, w;
			y = TopPosition.y;
			h = BottomPosition.y - TopPosition.y;
			w = h * (45.f / 80.f);
			x = TopPosition.x - w / 2.f;
			D3DXVECTOR2 ScreenPosition;
			ScreenPosition.x = x;
			ScreenPosition.y = y;
			if (OnScreen(ScreenPosition))
			{
				pD3D.Functions.DrawRect(x, y, w, h, config->esp.FriendlyBoxLineWidth, config->esp.FriendlyBoxColor, false, true, config->esp.FriendlyBoxOutlineWidth, config->esp.FriendlyBoxOutlineColor);
			}
		}
	}
}

void tESP::tESPFunctions::DrawSnapLines(int pIndex)
{
	if (config->esp.DrawEnemySnapLines)
	{
		if (localEntity.getTeamNum() != entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			if (config->esp.DrawTargetSnapLineHighlight && localEntity.getCrosshairEnt() - 1 == pIndex)
			{
				pD3D.Functions.DrawLine(pOverlayWindow.Width / 2.f, pOverlayWindow.Height, BottomPosition.x, BottomPosition.y, config->esp.TargetEnemySnapLineWidth, config->esp.TargetEnemySnapLineColor, true, config->esp.TargetEnemySnapLineOutlineWidth, config->esp.TargetEnemySnapLineOutlineColor);
			}
			else if (OnScreen(BottomPosition))
			{
				pD3D.Functions.DrawLine(pOverlayWindow.Width / 2.f, pOverlayWindow.Height, BottomPosition.x, BottomPosition.y, config->esp.EnemySnapLineWidth, config->esp.EnemySnapLineColor, true, config->esp.EnemySnapLineOutlineWidth, config->esp.EnemySnapLineOutlineColor);
			}
		}
	}
	if (config->esp.DrawFriendlySnapLines)
	{
		if (localEntity.getTeamNum() == entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			if (OnScreen(BottomPosition))
			{
				pD3D.Functions.DrawLine(pOverlayWindow.Width / 2.f, pOverlayWindow.Height, BottomPosition.x, BottomPosition.y, config->esp.FriendlySnapLineWidth, config->esp.FriendlySnapLineColor, true, config->esp.FriendlySnapLineOutlineWidth, config->esp.FriendlySnapLineOutlineColor);
			}
		}
	}
}

void tESP::tESPFunctions::DrawHealthBar(int pIndex)
{
	if (config->esp.DrawEnemyHealthBar)
	{
		if (localEntity.getTeamNum() != entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			Vector OriginPlusHeight = entityList[pIndex].getOrigin();
			OriginPlusHeight.z += 80.f;
			D3DXVECTOR2 TopPosition = pESP.Functions.WorldToScreen(OriginPlusHeight, game->getViewMatrix());
			float x, y, h, w;
			y = TopPosition.y;
			h = BottomPosition.y - TopPosition.y;
			w = h * (config->esp.EnemyHealthBarWidth / 80.f);
			x = TopPosition.x - (h * (45.f / 80.f)) / 2.f;
			D3DXVECTOR2 ScreenPosition;
			ScreenPosition.x = x;
			ScreenPosition.y = y;
			if (OnScreen(ScreenPosition))
			{
				pD3D.Functions.DrawRect(x, y, w, h, 0, config->esp.EnemyHealthBarBackColor, true, true, config->esp.EnemyHealthBarOutlineWidth, config->esp.EnemyHealthBarOutlineColor);
				pD3D.Functions.DrawRect(x, y += (h / 100.f) * (100 - entityList[pIndex].getHealth()), w, (h / 100.f) * entityList[pIndex].getHealth(), 0, config->esp.EnemyHealthBarFrontColor, true, true, config->esp.EnemyHealthBarOutlineWidth, config->esp.EnemyHealthBarOutlineColor);
			}
		}
	}
	if (config->esp.DrawFriendlyHealthBar)
	{
		if (localEntity.getTeamNum() == entityList[pIndex].getTeamNum())
		{
			D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
			Vector OriginPlusHeight = entityList[pIndex].getOrigin();
			OriginPlusHeight.z += 80.f;
			D3DXVECTOR2 TopPosition = pESP.Functions.WorldToScreen(OriginPlusHeight, game->getViewMatrix());
			float x, y, h, w;
			y = TopPosition.y;
			h = BottomPosition.y - TopPosition.y;
			w = h * (config->esp.FriendlyHealthBarWidth / 80.f);
			x = TopPosition.x - (h * (45.f / 80.f)) / 2.f;
			D3DXVECTOR2 ScreenPosition;
			ScreenPosition.x = x;
			ScreenPosition.y = y;
			if (OnScreen(ScreenPosition))
			{
				pD3D.Functions.DrawRect(x, y, w, h, 0, config->esp.FriendlyHealthBarBackColor, true, true, config->esp.FriendlyHealthBarOutlineWidth, config->esp.FriendlyHealthBarOutlineColor);
				pD3D.Functions.DrawRect(x, y += (h / 100.f) * (100 - entityList[pIndex].getHealth()), w, (h / 100.f) * entityList[pIndex].getHealth(), 0, config->esp.FriendlyHealthBarFrontColor, true, true, config->esp.FriendlyHealthBarOutlineWidth, config->esp.FriendlyHealthBarOutlineColor);
			}
		}
	}
}

void tESP::tESPFunctions::DrawInfoText(int pIndex)
{
	pESP.InfoText.clear();
	if (localEntity.getTeamNum() != entityList[pIndex].getTeamNum())
	{
		pESP.InfoTextColor = config->esp.EnemyInfoTextColor;
		pESP.InfoTextOutlineColor = config->esp.EnemyInfoTextOutlineColor;
		if (config->esp.DrawEnemyNameText)
		{
			pESP.InfoText.append(L"Name: ");
			pESP.InfoText.append(entityList[pIndex].getPlayerName());
		}
		if (config->esp.DrawEnemyHealthText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Health: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getHealth()));
		}
		if (config->esp.DrawEnemyKillsText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Kills: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getKillsNum()));
		}
		if (config->esp.DrawEnemyDeathsText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Deaths: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getDeathsNum()));
		}
		if (config->esp.DrawEnemyKDRatioText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"K/D Ratio: ");
			float KDRatio = 0;
			if (entityList[pIndex].getKillsNum() != 0)
			{
				if (entityList[pIndex].getDeathsNum() != 0)
				{
					KDRatio = (float)entityList[pIndex].getKillsNum() / (float)entityList[pIndex].getDeathsNum();
				}
				else
				{
					KDRatio = (float)entityList[pIndex].getKillsNum();
				}
			}
			pESP.InfoText.append(std::to_wstring(KDRatio).substr(0, std::to_wstring(KDRatio).find(L".") + 3));
		}
		/*
		if (config->esp.DrawEnemyRankText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Rank: ");
			pESP.InfoText.append(std::to_wstring(pESP.GeneralPlayerEntityInfo.CompetetiveRankNumber));
		}
		if (config->esp.DrawEnemyBombCarrierText)
		{
			pESP.InfoText.append(L"\n");
			if (pESP.GeneralPlayerEntityInfo.HasC4 - 1 == pIndex)
			{
				pESP.InfoText.append(L"C4\n");
			}
		}
		if (config->esp.DrawEnemyDefuseKitCarrierText)
		{
			if (pESP.GeneralPlayerEntityInfo.HasDefuser)
			{
				pESP.InfoText.append(L"Defuse Kit");
			}
		}
		*/
	}
	else
	{
		pESP.InfoTextColor = config->esp.FriendlyInfoTextColor;
		pESP.InfoTextOutlineColor = config->esp.FriendlyInfoTextOutlineColor;
		if (config->esp.DrawFriendlyNameText)
		{
			pESP.InfoText.append(L"Name: ");
			pESP.InfoText.append(entityList[pIndex].getPlayerName());
		}
		if (config->esp.DrawFriendlyHealthText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Health: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getHealth()));
		}
		if (config->esp.DrawFriendlyKillsText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Kills: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getKillsNum()));
		}
		if (config->esp.DrawFriendlyDeathsText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Deaths: ");
			pESP.InfoText.append(std::to_wstring(entityList[pIndex].getDeathsNum()));
		}
		if (config->esp.DrawFriendlyKDRatioText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"K/D Ratio: ");
			float KDRatio = 0.f;
			if (entityList[pIndex].getKillsNum() != 0)
			{
				if (entityList[pIndex].getDeathsNum() != 0)
				{
					KDRatio = (float)entityList[pIndex].getKillsNum() / (float)entityList[pIndex].getDeathsNum();
				}
				else
				{
					KDRatio = (float)entityList[pIndex].getKillsNum();
				}
			}
			pESP.InfoText.append(std::to_wstring(KDRatio).substr(0, std::to_wstring(KDRatio).find(L".") + 3));
		}
		/*
		if (config->esp.DrawFriendlyRankText)
		{
			pESP.InfoText.append(L"\n");
			pESP.InfoText.append(L"Rank: ");
			pESP.InfoText.append(std::to_wstring(pESP.GeneralPlayerEntityInfo.CompetetiveRankNumber));
		}
		if (config->esp.DrawFriendlyBombCarrierText)
		{
			pESP.InfoText.append(L"\n");
			if (pESP.GeneralPlayerEntityInfo.HasC4 - 1 == pIndex)
			{
				pESP.InfoText.append(L"C4\n");
			}
		}
		if (config->esp.DrawEnemyDefuseKitCarrierText)
		{
			if (pESP.GeneralPlayerEntityInfo.HasDefuser)
			{
				pESP.InfoText.append(L"Defuse Kit");
			}
		}
		*/
	}
	if (!pESP.InfoText.empty())
	{
		D3DXVECTOR2 BottomPosition = pESP.Functions.WorldToScreen(entityList[pIndex].getOrigin(), game->getViewMatrix());
		Vector OriginMinusHeight = entityList[pIndex].getOrigin();
		OriginMinusHeight.z -= 5.f;
		D3DXVECTOR2 TextPosition = pESP.Functions.WorldToScreen(OriginMinusHeight, game->getViewMatrix());
		float x, y;
		y = TextPosition.y;
		x = TextPosition.x;
		D3DXVECTOR2 ScreenPosition;
		ScreenPosition.x = x;
		ScreenPosition.y = y;
		if (OnScreen(ScreenPosition))
		{
			pD3D.Functions.DrawString(x, y, pESP.InfoText, pESP.InfoTextColor, true, pESP.InfoTextOutlineColor, true);
		}
	}
}
/*
void tESP::tESPFunctions::DrawTargetBoneMarker(int pIndex)
{
	if (localEntity.getTeamNum() != entityList[pIndex].getTeamNum())
	{
		if (config->esp.DrawEnemyTargetBoneMarker && pD3D.Functions.Get3dDistance(pGeneralPlayerEntity.Functions.getBonePosition(&pESP.GeneralPlayerEntityInfo.BoneMatrix, pGlobalVars.AimbotSettings.TargetBone), pESP.GeneralPlayerEntityInfo.Origin) < 100.f)
		{
			D3DXVECTOR2 ScreenPosition = pESP.Functions.WorldToScreen(pGeneralPlayerEntity.Functions.getBonePosition(&pESP.GeneralPlayerEntityInfo.BoneMatrix, pGlobalVars.AimbotSettings.TargetBone), pESP.LocalPlayerEntityInfo.ViewMatrix);
			if (OnScreen(ScreenPosition))
			{
				if (config->esp.EnemyTargetBoneMarkerType == 1)
				{
					pD3D.Functions.DrawCircle(ScreenPosition.x, ScreenPosition.y, config->esp.EnemyTargetBoneMarkerSize, config->esp.EnemyTargetBoneMarkerLineWidth, config->esp.EnemyTargetBoneMarkerColor, true, config->esp.EnemyTargetBoneMarkerOutlineWidth, config->esp.EnemyTargetBoneMarkerOutlineColor);
				}
				if (config->esp.EnemyTargetBoneMarkerType == 2)
				{
					pD3D.Functions.DrawCross(ScreenPosition.x, ScreenPosition.y, config->esp.EnemyTargetBoneMarkerSize, config->esp.EnemyTargetBoneMarkerLineWidth, config->esp.EnemyTargetBoneMarkerColor, true, config->esp.EnemyTargetBoneMarkerOutlineWidth, config->esp.EnemyTargetBoneMarkerOutlineColor);
				}
			}
		}
	}
	else if (localEntity.getTeamNum() == entityList[pIndex].getTeamNum())
	{
		if (config->esp.DrawFriendlyTargetBoneMarker && pD3D.Functions.Get3dDistance(pGeneralPlayerEntity.Functions.getBonePosition(&pESP.GeneralPlayerEntityInfo.BoneMatrix, pGlobalVars.AimbotSettings.TargetBone), pESP.GeneralPlayerEntityInfo.Origin) < 100.f)
		{
			D3DXVECTOR2 ScreenPosition = pESP.Functions.WorldToScreen(pGeneralPlayerEntity.Functions.getBonePosition(&pESP.GeneralPlayerEntityInfo.BoneMatrix, pGlobalVars.AimbotSettings.TargetBone), pESP.LocalPlayerEntityInfo.ViewMatrix);
			if (OnScreen(ScreenPosition))
			{
				if (config->esp.FriendlyTargetBoneMarkerType == 1)
				{
					pD3D.Functions.DrawCircle(ScreenPosition.x, ScreenPosition.y, config->esp.FriendlyTargetBoneMarkerSize, config->esp.FriendlyTargetBoneMarkerLineWidth, config->esp.FriendlyTargetBoneMarkerColor, true, config->esp.FriendlyTargetBoneMarkerOutlineWidth, config->esp.FriendlyTargetBoneMarkerOutlineColor);
				}
				if (config->esp.FriendlyTargetBoneMarkerType == 2)
				{
					pD3D.Functions.DrawCross(ScreenPosition.x, ScreenPosition.y, config->esp.FriendlyTargetBoneMarkerSize, config->esp.FriendlyTargetBoneMarkerLineWidth, config->esp.FriendlyTargetBoneMarkerColor, true, config->esp.FriendlyTargetBoneMarkerOutlineWidth, config->esp.FriendlyTargetBoneMarkerOutlineColor);
				}
			}
		}
	}
}
*/
void tESP::tESPFunctions::DrawRecoilMarker()
{
	if (config->esp.DrawRecoilMarker)
	{
		float CenterX = pOverlayWindow.Width / 2.f;
		float CenterY = pOverlayWindow.Height / 2.f;
		float HeightPixelsPerAngle = pOverlayWindow.Height / 178.f;
		float WidthPixelsPerAngle = pOverlayWindow.Width / 358.f;
		Vector PunchAngles = localEntity.getPunchAngles();
		CenterX -= (HeightPixelsPerAngle * (PunchAngles.y * 2.f));
		CenterY += (WidthPixelsPerAngle * (PunchAngles.x * 2.f));
		if (config->esp.RecoilMarkerType == 1)
		{
			pD3D.Functions.DrawCircle(CenterX, CenterY, config->esp.RecoilMarkerSize, config->esp.RecoilMarkerLineWidth, config->esp.RecoilMarkerColor, true, config->esp.RecoilMarkerOutlineWidth, config->esp.RecoilMarkerOutlineColor);
		}
		if (config->esp.RecoilMarkerType == 2)
		{
			pD3D.Functions.DrawCross(CenterX, CenterY, config->esp.RecoilMarkerSize, config->esp.RecoilMarkerLineWidth, config->esp.RecoilMarkerColor, true, config->esp.RecoilMarkerOutlineWidth, config->esp.RecoilMarkerOutlineColor);
		}
		pD3D.Functions.DrawLine(pOverlayWindow.Width / 2.f, pOverlayWindow.Height, CenterX, CenterY, 1.f, D3DCOLOR_ARGB(255, 255, 0, 0), true, 1.f, D3DCOLOR_ARGB(255, 255, 0, 0));
	}
}

void tESP::tESPFunctions::Tick() {
	if (config->esp.enabled) {
		pD3D.Functions.BeginRender();
		if (!config->general.panic_mode && Helper::getInstance().isCSGOWindowActive() && game->isInGame() /*&& game->isTeamSelected() && localEntity.isValid()*/) {
			for (int pIndex = 0; pIndex < game->getMaxClients(); pIndex++) {
				if (entityList[pIndex].isValid() && localEntity.getIndex() != pIndex) {
					//DrawBox(pIndex);
					//DrawHealthBar(pIndex);
					//DrawSnapLines(pIndex);
					//DrawInfoText(pIndex);
					//DrawTargetBoneMarker(pIndex);
					/*
					for (int boneIndex = 0; boneIndex < 100; boneIndex++) {
						Vector bonePosition = entityList[pIndex].getBonePosition(boneIndex);
						std::wstring text = std::to_wstring(boneIndex);
						D3DXVECTOR2 ScreenPosition = pESP.Functions.WorldToScreen(bonePosition, game->getViewMatrix());
						if (OnScreen(ScreenPosition)) {
							pD3D.Functions.DrawString(ScreenPosition.x, ScreenPosition.y, text, D3DCOLOR_ARGB(255, 255, 0, 0), false, D3DCOLOR_ARGB(255, 0, 0, 0), false);
						}
					}
					*/
				}
				//DrawRecoilMarker();
			}
		}
		/*
		*/
		Vector ViewAngles, PunchAngles, eyePosition, eyeAngles, origin;
		game->getViewAngles(ViewAngles);
		PunchAngles = localEntity.getPunchAngles();

		eyePosition = localEntity.getEyePosition();
		eyeAngles = entityList[2].getBonePosition(0);

		origin = localEntity.getOrigin();

		std::wstring viewAnglesString = L"ViewAngles " + std::to_wstring(ViewAngles[0]) + L" " + std::to_wstring(ViewAngles[1]) + L" " + std::to_wstring(ViewAngles[2]);
		std::wstring punchAnglesString = L"PunchAngles " + std::to_wstring(PunchAngles[0]) + L" " + std::to_wstring(PunchAngles[1]) + L" " + std::to_wstring(PunchAngles[2]);
		std::wstring eyePositionString = L"EyePosition " + std::to_wstring(eyePosition[0]) + L" " + std::to_wstring(eyePosition[1]) + L" " + std::to_wstring(eyePosition[2]);
		std::wstring eyeAnglesString = L"BonePosition(0) " + std::to_wstring(eyeAngles[0]) + L" " + std::to_wstring(eyeAngles[1]) + L" " + std::to_wstring(eyeAngles[2]);
		std::wstring originString = L"Origin " + std::to_wstring(origin[0]) + L" " + std::to_wstring(origin[1]) + L" " + std::to_wstring(origin[2]);
		std::wstring weaponString = L"WeaponId " + std::to_wstring(localEntity.getActiveWeapon());

		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 10, viewAnglesString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 24, punchAnglesString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 38, eyePositionString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 52, eyeAnglesString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 66, originString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		pD3D.Functions.DrawString(pOverlayWindow.Width / 2.f, 80, weaponString, D3DCOLOR_ARGB(255, 255, 255, 255), false, D3DCOLOR_ARGB(120, 0, 0, 0), true);
		
		pD3D.Functions.EndRender();
	}
}