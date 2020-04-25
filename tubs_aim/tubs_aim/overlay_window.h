#pragma once

struct tOverlayWindow
{
	HWND Handle;
	MSG Message;
	WNDCLASSEX Class;
	RECT Bounds;
	float Width;
	float Height;
	float X;
	float Y;
	struct tOverlayWindowFunctions
	{
		static LRESULT CALLBACK WndProc(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam);
		int GetRandomInt();
		bool Setup();
		void Refresh();
		void Destruct();
	} Functions;
};

extern tOverlayWindow pOverlayWindow;
