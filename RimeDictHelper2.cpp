#include <windows.h>
#include <string>
#include <cstring>
#include "framework.h"
#include "RimeDictHelper2.h"

#define MAX_LOADSTRING 100
#define ID_WORD 101
#define ID_CODE 102
#define ID_WEIGHT 103
#define ID_SYNC_BTN 114

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWord, hCode, hWeight, hParent;
HFONT g_hFont;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_RIMEDICTHELPER2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RIMEDICTHELPER2));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RIMEDICTHELPER2));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RIMEDICTHELPER2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

// 回调函数
BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam) {
	SendMessage(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT, CW_USEDEFAULT, 290, 230, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}
	g_hFont = CreateFont(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");

	CreateWindowW(L"STATIC", L"词语：", WS_CHILD | WS_VISIBLE, 20, 20, 60, 25, hWnd, NULL, hInstance, NULL);
	hWord = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 150, 25, hWnd, (HMENU)ID_WORD, hInstance, NULL);

	CreateWindowW(L"STATIC", L"编码：", WS_CHILD | WS_VISIBLE, 20, 50, 60, 25, hWnd, NULL, hInstance, NULL);
	hCode = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 50, 150, 25, hWnd, (HMENU)ID_CODE, hInstance, NULL);

	CreateWindowW(L"STATIC", L"权重：", WS_CHILD | WS_VISIBLE, 20, 80, 60, 25, hWnd, NULL, hInstance, NULL);
	hWeight = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 80, 150, 25, hWnd, (HMENU)ID_WEIGHT, hInstance, NULL);

	CreateWindowW(L"Button", L"添加并同步", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 80, 120, 100, 25, hWnd, (HMENU)ID_SYNC_BTN, hInstance, NULL);

	EnumChildWindows(hWnd, SetChildFont, (LPARAM)g_hFont);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND: {
		//OutputDebugString(L"call: WM_COMMAND\n");
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case ID_SYNC_BTN: {
			// 获取输入内容
			wchar_t word[256], code[256], weight[256];
			GetDlgItemTextW(hWnd, ID_WORD, word, 256);
			GetDlgItemTextW(hWnd, ID_CODE, code, 256);
			GetDlgItemTextW(hWnd, ID_WEIGHT, weight, 256);

			if (wcslen(word) == 0 || wcslen(code) == 0) {
				MessageBoxW(hWnd, L"词语和编码不能为空！", L"提示", MB_OK | MB_ICONWARNING);
				break;
			}

			// 权重默认为20
			if (wcslen(weight) == 0) {
				wcscpy_s(weight, L"20");
			}

			// 拼接追加行内容
			wchar_t line[512];
			swprintf(line, 512, L"%s\t%s\t%s\n", word, code, weight);
			OutputDebugStringW(line);


			break;
		}
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	}
	case WM_PAINT: {
		OutputDebugString(L"call: WM_PAINT\n");
		PAINTSTRUCT ps;
		BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DESTROY: {
		OutputDebugString(L"call: WM_DESTROY\n");
		PostQuitMessage(0);
		break;
	}
	default: {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG: {
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	}
	return (INT_PTR)FALSE;
}

