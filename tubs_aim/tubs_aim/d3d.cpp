#include "stdafx.h"
#include "dllmain.h"

float tD3D::tD3DFunctions::Get3dDistance(D3DXVECTOR3 PlayerPosition, D3DXVECTOR3 EnemyPosition)
{
	return sqrt(powf(EnemyPosition.x - PlayerPosition.x, 2.f) + powf(EnemyPosition.y - PlayerPosition.y, 2.f) + powf(EnemyPosition.z - PlayerPosition.z, 2.f));
}

void tD3D::tD3DFunctions::DrawLine(float X, float Y, float XX, float YY, float LineWidth, D3DCOLOR LineColor, bool Outlined, float OutlineWidth, D3DCOLOR OutlineColor)
{
	D3DXVECTOR2 Points[2];
	Points[0] = D3DXVECTOR2(X, Y);
	Points[1] = D3DXVECTOR2(XX, YY);
	if (Outlined)
	{
		pD3D.Line->SetWidth(OutlineWidth * 2.f + LineWidth);
		pD3D.Line->Draw(Points, 2, OutlineColor);
	}
	pD3D.Line->SetWidth(LineWidth);
	pD3D.Line->Draw(Points, 2, LineColor);
}

void tD3D::tD3DFunctions::DrawCircle(float X, float Y, float Radius, float LineWidth, D3DCOLOR LineColor, bool Outlined, float OutlineWidth, D3DCOLOR OutlineColor)
{
	D3DXVECTOR2 Points[50];
	for (int i = 0; i < 50; i++)
	{
		Points[i] = D3DXVECTOR2(static_cast<float>(X + (Radius * cos(i))), static_cast<float>(Y + (Radius * sin(i))));
	}
	if (Outlined)
	{
		pD3D.Line->SetWidth(LineWidth + OutlineWidth * 2);
		pD3D.Line->Draw(Points, 50, OutlineColor);
	}
	pD3D.Line->SetWidth(LineWidth);
	pD3D.Line->Draw(Points, 50, LineColor);
}


void tD3D::tD3DFunctions::DrawRect(float X, float Y, float Width, float Height, float LineWidth, D3DCOLOR LineColor, bool Filled, bool Outlined, float OutlineWidth, D3DCOLOR OutlineColor)
{
	if (Filled)
	{
		if (Outlined)
		{
			DrawLine(X - OutlineWidth, Y + Height / 2, X + Width + OutlineWidth, Y + Height / 2.f, Height, OutlineColor, true, OutlineWidth, OutlineColor);
		}
		DrawLine(X, Y + Height / 2.f, X + Width, Y + Height / 2, Height, LineColor, false, 0, 0);
	}
	else
	{
		if (Outlined)
		{
			OutlineWidth *= 2.f + LineWidth;
			DrawLine(X, Y, X, Y + Height, OutlineWidth, OutlineColor, false, 0, 0);
			DrawLine(X, Y + Height, X + Width, Y + Height, OutlineWidth, OutlineColor, false, 0, 0);
			DrawLine(X + Width, Y + Height, X + Width, Y, OutlineWidth, OutlineColor, false, 0, 0);
			DrawLine(X + Width, Y, X, Y, OutlineWidth, OutlineColor, false, 0, 0);
			DrawLine(X, Y, X, Y + Height, LineWidth, LineColor, false, 0, 0);
			DrawLine(X, Y + Height, X + Width, Y + Height, LineWidth, LineColor, false, 0, 0);
			DrawLine(X + Width, Y + Height, X + Width, Y, LineWidth, LineColor, false, 0, 0);
			DrawLine(X + Width, Y, X, Y, LineWidth, LineColor, false, 0, 0);
		}
		else
		{
			DrawLine(X, Y, X, Y + Height, LineWidth, LineColor, false, 0, 0);
			DrawLine(X, Y + Height, X + Width, Y + Height, LineWidth, LineColor, false, 0, 0);
			DrawLine(X + Width, Y + Height, X + Width, Y, LineWidth, LineColor, false, 0, 0);
			DrawLine(X + Width, Y, X, Y, LineWidth, LineColor, false, 0, 0);
		}
	}
}

void tD3D::tD3DFunctions::DrawString(float X, float Y, std::wstring Text, D3DCOLOR TextColor, bool Outlined, D3DCOLOR OutlineColor, bool Centered)
{
	RECT Rect;
	Rect.top = static_cast<long>(Y);
	Rect.left = static_cast<long>(X);
	Rect.bottom = static_cast<long>(Y);
	Rect.right = static_cast<long>(X);
	if (Outlined)
	{
		RECT OutlineRectTop;
		OutlineRectTop.top = static_cast<long>(Y - 1.f);
		OutlineRectTop.left = static_cast<long>(X);
		OutlineRectTop.bottom = static_cast<long>(Y);
		OutlineRectTop.right = static_cast<long>(X - 1.f);
		RECT OutlineRectLeft;
		OutlineRectLeft.top = static_cast<long>(Y);
		OutlineRectLeft.left = static_cast<long>(X - 1.f);
		OutlineRectLeft.bottom = static_cast<long>(Y);
		OutlineRectLeft.right = static_cast<long>(X - 1.f);
		RECT OutlineRectRight;
		OutlineRectRight.top = static_cast<long>(Y);
		OutlineRectRight.left = static_cast<long>(X + 1.f);
		OutlineRectRight.bottom = static_cast<long>(Y);
		OutlineRectRight.right = static_cast<long>(X + 1.f);
		RECT OutlineRectBottom;
		OutlineRectBottom.top = static_cast<long>(Y + 1.f);
		OutlineRectBottom.left = static_cast<long>(X);
		OutlineRectBottom.bottom = static_cast<long>(Y + 1.f);
		OutlineRectBottom.right = static_cast<long>(X);
		if (Centered)
		{
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectTop, DT_CENTER | DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectLeft, DT_CENTER | DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectRight, DT_CENTER | DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectBottom, DT_CENTER | DT_NOCLIP, OutlineColor);
		}
		else
		{
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectTop, DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectLeft, DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectRight, DT_NOCLIP, OutlineColor);
			pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &OutlineRectBottom, DT_NOCLIP, OutlineColor);
		}
	}
	if (Centered)
	{
		pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &Rect, DT_CENTER | DT_NOCLIP, TextColor);
	}
	else
	{
		pD3D.Font->DrawTextW(NULL, Text.c_str(), wcslen(Text.c_str()), &Rect, DT_NOCLIP, TextColor);
	}
}

void tD3D::tD3DFunctions::DrawCross(float X, float Y, float Size, float LineWidth, D3DCOLOR CrossColor, bool Outlined, float OutlineWidth, D3DCOLOR OutlineColor)
{
	if (Outlined)
	{
		DrawLine(X - Size / 2.f - OutlineWidth, Y, X - 1.f, Y, LineWidth, OutlineColor, true, OutlineWidth, OutlineColor);
		DrawLine(X + Size / 2.f + OutlineWidth, Y, X, Y, LineWidth, OutlineColor, true, OutlineWidth, OutlineColor);
		DrawLine(X, Y - Size / 2.f - OutlineWidth, X, Y - 1.f, LineWidth, OutlineColor, true, OutlineWidth, OutlineColor);
		DrawLine(X, Y + Size / 2.f + OutlineWidth, X, Y, LineWidth, OutlineColor, true, OutlineWidth, OutlineColor);
	}
	DrawLine(X - Size / 2.f, Y, X - 1.f, Y, LineWidth, CrossColor, false, OutlineWidth, OutlineColor);
	DrawLine(X + Size / 2.f, Y, X, Y, LineWidth, CrossColor, false, OutlineWidth, OutlineColor);
	DrawLine(X, Y - Size / 2.f, X, Y - 1.f, LineWidth, CrossColor, false, OutlineWidth, OutlineColor);
	DrawLine(X, Y + Size / 2.f, X, Y, LineWidth, CrossColor, false, OutlineWidth, OutlineColor);
}

void tD3D::tD3DFunctions::BeginRender()
{
	pD3D.Device->Clear(NULL, NULL, D3DCLEAR_TARGET, NULL, 1, NULL);
	pD3D.Device->BeginScene();
}

void tD3D::tD3DFunctions::EndRender()
{
	pD3D.Device->EndScene();
	pD3D.Device->PresentEx(NULL, NULL, NULL, NULL, NULL);
}

bool tD3D::tD3DFunctions::Setup()
{
	Direct3DCreate9Ex(D3D_SDK_VERSION, &pD3D.Interface);
	if (pD3D.Interface)
	{
		ZeroMemory(&pD3D.Params, sizeof(pD3D.Params));
		pD3D.Params.Windowed = TRUE;
		pD3D.Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		pD3D.Params.hDeviceWindow = pOverlayWindow.Handle;
		pD3D.Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
		pD3D.Params.BackBufferFormat = D3DFMT_A8R8G8B8;
		pD3D.Params.BackBufferWidth = static_cast<long>(pOverlayWindow.Width);
		pD3D.Params.BackBufferHeight = static_cast<long>(pOverlayWindow.Height);
		pD3D.Params.EnableAutoDepthStencil = TRUE;
		pD3D.Params.AutoDepthStencilFormat = D3DFMT_D16;
		pD3D.Interface->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, pOverlayWindow.Handle, D3DCREATE_HARDWARE_VERTEXPROCESSING, &pD3D.Params, 0, &pD3D.Device);
		if (pD3D.Device)
		{
			D3DXCreateLine(pD3D.Device, &pD3D.Line);
			if (pD3D.Line)
			{
				D3DXCreateFont(pD3D.Device, 18, NULL, FW_NORMAL, NULL, false, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, "Calibri", &pD3D.Font);
				if (pD3D.Font)
				{
					pD3D.Line->SetAntialias(TRUE);
					return true;
				}
				else
				{
					MessageBoxW(NULL, L"D3DXCreateFont failed!", L"Fatal Error", MB_OK);
				}
			}
			else
			{
				MessageBoxW(NULL, L"D3DXCreateLine failed!", L"Fatal Error", MB_OK);
			}
		}
		else
		{
			MessageBoxW(NULL, L"CreateDeviceEx failed!", L"Fatal Error", MB_OK);
		}
	}
	else
	{
		MessageBoxW(NULL, L"Direct3DCreate9Ex failed!", L"Fatal Error", MB_OK);
	}
	return false;
}

void tD3D::tD3DFunctions::Destruct()
{
	pD3D.Line->Release();
	pD3D.Font->Release();
	pD3D.Device->Release();
	pD3D.Interface->Release();
}