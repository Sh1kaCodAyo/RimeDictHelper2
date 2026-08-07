#include <windows.h>
#include <string>
#include <cstring>
#include <commctrl.h>
#include "framework.h"
#include "RimeDictHelper2.h"

#define MAX_LOADSTRING 100
#define ID_WORD 101
#define ID_CODE 102
#define ID_WEIGHT 103
#define ID_ADD_BTN 112
#define ID_SYNC_BTN 113
#define ID_ADD_SYNC_BTN 114

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWord, hCode, hWeight, hParent;
HFONT g_hFont;

// 子类化需要的变量
WNDPROC g_oldWordProc = NULL;
WNDPROC g_oldCodeProc = NULL;
WNDPROC g_oldWeightProc = NULL;

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
int add(HWND hWnd);
void sync(HWND hWnd);
void addAndSync(HWND hWnd);
LRESULT CALLBACK EditSubclassProc(HWND, UINT, WPARAM, LPARAM); // 新增

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

// 编辑框的子类化窗口过程
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	WNDPROC oldProc = NULL;
	if (hWnd == hWord) oldProc = g_oldWordProc;
	else if (hWnd == hCode) oldProc = g_oldCodeProc;
	else if (hWnd == hWeight) oldProc = g_oldWeightProc;

	switch (message) {
	case WM_CHAR: {
		wchar_t ch = (wchar_t)wParam;

		// 允许的控制字符：退格(8)、删除(127)、回车(13)、Tab(9)、Esc(27)
		if (ch == VK_BACK || ch == VK_DELETE || ch == VK_RETURN ||
			ch == VK_TAB || ch == VK_ESCAPE) {
			break; // 让这些字符通过
		}

		// 编码输入框限制
		if (hWnd == hCode) {
			// 只允许英文字母
			if (!iswalpha(ch) || !iswascii(ch)) {
				MessageBeep(MB_ICONWARNING);
				return 0;
			}

			// 获取当前文本长度（考虑可能的中文输入法候选）
			int len = GetWindowTextLengthW(hWnd);
			if (len >= 4) {
				MessageBeep(MB_ICONWARNING);
				return 0;
			}
		}

		// 权重输入框限制
		if (hWnd == hWeight) {
			// 只允许数字
			if (!iswdigit(ch)) {
				MessageBeep(MB_ICONWARNING);
				return 0;
			}

			// 获取当前选择范围和文本
			DWORD start, end;
			SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

			wchar_t currentText[256];
			GetWindowTextW(hWnd, currentText, 256);
			std::wstring newText = currentText;

			// 处理输入
			if (start != end) {
				newText.replace(start, end - start, 1, ch);
			}
			else {
				newText.insert(start, 1, ch);
			}

			// 验证范围：0-100
			if (!newText.empty()) {
				// 检查是否只包含数字（防止非数字字符）
				bool valid = true;
				for (wchar_t c : newText) {
					if (!iswdigit(c)) {
						valid = false;
						break;
					}
				}

				if (valid) {
					int value = _wtoi(newText.c_str());
					if (value < 0 || value > 100) {
						MessageBeep(MB_ICONWARNING);
						return 0;
					}
				}
			}
		}
		break;
	}

				// 额外处理：当失去焦点或用户粘贴时，验证内容
	case WM_KILLFOCUS: {
		if (hWnd == hWeight) {
			wchar_t text[256];
			GetWindowTextW(hWnd, text, 256);
			if (wcslen(text) > 0) {
				int value = _wtoi(text);
				if (value < 0 || value > 100) {
					// 自动修正为边界值
					if (value < 0) {
						SetWindowTextW(hWnd, L"0");
					}
					else if (value > 100) {
						SetWindowTextW(hWnd, L"100");
					}
				}
			}
		}
		break;
	}	case WM_KEYDOWN: {
		if (wParam == VK_RETURN) {
			HWND hParent = GetParent(hWnd);
			addAndSync(hParent);
			return 0;
		}
		else if (wParam == VK_TAB) {
			bool bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			HWND hNext = NULL;

			if (!bShift) {
				if (hWnd == hWord) hNext = hCode;
				else if (hWnd == hCode) hNext = hWeight;
				else if (hWnd == hWeight) hNext = hWord;
			}
			else {
				if (hWnd == hWord) hNext = hWeight;
				else if (hWnd == hCode) hNext = hWord;
				else if (hWnd == hWeight) hNext = hCode;
			}

			if (hNext) {
				SetFocus(hNext);
				SendMessage(hNext, EM_SETSEL, 0, -1);
			}
			return 0;
		}
		break;
	}
	}

	if (oldProc) {
		return CallWindowProc(oldProc, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
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

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int winWidth = 290;
	int winHeight = 230;

	// 黄金分割位置：宽度取 1/3，高度取 1/4（也可以调整）
	int x = (screenWidth - winWidth) / 3;
	int y = (screenHeight - winHeight) / 3;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x, y, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
	//HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		//CW_USEDEFAULT, CW_USEDEFAULT, 290, 230, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	CreateWindowW(L"STATIC", L"词语", WS_CHILD | WS_VISIBLE, 20, 20, 60, 25, hWnd, NULL, hInstance, NULL);
	hWord = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 150, 25, hWnd, (HMENU)ID_WORD, hInstance, NULL);
	CreateWindowW(L"STATIC", L"编码", WS_CHILD | WS_VISIBLE, 20, 50, 60, 25, hWnd, NULL, hInstance, NULL);
	hCode = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 50, 150, 25, hWnd, (HMENU)ID_CODE, hInstance, NULL);
	CreateWindowW(L"STATIC", L"权重", WS_CHILD | WS_VISIBLE, 20, 80, 60, 25, hWnd, NULL, hInstance, NULL);
	hWeight = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 80, 150, 25, hWnd, (HMENU)ID_WEIGHT, hInstance, NULL);
	CreateWindowW(L"Button", L"添加", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 120, 50, 25, hWnd, (HMENU)ID_ADD_BTN, hInstance, NULL);
	CreateWindowW(L"Button", L"同步", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 120, 50, 25, hWnd, (HMENU)ID_SYNC_BTN, hInstance, NULL);
	CreateWindowW(L"Button", L"添加并同步", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, 120, 100, 25, hWnd, (HMENU)ID_ADD_SYNC_BTN, hInstance, NULL);

	g_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
	EnumChildWindows(hWnd, SetChildFont, (LPARAM)g_hFont);

	// 为编码输入框设置最大长度
	SendMessage(hWord, EM_LIMITTEXT, 10, 0);
	// 为编码输入框设置最大长度
	SendMessage(hCode, EM_LIMITTEXT, 4, 0);
	// 为权重输入框设置最大长度（3位足够，因为100是3位）
	SendMessage(hWeight, EM_LIMITTEXT, 3, 0);
	// 设置子类化（在创建所有编辑框之后）
	g_oldWordProc = (WNDPROC)SetWindowLongPtr(hWord, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
	g_oldCodeProc = (WNDPROC)SetWindowLongPtr(hCode, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
	g_oldWeightProc = (WNDPROC)SetWindowLongPtr(hWeight, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);


	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// 将焦点设置到词语输入框
	SetFocus(hWord);
	// 可选：全选已有内容（如果有的话），方便直接覆盖输入
	SendMessage(hWord, EM_SETSEL, 0, -1);

	return TRUE;
}

// 配置文件路径
const wchar_t* CONFIG_FILE = L".\\config.ini";

// 读取配置
std::wstring GetDictPath() {
	wchar_t path[512] = { 0 };
	GetPrivateProfileStringW(L"Settings", L"DictPath", L"", path, 512, CONFIG_FILE);
	return std::wstring(path);
}

// 保存配置
void SaveDictPath(const std::wstring& path) {
	WritePrivateProfileStringW(L"Settings", L"DictPath", path.c_str(), CONFIG_FILE);
}

int add(HWND hWnd) {
	// 获取输入内容
	wchar_t word[256], code[256], weight[256];
	GetDlgItemTextW(hWnd, ID_WORD, word, 256);
	GetDlgItemTextW(hWnd, ID_CODE, code, 256);
	GetDlgItemTextW(hWnd, ID_WEIGHT, weight, 256);

	// 校验参数
	if (wcslen(word) == 0 || wcslen(code) == 0) {
		MessageBoxW(hWnd, L"词语和编码不能为空！", L"提示", MB_OK | MB_ICONWARNING);
		return -1;
	}

	// 权重默认为20
	if (wcslen(weight) == 0) {
		wcscpy_s(weight, L"20");
	}

	// 拼接追加行内容
	wchar_t line[512];
	swprintf(line, 512, L"%s\t%s\t%s\n", word, code, weight);
	OutputDebugStringW(line);

	// 获取词典文件路径
	std::wstring dictPath = GetDictPath();
	errno_t err;
	FILE* fp = nullptr;
	err = _wfopen_s(&fp, dictPath.c_str(), L"a, ccs=UTF-8");

	// 追加词典行
	if (err == 0 && fp != nullptr) {
		fputws(line, fp);
		fclose(fp);
		return 0;
	} else {
		MessageBoxW(hWnd, L"无法打开词典文件！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}
}
void sync(HWND hWnd) {
	ShellExecuteW(NULL, L"open", L".\\sync.bat", NULL, NULL, SW_SHOW);
}
void addAndSync(HWND hWnd) {
	int addresp = add(hWnd);
	if (addresp == 0) {
		sync(hWnd);
	}
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

	case WM_CTLCOLORSTATIC: {
		// 让静态文本背景透明
		HDC hdcStatic = (HDC)wParam;
		SetBkMode(hdcStatic, TRANSPARENT);
		// 可选：设置文本颜色（如果不想要默认黑色）
		// SetTextColor(hdcStatic, RGB(0, 0, 0));
		return (LRESULT)GetStockObject(NULL_BRUSH);
	}

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_ADD_BTN: {
			add(hWnd);
			break;
		}
		case ID_SYNC_BTN: {
			sync(hWnd);
			break;
		}
		case ID_ADD_SYNC_BTN: {
			addAndSync(hWnd);
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

