#include <Windows.h>
#define _USE_MATH_DEFINES 
#include<cmath>

#include "resource.h"

#define WM_UPDATE_SPRITE (WM_USER + 1)
#define WM_LOAD_DEFAULT_SPRITE (WM_USER + 2)

constexpr auto WINDOW_NAME = "Lab_1";

ATOM RegisterWindowClass(HINSTANCE);
BOOL InitWindowInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool PostLoadDefaultSpriteMessage(HWND hWnd, HINSTANCE hInstance)
{
    return PostMessage(hWnd, WM_LOAD_DEFAULT_SPRITE, NULL, (LPARAM)hInstance);
}

bool PostUpdateSpriteMessage(HWND hWnd)
{
    return PostMessage(hWnd, WM_UPDATE_SPRITE, NULL, NULL);
}

SIZE GetSpriteSize(HBITMAP hBitmap)
{
    BITMAP sprite;
    GetObject(hBitmap, sizeof(BITMAP), &sprite);

    SIZE spriteSize;
    spriteSize.cx = sprite.bmWidth;
    spriteSize.cy = sprite.bmHeight;

    return spriteSize;
}

XFORM GetMovementXform(COORD coordinates)
{
    XFORM xForm;
    xForm.eM11 = 1;
    xForm.eM12 = 0;
    xForm.eM21 = 0;
    xForm.eM22 = 1;
    xForm.eDx = coordinates.X;
    xForm.eDy = coordinates.Y;

    return xForm;
}

XFORM GetRotationXform(short degreeAngle)
{
    XFORM xForm;
    FLOAT radAngle = (FLOAT)(M_PI * degreeAngle / 180);
    FLOAT angleSin = sin(radAngle);
    FLOAT angleCos = cos(radAngle);

    xForm.eM11 = angleCos;
    xForm.eM12 = angleSin;
    xForm.eM21 = -angleSin;
    xForm.eM22 = angleCos;
    xForm.eDx = 0;
    xForm.eDy = 0;

    return xForm;
}

bool PutSpriteOnWindow(HWND hWnd, HBITMAP sprite, COORD coordinates, short angle)
{
    HDC windowDC = GetDC(hWnd);
    HDC spriteDC = CreateCompatibleDC(windowDC);

    HGDIOBJ oldObject = SelectObject(spriteDC, sprite);
    SIZE bitmapSize = GetSpriteSize(sprite);

    XFORM xForm;
    int graphicsMode = SetGraphicsMode(windowDC, GM_ADVANCED);

    xForm = GetMovementXform(coordinates);
    SetWorldTransform(windowDC, &xForm);

    COORD transformationCenter;
    transformationCenter.X = (SHORT)(-(coordinates.X + bitmapSize.cx / 2));
    transformationCenter.Y = (SHORT)(-(coordinates.Y + bitmapSize.cy / 2));
    xForm = GetMovementXform(transformationCenter);
    ModifyWorldTransform(windowDC, &xForm, MWT_RIGHTMULTIPLY);

    xForm = GetRotationXform(angle);
    ModifyWorldTransform(windowDC, &xForm, MWT_RIGHTMULTIPLY);

    transformationCenter.X = -transformationCenter.X;
    transformationCenter.Y = -transformationCenter.Y;
    xForm = GetMovementXform(transformationCenter);
    ModifyWorldTransform(windowDC, &xForm, MWT_RIGHTMULTIPLY);

    bool result = BitBlt(windowDC, 0, 0, bitmapSize.cx, bitmapSize.cy, spriteDC, 0, 0, SRCCOPY);
    ModifyWorldTransform(windowDC, NULL, MWT_IDENTITY);
    SetGraphicsMode(windowDC, graphicsMode);

    SelectObject(spriteDC, oldObject);
    DeleteDC(spriteDC);
    ReleaseDC(hWnd, windowDC);

    return result;
}

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

    // Load default sprite.
    PostLoadDefaultSpriteMessage(hWnd, hInstance);

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP sprite = NULL;
    static COORD spritePosition = { 0, 0 };
    static short angle = 0;

    switch (message)
    {
    case WM_LOAD_DEFAULT_SPRITE:
        if ((sprite = LoadBitmap((HINSTANCE)lParam, MAKEINTRESOURCE(IDB_SPRITE))) != NULL)
        {
            PostUpdateSpriteMessage(hWnd);
        }
        break;
    case WM_UPDATE_SPRITE:
        PutSpriteOnWindow(hWnd, sprite, spritePosition, angle);
        break;
    case WM_SIZE:
        PostUpdateSpriteMessage(hWnd);
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
