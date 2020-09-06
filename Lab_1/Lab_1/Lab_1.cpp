#include <Windows.h>
#include <tchar.h>
#include <xstring>

#include "resource.h";

typedef std::basic_string<TCHAR, std::char_traits<TCHAR>,
	std::allocator<TCHAR> > String;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

TCHAR WinName[] = _T("Lab_1");
HINSTANCE hInst;

int APIENTRY _tWinMain(HINSTANCE This, HINSTANCE Prev, LPTSTR cmd, int mode) 
{
	HWND hWnd;    // Window descriptor.
	MSG msg;      // Structure for storing the message.
	WNDCLASS wc;  // Window class.

	wc.hInstance = This;                          
	wc.lpszClassName = WinName;                  // Window name.
	wc.lpfnWndProc = WndProc;                    // Callback window function.
	wc.style = CS_HREDRAW | CS_VREDRAW;          // Window style.
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // Default window icon.
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);    // Default mouse cursor.
	wc.lpszMenuName = NULL;                      // Window has no menu.
	wc.cbClsExtra = 0;                           // No extra class info.
	wc.cbWndExtra = 0;                           // No extra window info.

	// Fill window with white color.
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	hInst = This;

	// Class registration
	if (!RegisterClass(&wc))
		return 0;

	hWnd = CreateWindow(WinName,  // Window name.
		_T("Lab_1"),              // Window title.
		WS_OVERLAPPEDWINDOW,      // Window style.
		CW_USEDEFAULT,            // X.
		CW_USEDEFAULT,            // Y.
		CW_USEDEFAULT,            // Window width.
		CW_USEDEFAULT,            // Window height.
		HWND_DESKTOP,             // Parent window descriptor.
		NULL,                     // Window has no menu.
		This,                     // Application descriptor.
		NULL);                    // No extra info.

	ShowWindow(hWnd, mode);

	// Message processing cycle.
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

// The window function is called by the operating system and
// receives messages from the queue for this application.
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	HBITMAP hBitmap;

	static HDC memBitMap;
	static BITMAP bm;

	switch (message)
	{
	case WM_CREATE:
		hBitmap = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BITMAP1));
		GetObject(hBitmap, sizeof(bm), &bm);
		hdc = GetDC(hWnd);
		memBitMap = CreateCompatibleDC(hdc);
		SelectObject(memBitMap, hBitmap);
		ReleaseDC(hWnd, hdc);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, memBitMap, 0, 0, SRCCOPY);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY: 
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}