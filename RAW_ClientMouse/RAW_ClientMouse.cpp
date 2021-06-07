#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#include <dwmapi.h>
#include <ctime>
#include <ratio>
#include <chrono>
#include <shellapi.h>

// Only for redirecting stdout to the console.
#include <cstdio>
#include <iostream>
#include "Mouse.h"
// Structure to store out input data.
// Not necessary, I just find it neat.
struct {
	struct {
		// Keep in mind these are all deltas,
		// they'll change for one frame/cycle
		// before going back to zero.
		long x = 0;
		long y = 0;
		long wheel = 0;
	} mouse;
} input;
long oldX = 0, oldY = 0;
// Window message callback.
LRESULT CALLBACK EventHandler(HWND, unsigned, WPARAM, LPARAM);

// Make sure to set the entrypoint to "mainCRTStartup", or change this to WinMain.
int main() {
	std::string portString;
	std::cin >> portString;
	SerialPort* port = new SerialPort(portString);
	Mouse arduino = Mouse(port);
	// Why even bother with WinMain?
	HINSTANCE instance = GetModuleHandle(0);
	// Make std::cout faster:
	std::ios_base::sync_with_stdio(false);


	//if (AllocConsole()) {
	//	freopen_s(&console_output, "CONOUT$", "w", stdout);
	//	freopen_s(&console_error, "CONERR$", "w", stderr);
	//}
	//else {
	//	return GetLastError();
	//}

	// Create message-only window:
	const wchar_t* class_name = L"SimpleEngine Class";

	// "{}" is necessary, otherwise we have to use ZeroMemory() (which is just memset).
	WNDCLASSEX window_class = {};
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpfnWndProc = EventHandler;
	window_class.hInstance = GetModuleHandle(0);
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	window_class.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	window_class.lpszClassName = L"CurserBlock";

	if (!RegisterClassEx(&window_class))
		return -1;
	int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	HWND window = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, window_class.lpszClassName, L"CurserBlock", WS_POPUP | WS_VISIBLE, 0, 0, ScreenWidth, ScreenHeight, NULL, NULL, window_class.hInstance, NULL);
	SetWindowLong(window, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	const MARGINS margins = { -1 };
	DwmExtendFrameIntoClientArea(window, &margins);

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);
	if (window == nullptr)
		return -1;
	// End of creating window.

	// Registering raw input devices
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((unsigned short) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((unsigned short) 0x02)
#endif

// We're configuring just one RAWINPUTDEVICE, the mouse,
// so it's a single-element array (a pointer).
	RAWINPUTDEVICE rid[1];
	rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
	rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
	rid[0].dwFlags = RIDEV_INPUTSINK;
	rid[0].hwndTarget = window;
	RegisterRawInputDevices(rid, 1, sizeof(rid[0]));
	// End of resgistering.

	// Main loop:
	MSG event;
	bool quit = false;

	RECT rect;
	GetWindowRect(window, &rect);
	ClipCursor(&rect);

	UpdateWindow(window);
	SetForegroundWindow(window);
	long style = GetWindowLong(window, GWL_EXSTYLE);
	style &= ~WS_EX_LAYERED;
	SetWindowLong(window, GWL_EXSTYLE, style);
	SetForegroundWindow(window);
	//windowstate = 1;
	std::ios::sync_with_stdio(false);
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	while (!quit) {
		while (PeekMessage(&event, 0, 0, 0, PM_REMOVE)) {
			if (event.message == WM_QUIT) {
				quit = true;
				break;
			}

			// Does some Windows magic and sends the message to EventHandler()
			// because it's associated with the window we created.
			TranslateMessage(&event);
			DispatchMessage(&event);
		}

		// Output mouse input to console:
		int a = 0, b = 0, c = 0, d = 0, e = 0;
		if (GetAsyncKeyState(VK_LBUTTON))
		{
			a = 1;
		}
		if (GetAsyncKeyState(VK_RBUTTON))
		{
			b = 1;
		}
		if (GetAsyncKeyState(VK_MBUTTON))
		{
			c = 1;
		}
		if (GetAsyncKeyState(VK_XBUTTON1))
		{
			d = 1;
		}
		if (GetAsyncKeyState(VK_XBUTTON2))
		{
			e = 1;
		}

		if (input.mouse.x > 127)
			oldX = input.mouse.x - 127;
		else if (input.mouse.x < -127)
			oldX = input.mouse.x + 127;
		else {
			input.mouse.x += oldX;
			if (input.mouse.x > 127)
				oldX = input.mouse.x - 127;
			else if (input.mouse.x < -127)
				oldX = input.mouse.x + 127;
		}

		if (input.mouse.y > 127)
			oldY = input.mouse.y - 127;
		else if (input.mouse.y < -127)
			oldY = input.mouse.y + 127;
		else {
			input.mouse.y += oldY;
			if (input.mouse.y > 127)
				oldY = input.mouse.y - 127;
			else if (input.mouse.y < -127)
				oldY = input.mouse.y + 127;
		}

		arduino.Send(input.mouse.x, input.mouse.y, a, b, c, input.mouse.wheel, d, e);

		
		while (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1) < (std::chrono::microseconds)2100)
		{
			//std::cout << "Wait\n";
		}
		t1 = std::chrono::high_resolution_clock::now();
		// Reset mouse data in case WM_INPUT isn't called:
		input.mouse.x = 0;
		input.mouse.y = 0;
		input.mouse.wheel = 0;
		HWND DESKTOP = GetForegroundWindow();
		HWND MOVERDESK = GetWindow(DESKTOP, GW_HWNDPREV);
		SetWindowPos(window, MOVERDESK, NULL, NULL, NULL, NULL, SWP_NOMOVE | SWP_NOSIZE);
		UpdateWindow(window);
	}

	/*fclose(console_output);
	fclose(console_error);*/
	return 0;
}

LRESULT CALLBACK EventHandler(
	HWND hwnd,
	unsigned event,
	WPARAM wparam,
	LPARAM lparam
) {
	switch (event) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_INPUT: {
		// The official Microsoft examples are pretty terrible about this.
		// Size needs to be non-constant because GetRawInputData() can return the
		// size necessary for the RAWINPUT data, which is a weird feature.
		unsigned size = sizeof(RAWINPUT);
		static RAWINPUT raw[sizeof(RAWINPUT)];
		GetRawInputData((HRAWINPUT)lparam, RID_INPUT, raw, &size, sizeof(RAWINPUTHEADER));

		if (raw->header.dwType == RIM_TYPEMOUSE) {
			input.mouse.x = raw->data.mouse.lLastX;
			input.mouse.y = raw->data.mouse.lLastY;

			// Wheel data needs to be pointer casted to interpret an unsigned short as a short, with no conversion
			// otherwise it'll overflow when going negative.
			// Didn't happen before some minor changes in the code, doesn't seem to go away
			// so it's going to have to be like this.
			if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
				input.mouse.wheel = (*(short*)&raw->data.mouse.usButtonData) / WHEEL_DELTA;
		}
	} return 0;
	}

	// Run default message processor for any missed events:
	return DefWindowProc(hwnd, event, wparam, lparam);
}