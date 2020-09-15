#include <Windows.h>

constexpr auto WINDOW_NAME = "Lab_1";

ATOM RegisterWindowClass(HINSTANCE);
BOOL InitWindowInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR cmdLine, int cmdShowMode)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	MSG msg;

	RegisterWindowClass(hInstance);

	if (!InitWindowInstance(hInstance, cmdShowMode))
		return FALSE;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
	WNDCLASSEX windowClassEx;

	windowClassEx.cbSize = sizeof(WNDCLASSEX);
	windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
	windowClassEx.lpfnWndProc = WndProc;
	windowClassEx.cbClsExtra = 0;
	windowClassEx.cbWndExtra = 0;
	windowClassEx.hInstance = hInstance;
	windowClassEx.hIcon = LoadIcon(0, IDI_WINLOGO);;
	windowClassEx.hCursor = LoadCursor(0, IDC_ARROW);
	windowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	windowClassEx.lpszMenuName = 0;
	windowClassEx.lpszClassName = WINDOW_NAME;
	windowClassEx.hIconSm = 0;

	return RegisterClassEx(&windowClassEx);
}

BOOL InitWindowInstance(HINSTANCE hInstance, int cmdShowMode)
{
	HWND hWnd;

	hWnd = CreateWindow(WINDOW_NAME, WINDOW_NAME, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, cmdShowMode);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
