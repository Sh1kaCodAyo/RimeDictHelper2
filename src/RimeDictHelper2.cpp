#define UNICODE
#define _UNICODE
#include "RimeDictHelper2.h"

// Global Variables:

// function declare:
int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_RIMEDICTHELPER2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow)) {
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RIMEDICTHELPER2));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

/**
 * 注册窗口
 * @param hInstance
 * @return
 */
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

/**
 * init instance
 * @param hInstance
 * @param nCmdShow
 * @return
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // Store instance handle in our global variable

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	int winWidth = 290;
	int winHeight = 240;

	int x = (screenWidth - winWidth) / 3;
	int y = (screenHeight - winHeight) / 3;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x, y, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	CreateWindowW(L"STATIC", L"词语", WS_CHILD | WS_VISIBLE, 30, 20, 60, 25, hWnd, nullptr, hInstance, nullptr);
	hWord = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 150, 25, hWnd, (HMENU)ID_WORD, hInstance, nullptr);
	CreateWindowW(L"STATIC", L"编码", WS_CHILD | WS_VISIBLE, 30, 50, 60, 25, hWnd, nullptr, hInstance, nullptr);
	hCode = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 50, 150, 25, hWnd, (HMENU)ID_CODE, hInstance, nullptr);
	CreateWindowW(L"STATIC", L"权重", WS_CHILD | WS_VISIBLE, 30, 80, 60, 25, hWnd, nullptr, hInstance, nullptr);
	hWeight = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 80, 150, 25, hWnd, (HMENU)ID_WEIGHT, hInstance, nullptr);
	// CreateWindowW(L"Button", L"查询编码", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 100, 50, 25, hWnd, (HMENU)ID_CODE_BTN, hInstance, nullptr);
	hBtnAdd = CreateWindowW(L"Button", L"添加", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 120, 50, 25, hWnd, (HMENU)ID_ADD_BTN, hInstance, nullptr);
	hBtnSync = CreateWindowW(L"Button", L"部署", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 120, 50, 25, hWnd, (HMENU)ID_SYNC_BTN, hInstance, nullptr);
	hBtnAddSync = CreateWindowW(L"Button", L"添加并部署", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, 120, 100, 25, hWnd, (HMENU)ID_ADD_SYNC_BTN, hInstance, nullptr);
	hStatusBar = CreateWindowW( STATUSCLASSNAMEW, nullptr, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_STATUSBAR, hInstance, nullptr);

	g_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
	EnumChildWindows(hWnd, SetChildFont, (LPARAM)g_hFont);
	SetWindowTextW(hWeight, L"20");
	UpdateSyncButtonState(hWnd);

	// 为编码输入框设置最大长度
	SendMessage(hWord, EM_LIMITTEXT, 10, 0);
	SendMessage(hCode, EM_LIMITTEXT, 4, 0);
	SendMessage(hWeight, EM_LIMITTEXT, 3, 0);

	// 设置子类化
	g_oldWordProc = (WNDPROC)SetWindowLongPtr(hWord, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
	g_oldCodeProc = (WNDPROC)SetWindowLongPtr(hCode, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);
	g_oldWeightProc = (WNDPROC)SetWindowLongPtr(hWeight, GWLP_WNDPROC, (LONG_PTR)EditSubclassProc);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	g_hWnd = hWnd;

	SetFocus(hWord);
	SendMessage(hWord, EM_SETSEL, 0, -1);
	PostMessage(hWnd, WM_LOAD_DICT, 0, 0);

	return TRUE;
}

/**
 * 重写编辑框对事件的响应行为
 * @param hWnd
 * @param message
 * @param wParam
 * @param lParam
 * @return
 */
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	WNDPROC oldProc = nullptr;
	if (hWnd == hWord) {
		oldProc = g_oldWordProc;
	} else if (hWnd == hCode) {
		oldProc = g_oldCodeProc;
	} else if (hWnd == hWeight) {
		oldProc = g_oldWeightProc;
	}

	switch (message) {
	case WM_COMMAND: {
		break;
	}
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


	// ===== 新增：文本变化时自动生成编码 =====
	case WM_KEYUP: {
		if (hWnd == hWord) {
			// 获取当前文本
			wchar_t currentText[256];
			GetWindowTextW(hWnd, currentText, 256);

			// 只对词语输入框做自动补全
			static std::wstring lastText;
			std::wstring newText = currentText;

			if (newText != lastText) {
				lastText = newText;
				// 自动生成编码
				HWND hParent = GetParent(hWnd);
				if (hParent) {
					getCode(hParent);
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
	}

	case WM_KEYDOWN: {
		// ctrl+A
		if (wParam == 'A' && (GetKeyState(VK_CONTROL) & 0x8000)) {
			SendMessage(hWnd, EM_SETSEL, 0, -1);
			return 0;
		}
		// enter
		if (wParam == VK_RETURN) {
			if (!enableSync) {
				return 0;
			}
			HWND hParent = GetParent(hWnd);
			addAndSync(hParent);
			return 0;
		}
		// tab
		else if (wParam == VK_TAB) {
			bool bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
			HWND hNext = nullptr;

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
	default:
		break;
	}

	if (oldProc != nullptr) {
		return CallWindowProc(oldProc, hWnd, message, wParam, lParam);
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam) {
	SendMessage(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
	return TRUE;
}

// ========== 检测文件是否存在 ==========
bool FileExists(const std::wstring& filePath) {
	DWORD attrs = GetFileAttributesW(filePath.c_str());
	return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// ========== 检测同步脚本是否存在 ==========
bool IsSyncScriptAvailable() {
	// 默认检测当前目录下的 after.bat
	return FileExists(SCRIPT_NAME);
}

// ========== 更新同步按钮状态 ==========
void UpdateSyncButtonState(HWND hWnd) {
	bool syncAvailable = IsSyncScriptAvailable();

	// 获取按钮句柄（如果还没保存，可以通过 GetDlgItem 获取）
	HWND hBtnSync = GetDlgItem(hWnd, ID_SYNC_BTN);
	HWND hBtnAddSync = GetDlgItem(hWnd, ID_ADD_SYNC_BTN);

	// 启用或禁用按钮
	enableSync = syncAvailable;
	EnableWindow(hBtnSync, syncAvailable);
	EnableWindow(hBtnAddSync, syncAvailable);
}

/**
 * 自动生成词组编码
 * @param hWnd
 */
void getCode(HWND hWnd) {
	//GetCharCode
	wchar_t word[256];
	GetDlgItemTextW(hWnd, ID_WORD, word, 256);
	if (wcslen(word) == 0) {
		SetWindowTextW(hCode, L"");
		return;
	}
	// 输入的词语
	std::wstring wordsw(word);

	// 词语转编码
	std::wstring codesw;
	size_t len = wordsw.length();
	if (len == 1) {
		codesw = GetCharCode(wordsw);
	} else if (len == 2) {
		std::wstring codesw1 = GetCharCode(wordsw[0]);
		std::wstring codesw2 = GetCharCode(wordsw[1]);
		codesw = codesw1.substr(0, 2) + codesw2.substr(0, 2);
	} else if (len == 3) {
		std::wstring codesw1 = GetCharCode(wordsw[0]);
		std::wstring codesw2 = GetCharCode(wordsw[1]);
		std::wstring codesw3 = GetCharCode(wordsw[2]);
		codesw = codesw1.substr(0, 1) + codesw2.substr(0, 1) + codesw3.substr(0, 2);
	} else { // len >= 4
		std::wstring codesw1 = GetCharCode(wordsw[0]);
		std::wstring codesw2 = GetCharCode(wordsw[1]);
		std::wstring codesw3 = GetCharCode(wordsw[2]);
		std::wstring codesw4 = GetCharCode(wordsw[len - 1]);
		codesw = codesw1.substr(0, 1) + codesw2.substr(0, 1) + codesw3.substr(0, 1) + codesw4.substr(0, 1);
	}

	// 显示编码
	SetWindowTextW(hCode, codesw.c_str());
}

int add(HWND hWnd) {
	// 获取输入内容
	wchar_t word[256], code[256], weight[256];
	GetDlgItemTextW(hWnd, ID_WORD, word, 256);
	GetDlgItemTextW(hWnd, ID_CODE, code, 256);
	GetDlgItemTextW(hWnd, ID_WEIGHT, weight, 256);

	// 校验参数
	if (wcslen(word) == 0 || wcslen(code) == 0) {
		SetStatusText(hWnd, L"词语和编码不能为空！");
		return -1;
	}

	// 权重默认为20
	if (wcslen(weight) == 0) {
		wcscpy_s(weight, L"20");
	}

	// 拼接追加行内容
	wchar_t line[512];
	swprintf(line, 512, L"%s\t%s\t%s\n", word, code, weight);
	OutputDebugString(line);

	// 获取词典文件路径
	std::wstring dictPath = getConfigValue(CONFIG_KEY_USER_DICT);
	errno_t err;
	FILE* fp = nullptr;
	err = _wfopen_s(&fp, dictPath.c_str(), L"a, ccs=UTF-8");

	// 追加词典行
	if (err == 0 && fp != nullptr) {
		fputws(line, fp);
		fclose(fp);
		SetStatusText(hWnd, L"添加完成！");
		return 0;
	} else {
		MessageBoxW(hWnd, L"无法打开词典文件！", L"错误", MB_OK | MB_ICONERROR);
		return -1;
	}
}
void sync(HWND hWnd) {
	// 检查脚本是否存在。正常情况下不会不存在（不存在的话按钮会被禁用，无法调用此方法）
	if (!IsSyncScriptAvailable()) {
		MessageBoxW(hWnd, L"同步脚本不存在！", L"提示", MB_OK | MB_ICONWARNING);
		UpdateSyncButtonState(hWnd);
		return;
	}

	// 禁用按钮，防止重复点击
	enableSync = FALSE;
	EnableWindow(hBtnSync, FALSE);
	EnableWindow(hBtnAddSync, FALSE);
	SetStatusText(hWnd, L"正在执行部署脚本...");

	// 构建命令行
	std::wstring cmd = L"cmd.exe /c \"" + std::wstring(SCRIPT_NAME) + L"\"";

	// 设置启动信息
	STARTUPINFOW si = { sizeof(STARTUPINFOW) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;  // 隐藏窗口

	PROCESS_INFORMATION pi = { 0 };

	// 创建进程
	BOOL success = CreateProcessW(
		nullptr,
		(LPWSTR)cmd.c_str(),
		nullptr,
		nullptr,
		FALSE,
		CREATE_NO_WINDOW,
		nullptr,
		nullptr,
		&si,
		&pi
	);

	if (success) {
		// 在后台线程中等待，完成后通知主线程
		HANDLE hThread = CreateThread(nullptr, 0, WaitForScriptThread, (LPVOID)pi.hProcess, 0, nullptr);
		if (hThread) {
			CloseHandle(hThread);
		}

		CloseHandle(pi.hThread);
	}
	else {
		MessageBoxW(hWnd, L"启动部署脚本失败！", L"错误", MB_OK | MB_ICONERROR);
		enableSync = TRUE;
		EnableWindow(hBtnSync, TRUE);
		EnableWindow(hBtnAddSync, TRUE);
		SetStatusText(hWnd, L"部署失败");
	}
}
void addAndSync(HWND hWnd) {
	int addresp = add(hWnd);
	if (addresp == 0) {
		sync(hWnd);
	}
}

// 等待脚本完成的线程
DWORD WINAPI WaitForScriptThread(LPVOID lpParam) {
	HANDLE hProcess = (HANDLE)lpParam;

	// 等待进程结束
	DWORD waitResult = WaitForSingleObject(hProcess, INFINITE);

	// 获取退出码
	DWORD exitCode = 0;
	GetExitCodeProcess(hProcess, &exitCode);

	CloseHandle(hProcess);

	// 通知主线程
	PostMessage(g_hWnd, WM_SCRIPT_COMPLETE, (WPARAM)exitCode, 0);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_LOAD_DICT: {
		OutputDebugString(L"开始异步加载基本词库...\n");
		SetStatusText(hWnd, L"基本词库加载中...");
		// 启动后台线程加载词库
		HANDLE hThread = CreateThread( nullptr, 0, LoadDictThread, hWnd, 0, nullptr);
		if (hThread) {
			CloseHandle(hThread);  // 分离线程，让系统回收
		} else {
			SetStatusText(hWnd, L"启动加载线程失败");
		}
		break;
	}
	case WM_LOAD_DICT_COMPLETE: {
		bool success = (bool)wParam;
		size_t dictSize = (size_t)lParam;

		if (success) {
			std::wstring msg = L"基本词库加载完成，共 " + std::to_wstring(dictSize) + L" 个单字";
			SetStatusText(hWnd, msg);
			OutputDebugString((msg + L"\n").c_str());
			getCode(hWnd);
		}
		else {
			SetStatusText(hWnd, L"基本词库加载失败");
			OutputDebugString(L"基本词库加载失败\n");
		}
		break;
	}
	case WM_SCRIPT_COMPLETE: {
		DWORD exitCode = (DWORD)wParam;

		// 恢复按钮状态
		enableSync = TRUE;
		EnableWindow(hBtnSync, TRUE);
		EnableWindow(hBtnAddSync, TRUE);

		if (exitCode == 0) {
			SetStatusText(hWnd, L"部署完成");
		}
		else {
			SetStatusText(hWnd, L"部署失败，退出码: " + std::to_wstring(exitCode));
		}
		break;
	}
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
		//case ID_CODE_BTN: {
		//	getCode(hWnd);
		//	break;
		//}
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
		if (g_hFont != nullptr) {
			DeleteObject(g_hFont);
			g_hFont = nullptr;
		}
		PostQuitMessage(0);
		break;
	}
	default: {
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	}
	return 0;
}



// ========== 查询单字编码 ==========
std::wstring GetCharCode(const std::wstring& character) {
	auto it = g_charCodeMap.find(character);
	if (it != g_charCodeMap.end()) {
		return it->second;
	}
	return L"";  // 未找到
}
// reload
std::wstring GetCharCode(wchar_t ch) {
	std::wstring key(1, ch);
	auto it = g_charCodeMap.find(key);
	if (it != g_charCodeMap.end()) {
		return it->second;
	}
	return L"";
}

// 设置状态栏文本
void SetStatusText(HWND hWnd, const std::wstring& text) {
	HWND hStatusBar = GetDlgItem(hWnd, IDC_STATUSBAR);
	if (hStatusBar) {
		SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)text.c_str());
	}
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
