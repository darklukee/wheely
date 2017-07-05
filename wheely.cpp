#include <windows.h>
#include <cstdint>

bool process(DWORD mouseData)
{
	POINT p;
	if (!GetCursorPos(&p))
		return false;
	HWND win = WindowFromPoint(p);
	if (!win)
		return false;

	const uint16_t pressed{ 0x8000 }; //if the high-order bit is 1, the key is down; otherwise, it is up.
	if ((GetKeyState(VK_LBUTTON) | GetKeyState(VK_RBUTTON) | GetKeyState(VK_MBUTTON)) & pressed)
		return false;

	WPARAM mhwParam = mouseData & 0xFFFF0000; //high-order word of this member is the wheel delta
	if (GetKeyState(VK_CONTROL) & pressed)
	{
		mhwParam |= MK_CONTROL;
	}
	if (GetKeyState(VK_SHIFT) & pressed)
	{
		mhwParam |= MK_SHIFT;
	}

	if (!PostMessage(win, WM_MOUSEWHEEL, mhwParam, static_cast<LPARAM>(p.x | (p.y << 16))))
		return false;

	return true;
}

LRESULT CALLBACK mouseHook(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WM_MOUSEWHEEL && nCode >= 0)
	{
		if (process(reinterpret_cast<MSLLHOOKSTRUCT*>(lParam)->mouseData))
		{
			return 1;
		}
	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
		return -1;

	if (!SetWindowsHookEx(WH_MOUSE_LL, mouseHook, hInstance, 0))
		return -1;

	//MSG msg;
	while (GetMessage(nullptr, nullptr, WM_MOUSEWHEEL, WM_MOUSEWHEEL))
	{
	}

	return 0;
}
