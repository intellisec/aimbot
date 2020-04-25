#pragma once

struct tESP
{
	std::wstring InfoText;
	D3DCOLOR InfoTextColor;
	D3DCOLOR InfoTextOutlineColor;
	struct tDataStructs
	{
		struct tPlayerSkeleton
		{
			struct
			{
				D3DXVECTOR2 ScreenPosition[4];
			} Limbs[4];

			struct
			{
				D3DXVECTOR2 ScreenPosition[7];
			} Spine;
			bool Valid;
		};
	};
	struct tESPFunctions
	{
		D3DXVECTOR2 WorldToScreen(Vector EnemyPosition, Matrix4x4 ViewMatrix);
		tESP::tDataStructs::tPlayerSkeleton ParsePlayerSkeleton();
		void DrawBox(int pIndex);
		void DrawSnapLines(int pIndex);
		void DrawBones(int pIndex);
		void DrawHealthBar(int pIndex);
		void DrawInfoText(int pIndex);
		void DrawTargetBoneMarker(int pIndex);
		void DrawRecoilMarker();
		void DrawRecoilMarkerLines();
		bool OnScreen(D3DXVECTOR2 ScreenPosition);
		void Tick();
	} Functions;
};

extern tESP pESP;