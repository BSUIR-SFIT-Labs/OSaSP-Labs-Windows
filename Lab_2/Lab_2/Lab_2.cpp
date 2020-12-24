#include <Windows.h>
#include <string>
#include <fstream>

using namespace std;

#define BACKGROUND_COLOR COLOR_WINDOW 

constexpr auto WINDOW_NAME = "Lab_2";
constexpr auto PATH_TO_FILE = "D:\\text.txt";
constexpr auto NUM_OF_COLUMNS = 4;
constexpr auto NUM_OF_ROWS = 6;

ATOM RegisterWindowClass(HINSTANCE);
BOOL InitWindowInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

string _listOfData[NUM_OF_COLUMNS * NUM_OF_ROWS];
int _userWindowWidth = 0;
int _userWindowHeight = 0;

void GetDataFromFile() 
{
    int requiredNumOfLines = NUM_OF_COLUMNS * NUM_OF_ROWS;

    ifstream file(PATH_TO_FILE);

    int i = 0;
    while (getline(file, _listOfData[i]) && i < requiredNumOfLines) 
        i++;

    file.close();
}

void DrawTableWithText(HWND hWnd)
{
    RECT rect;
    PAINTSTRUCT paintStruct;
    HDC hdc = BeginPaint(hWnd, &paintStruct);

    int columnWidth = _userWindowWidth / NUM_OF_COLUMNS;
    int tableHeight = 0;

    for (int i = 0; i < NUM_OF_ROWS; i++)
    {
        int currentMaxColumnHeight = 0;

        for (int j = 0; j < NUM_OF_COLUMNS; j++)
        {
            SetTextColor(hdc, RGB(128, 57, 30));

            LPCSTR dataItem = _listOfData[NUM_OF_COLUMNS * i + j].c_str();
            int lengthOfDataItem = strlen(dataItem);

            SetRect(&rect, j * columnWidth + 1, tableHeight + 1, (j + 1) * columnWidth - 1, _userWindowHeight);
            int strHeight = DrawText(hdc, dataItem, lengthOfDataItem, &rect, DT_WORDBREAK);

            if (strHeight > currentMaxColumnHeight)
                currentMaxColumnHeight = strHeight;
           
        }

        tableHeight += currentMaxColumnHeight;

        MoveToEx(hdc, 0, tableHeight, NULL);
        LineTo(hdc, _userWindowWidth, tableHeight);
    }

    for (int i = 1; i < NUM_OF_COLUMNS; i++)
    {
        int columnPosition = i * columnWidth;
        MoveToEx(hdc, columnPosition, 0, NULL);
        LineTo(hdc, columnPosition, tableHeight);
    }

    EndPaint(hWnd, &paintStruct);
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR cmdLine, _In_ int cmdShowMode)
{
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
    windowClassEx.hbrBackground = CreateSolidBrush(GetSysColor(BACKGROUND_COLOR));
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

    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        GetDataFromFile();
        break;
    case WM_PAINT:
    {
        DrawTableWithText(hWnd);
        break;
    }
    case WM_SIZE:
        _userWindowWidth = LOWORD(lParam);
        _userWindowHeight = HIWORD(lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_ERASEBKGND:
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}