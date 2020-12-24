﻿#include <Windows.h>
#include "resource.h"

#pragma comment(lib, "msimg32.lib")

#define WM_UPDATE_SPRITE (WM_USER)
#define WM_LOAD_DEFAULT_SPRITE (WM_USER + 1)
#define BACKGROUND_COLOR COLOR_WINDOW + 21

constexpr auto WINDOW_NAME = "Lab_1";
constexpr auto SPRITE_STEP = 10;

constexpr auto VK_W = 0x57;
constexpr auto VK_A = 0x41;
constexpr auto VK_S = 0x53;
constexpr auto VK_D = 0x44;

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

SIZE GetClientWindowSize(HWND hWnd)
{
    RECT windowRectangle;
    GetClientRect(hWnd, &windowRectangle);

    SIZE windowSize;
    windowSize.cx = windowRectangle.right - windowRectangle.left;
    windowSize.cy = windowRectangle.bottom - windowRectangle.top;

    return windowSize;
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

int FillWindowWithColor(HWND hWnd, COLORREF color)
{
    RECT windowRectangle;
    GetClientRect(hWnd, &windowRectangle);

    HDC windowDC = GetDC(hWnd);
    HBRUSH hBrush = CreateSolidBrush(color);

    int result = FillRect(windowDC, &windowRectangle, hBrush);

    DeleteObject(hBrush);
    ReleaseDC(hWnd, windowDC);

    return result;
}

COORD CreateNewSpritePosition(COORD spritePosition, COORD spriteSteps, HWND hWnd, HBITMAP sprite)
{
    SIZE windowSize = GetClientWindowSize(hWnd);
    SIZE spriteSize = GetSpriteSize(sprite);

    COORD newSpritePosition;

    newSpritePosition.X = spritePosition.X + spriteSteps.X;
    if (newSpritePosition.X < 0)
    {
        newSpritePosition.X = 0 + SPRITE_STEP;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)
    {
        newSpritePosition.X = (SHORT)(windowSize.cx - spriteSize.cx - SPRITE_STEP);
    }

    newSpritePosition.Y = spritePosition.Y + spriteSteps.Y;
    if (newSpritePosition.Y < 0)
    {
        newSpritePosition.Y = 0 + SPRITE_STEP;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)
    {
        newSpritePosition.Y = (SHORT)(windowSize.cy - spriteSize.cy - SPRITE_STEP);
    }

    return newSpritePosition;
}

COORD CreateUpSteps()
{
    COORD steps;
    steps.X = 0;
    steps.Y = -SPRITE_STEP;

    return steps;
}

COORD CreateRightSteps()
{
    COORD steps;
    steps.X = SPRITE_STEP;
    steps.Y = 0;

    return steps;
}

COORD CreateDownSteps()
{
    COORD steps;
    steps.X = 0;
    steps.Y = SPRITE_STEP;

    return steps;
}

COORD CreateLeftSteps()
{
    COORD steps;
    steps.X = -SPRITE_STEP;
    steps.Y = 0;

    return steps;
}

bool PutSpriteOnWindow(HWND hWnd, HBITMAP sprite, COORD coordinates)
{
    HDC windowDC = GetDC(hWnd);
    HDC spriteDC = CreateCompatibleDC(windowDC);

    HGDIOBJ oldObject = SelectObject(spriteDC, sprite);
    SIZE bitmapSize = GetSpriteSize(sprite);

    bool result = TransparentBlt(windowDC, // дескриптор приемного DC
        coordinates.X,                     // x-коорд. верхнего левого угла приемника
        coordinates.Y,                     // y-коорд. верхнего левого угла приемника
        bitmapSize.cx,                     // ширина приемного прямоугольника
        bitmapSize.cy,                     // высота приемного прямоугольника
        spriteDC,                          // дескриптор источника DC
        0,                                 // x-коорд. верхнего левого угла источника
        0,                                 // y-коорд. верхнего левого угла источника
        bitmapSize.cx,                     // ширина источника прямоугольника
        bitmapSize.cy,                     // высота источника прямоугольника
        RGB(255, 255, 255));               // цвет который делается прозрачным
   
    DeleteDC(spriteDC);
    ReleaseDC(hWnd, windowDC);

    return result;
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR cmdLine, _In_ int showMode)
{
    MSG msg;

    RegisterWindowClass(hInstance);

    if (!InitWindowInstance(hInstance, showMode))
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
    windowClassEx.hbrBackground = (HBRUSH)(BACKGROUND_COLOR);
    windowClassEx.lpszMenuName = 0;
    windowClassEx.lpszClassName = WINDOW_NAME;
    windowClassEx.hIconSm = 0;

    return RegisterClassEx(&windowClassEx);
}

BOOL InitWindowInstance(HINSTANCE hInstance, int showMode)
{
    HWND hWnd;

    hWnd = CreateWindow(WINDOW_NAME,  // Указатель на зарегистрированное имя класса.
        WINDOW_NAME,                  // Указатель на имя окна.
        WS_OVERLAPPEDWINDOW,          // Стиль окна.
        CW_USEDEFAULT,                // Горизонтальная позиция окна.
        CW_USEDEFAULT,                // Вертикальная позиция окна.
        CW_USEDEFAULT,                // Ширина окна.
        CW_USEDEFAULT,                // Высота окна.
        NULL,                         // Дескриптор родительского или окна владельца.
        NULL,                         // Дескриптор меню или ID дочернего окна.
        hInstance,                    // Дескриптор экземпляра приложения.
        NULL);                        // Указатель на данные создания окна.

    if (!hWnd)
        return FALSE;

    ShowWindow(hWnd, showMode);

    // Load default sprite.
    PostLoadDefaultSpriteMessage(hWnd, hInstance);
  
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBITMAP sprite = NULL;
    static COORD spritePosition = { 0, 0 };

    switch (message)
    {
    case WM_LOAD_DEFAULT_SPRITE:
        if ((sprite = LoadBitmap((HINSTANCE)lParam, MAKEINTRESOURCE(IDB_SPRITE))) != NULL)
            PostUpdateSpriteMessage(hWnd);
        break;
    case WM_UPDATE_SPRITE:
        FillWindowWithColor(hWnd, GetSysColor(BACKGROUND_COLOR));
        PutSpriteOnWindow(hWnd, sprite, spritePosition);
        break;
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_UP:
        case VK_W:
            spritePosition = CreateNewSpritePosition(spritePosition, CreateUpSteps(), hWnd, sprite);
            PostUpdateSpriteMessage(hWnd);
            break;
        case VK_RIGHT:
        case VK_D:
            spritePosition = CreateNewSpritePosition(spritePosition, CreateRightSteps(), hWnd, sprite);
            PostUpdateSpriteMessage(hWnd);
            break;
        case VK_DOWN:
        case VK_S:
            spritePosition = CreateNewSpritePosition(spritePosition, CreateDownSteps(), hWnd, sprite);
            PostUpdateSpriteMessage(hWnd);
            break;
        case VK_LEFT:
        case VK_A:
            spritePosition = CreateNewSpritePosition(spritePosition, CreateLeftSteps(), hWnd, sprite);
            PostUpdateSpriteMessage(hWnd);
            break;
        default:
            break;
        }
        break;
    case WM_MOUSEWHEEL:
        if (GET_KEYSTATE_WPARAM(wParam) == MK_SHIFT)
        {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                spritePosition = CreateNewSpritePosition(spritePosition, CreateLeftSteps(), hWnd, sprite);
                PostUpdateSpriteMessage(hWnd);
            }
            else
            {
                spritePosition = CreateNewSpritePosition(spritePosition, CreateRightSteps(), hWnd, sprite);
                PostUpdateSpriteMessage(hWnd);
            }
        }
        else
        {
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
            {
                spritePosition = CreateNewSpritePosition(spritePosition, CreateUpSteps(), hWnd, sprite);
                PostUpdateSpriteMessage(hWnd);
            }
            else
            {
                spritePosition = CreateNewSpritePosition(spritePosition, CreateDownSteps(), hWnd, sprite);
                PostUpdateSpriteMessage(hWnd);
            }
        }
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

    return 0;
}