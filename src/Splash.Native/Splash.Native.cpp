// Splash.Native.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "Splash.Native.h"


const wchar_t CLASS_NAME[] = L"SplashWindow";

static HINSTANCE initialized = nullptr;

typedef struct {
	HINSTANCE hInstance;
	HBITMAP bitmap;
	int width;
	int height;

} ThreadState;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	ThreadState* state = (ThreadState*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBITMAP bmp = state->bitmap;

		BITMAP          bitmap;
		HDC             hdcMem;
		HGDIOBJ         oldBitmap;

		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, bmp);

		GetObject(bmp, sizeof(bitmap), &bitmap);
		//BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCINVERT);

		SetBkMode(hdc, TRANSPARENT);
		BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);


		// We are going to paint the two DDB's in sequence to the destination.
		// 1st the monochrome bitmap will be blitted using an AND operation to
		// cut a hole in the destination. The color image will then be ORed
		// with the destination, filling it into the hole, but leaving the
		// surrounding area untouched.



		EndPaint(hWnd, &ps);
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

static void init(HINSTANCE hInstance)
{
	if (initialized == hInstance) return;
	initialized = hInstance;

	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW | WS_EX_TRANSPARENT;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.lpszClassName = CLASS_NAME;
	wcex.hbrBackground = (HBRUSH)0;
	//wcex.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));

	RegisterClassExW(&wcex);
}
// Get the horizontal and vertical screen sizes in pixel
static void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}


static DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	ThreadState* state = (ThreadState*)lpParam;
	HINSTANCE hInstance = state->hInstance;

	auto w = state->width;
	auto h = state->height;
	int sw, sh;
	GetDesktopResolution(sw, sh);
	auto x = (sw - w) / 2;
	auto y = (sh - h) / 2;

	HWND hwnd = CreateWindowW(CLASS_NAME, nullptr, 0, x, y, w, h, nullptr, nullptr, hInstance, nullptr);
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
	SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)state);
	SetLayeredWindowAttributes(hwnd, RGB(255, 0, 255), 255, LWA_COLORKEY);
	SetWindowPos(hwnd, HWND_TOPMOST, x, y, w, h, SWP_FRAMECHANGED);
	
	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		//printf("msg: %x\n", msg.message);
		if (msg.message == WM_CLOSE) break;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	CloseWindow(hwnd);
	DestroyWindow(hwnd);
	DeleteObject(state->bitmap);
	delete state;

	return 0;
}

DllExport(DWORD) ShowSplash(int w, int h, int bits, const void* data)
{
	auto hInstance = (HINSTANCE)GetModuleHandle(NULL);
	init(hInstance);

	auto bmp = CreateBitmap(w, h, 1, bits, data);
	
	//auto bmp = (HBITMAP)LoadImage(hInstance, L"C:\\Users\\Schorsch\\Desktop\\test.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

	//BITMAP bm;
	//GetObject(bmp, sizeof(BITMAP), &bm);
	//auto w = bm.bmWidth;
	//auto h = bm.bmHeight;

	auto state = new ThreadState();
	state->hInstance = hInstance;
	state->bitmap = bmp;
	state->width = w;
	state->height = h;

	DWORD threadId;
	HANDLE thread = CreateThread(nullptr, 0, MyThreadFunction, state, 0, &threadId);

	return threadId;
}
DllExport(void) CloseSplash(DWORD threadId)
{	
	if (!threadId) return;
	PostThreadMessage(threadId, WM_CLOSE, 0, 0);
	//printf("worked: %d %d\n", threadId, worked);
}
