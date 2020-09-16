#include <Windows.h>
#include<gdiplus.h>
#include "resource.h"

#define _USE_MATH_DEFINES 
#include<cmath>

#define WM_LOAD_SPRITE WM_USER
#define WM_UPDATE_SPRITE (WM_USER + 1)
#define WM_LOAD_DEFAULT_SPRITE (WM_USER + 2)

#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

#define VK_Q 0x51
#define VK_E 0x45

#define VK_L 0x4c

constexpr auto WINDOW_NAME = "Lab_1";
constexpr auto SPRITE_STEP = 10;
constexpr auto SPRITE_DEGREE_ROTATE_STEP = 15;

ATOM RegisterWindowClass(HINSTANCE);
BOOL InitWindowInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool PostLoadDefaultSpriteMessage(HWND hWnd, HINSTANCE hInstance)
{
    return PostMessage(hWnd, WM_LOAD_DEFAULT_SPRITE, NULL, (LPARAM)hInstance);
}

bool PostLoadSpriteMessage(HWND hWnd)
{
    return PostMessage(hWnd, WM_LOAD_SPRITE, NULL, NULL);
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
        newSpritePosition.X = 0;
    }
    else if (newSpritePosition.X + spriteSize.cx > windowSize.cx)
    {
        newSpritePosition.X = (SHORT)(windowSize.cx - spriteSize.cx);
    }

    newSpritePosition.Y = spritePosition.Y + spriteSteps.Y;
    if (newSpritePosition.Y < 0)
    {
        newSpritePosition.Y = 0;
    }
    else if (newSpritePosition.Y + spriteSize.cy > windowSize.cy)
    {
        newSpritePosition.Y = (SHORT)(windowSize.cy - spriteSize.cy);
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

LRESULT LoadSprite(HWND hWnd, HBITMAP& sprite)
{
    char fileName[MAX_PATH] = { NULL };

    OPENFILENAME openFileName;
    openFileName.lStructSize = sizeof(OPENFILENAME);
    openFileName.hwndOwner = hWnd;
    openFileName.hInstance = NULL;
    openFileName.lpstrFilter = "Images\0*.bmp;*.gif;*.jpeg;*.png;\0\0";
    openFileName.lpstrCustomFilter = NULL;
    openFileName.nFilterIndex = 1;
    openFileName.lpstrFile = fileName;
    openFileName.nMaxFile = sizeof(fileName);
    openFileName.lpstrFileTitle = NULL;
    openFileName.lpstrInitialDir = NULL;
    openFileName.lpstrTitle = "Select sprite";
    openFileName.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    openFileName.lpstrDefExt = NULL;

    if (GetOpenFileName(&openFileName))
    {
        int fileNameLength = MultiByteToWideChar(CP_ACP, 0, fileName, -1, NULL, 0);
        WCHAR* wideCharFileName = new WCHAR[MultiByteToWideChar(CP_ACP, 0, fileName, -1, NULL, 0)];
        MultiByteToWideChar(CP_ACP, 0, fileName, -1, wideCharFileName, fileNameLength);

        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;
        GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

        Gdiplus::Bitmap* sourceImage = Gdiplus::Bitmap::FromFile(wideCharFileName);
        HBITMAP hBitmap;
        Gdiplus::Color imageBackgroundColor;
        imageBackgroundColor.SetFromCOLORREF(GetSysColor(COLOR_WINDOW));
        Gdiplus::Status bitmapStatus = sourceImage->GetHBITMAP(imageBackgroundColor, &hBitmap);

        Gdiplus::GdiplusShutdown(gdiplusToken);

        if (bitmapStatus != Gdiplus::Ok)
            return -1;

        SIZE windowSize = GetClientWindowSize(hWnd), spriteSize = GetSpriteSize(hBitmap);
        if ((windowSize.cx < spriteSize.cx) || (windowSize.cy < spriteSize.cy))
            return -2;

        sprite = hBitmap;

        return 0;
    }

    return 1;
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
    windowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW);
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
            PostUpdateSpriteMessage(hWnd);
        break;
    case WM_LOAD_SPRITE:
        switch (LoadSprite(hWnd, sprite))
        {
        case 0:
            spritePosition = { 0, 0 };
            angle = 0;
            PostUpdateSpriteMessage(hWnd);
            break;
        case -1:
            MessageBox(hWnd, "An unknown error occurred while loading the sprite.",
                "Unknown error", MB_OK | MB_ICONERROR);
            break;
        case -2:
            MessageBox(hWnd, "The sprite does not fit into the window.",
                "Large sprite size", MB_OK | MB_ICONERROR);
            break;
        default:
            break;
        }
        break;
    case WM_UPDATE_SPRITE:
        FillWindowWithColor(hWnd, GetSysColor(COLOR_WINDOW));
        PutSpriteOnWindow(hWnd, sprite, spritePosition, angle);
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
        case VK_Q:
            angle -= SPRITE_DEGREE_ROTATE_STEP;
            PostUpdateSpriteMessage(hWnd);
            break;
        case VK_E:
            angle += SPRITE_DEGREE_ROTATE_STEP;
            PostUpdateSpriteMessage(hWnd);
            break;
        case VK_L:
            PostLoadSpriteMessage(hWnd);
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
