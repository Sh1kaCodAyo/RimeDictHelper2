#pragma once
#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>
#include <cstring>
#include <commctrl.h>
#include <unordered_map>
#include "framework.h"
#include "RimeDictHelper2.h"

#define MAX_LOADSTRING 100
#define ID_WORD 101
#define ID_CODE 102
#define ID_WEIGHT 103
#define ID_CODE_BTN 115
#define ID_ADD_BTN 112
#define ID_SYNC_BTN 113
#define ID_ADD_SYNC_BTN 114
#define IDC_STATUSBAR 120
#define WM_LOAD_DICT (WM_USER + 100)
#define WM_LOAD_DICT_COMPLETE (WM_USER + 101)

// Global Variables:
const wchar_t *CONFIG_FILE = L".\\config.ini";
const wchar_t *CONFIG_SECTION = L"Settings";
const wchar_t *CONFIG_KEY_BASE_DICT = L"BaseDictPath";
const wchar_t *CONFIG_KEY_USER_DICT = L"UserDictPath";
const wchar_t *SCRIPT_NAME = L".\\after.bat";
extern std::unordered_map<std::wstring, std::wstring> g_charCodeMap;
HFONT g_hFont;
HINSTANCE hInst;
HWND hWord, hCode, hWeight, hParent, hStatusBar, hBtnAdd, hBtnSync, hBtnAddSync;
std::unordered_map<std::wstring, std::wstring> g_charCodeMap;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WNDPROC g_oldWordProc = NULL, g_oldCodeProc = NULL, g_oldWeightProc = NULL;

// function claim:
ATOM MyRegisterClass(HINSTANCE hInstance);

BOOL InitInstance(HINSTANCE, int);

bool FileExists(const std::wstring &filePath);

bool IsSyncScriptAvailable();

bool LoadBaseDict(const std::wstring &filePath);

DWORD WINAPI LoadDictThread(LPVOID lpParam);

int add(HWND hWnd);

INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK EditSubclassProc(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

std::wstring GetCharCode(const std::wstring &character);

std::wstring GetCharCode(wchar_t ch);

std::wstring GetLongestCode(const std::wstring &codeField);

void addAndSync(HWND hWnd);

void getCode(HWND hWnd);

void ParseDictLine(const wchar_t *line);

void SetStatusText(HWND hWnd, const std::wstring &text);

void sync(HWND hWnd);

void UpdateSyncButtonState(HWND hWnd);

std::wstring getConfigValue(LPCWSTR lpKeyName) {
    wchar_t path[512] = {0};
    GetPrivateProfileStringW(CONFIG_SECTION, lpKeyName, L"", path, 512, CONFIG_FILE);
    return std::wstring(path);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
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

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

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
    wcex.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RIMEDICTHELPER2);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    wcscpy_s(szWindowClass, L"RimeDictHelper2Class");
    return RegisterClassExW(&wcex);
}

// 编辑框的子类化窗口过程
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    WNDPROC oldProc = NULL;
    if (hWnd == hWord) oldProc = g_oldWordProc;
    else if (hWnd == hCode) oldProc = g_oldCodeProc;
    else if (hWnd == hWeight) oldProc = g_oldWeightProc;

    switch (message) {
        case WM_COMMAND: {
            break;
        }
        case WM_CHAR: {
            wchar_t ch = (wchar_t) wParam;

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
                SendMessage(hWnd, EM_GETSEL, (WPARAM) &start, (LPARAM) &end);

                wchar_t currentText[256];
                GetWindowTextW(hWnd, currentText, 256);
                std::wstring newText = currentText;

                // 处理输入
                if (start != end) {
                    newText.replace(start, end - start, 1, ch);
                } else {
                    newText.insert(start, 1, ch);
                }

                // 验证范围：0-100
                if (!newText.empty()) {
                    // 检查是否只包含数字（防止非数字字符）
                    bool valid = true;
                    for (wchar_t c: newText) {
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
                        } else if (value > 100) {
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
                HWND hParent = GetParent(hWnd);
                addAndSync(hParent);
                return 0;
            }
            // tab
            else if (wParam == VK_TAB) {
                bool bShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                HWND hNext = NULL;

                if (!bShift) {
                    if (hWnd == hWord) hNext = hCode;
                    else if (hWnd == hCode) hNext = hWeight;
                    else if (hWnd == hWeight) hNext = hWord;
                } else {
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

BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam) {
    SendMessage(hChild, WM_SETFONT, (WPARAM) lParam, TRUE);
    return TRUE;
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance; // Store instance handle in our global variable
    OutputDebugString(L"InitInstance called\n");
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int winWidth = 290;
    int winHeight = 240;

    int x = (screenWidth - winWidth) / 3;
    int y = (screenHeight - winHeight) / 3;

    HWND hWnd = CreateWindowW(L"RimeDictHelper2Class", L"RimeDictHelper2",
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                              x, y, winWidth, winHeight, nullptr, nullptr, hInstance, nullptr);
    // MessageBoxW(NULL, L"InitInstance 开始", L"调试", MB_OK);

    if (!hWnd) {
        DWORD err = GetLastError();
        wchar_t msg[256];
        swprintf(msg, 256, L"CreateWindowW 失败，错误码: %lu", err);
        MessageBoxW(NULL, msg, L"错误", MB_OK);
        return FALSE;
    }

    CreateWindowW(L"STATIC", L"词语", WS_CHILD | WS_VISIBLE, 30, 20, 60, 25, hWnd, NULL, hInstance, NULL);
    hWord = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 20, 150, 25, hWnd, (HMENU)ID_WORD,
                          hInstance, NULL);
    CreateWindowW(L"STATIC", L"编码", WS_CHILD | WS_VISIBLE, 30, 50, 60, 25, hWnd, NULL, hInstance, NULL);
    hCode = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 50, 150, 25, hWnd, (HMENU)ID_CODE,
                          hInstance, NULL);
    CreateWindowW(L"STATIC", L"权重", WS_CHILD | WS_VISIBLE, 30, 80, 60, 25, hWnd, NULL, hInstance, NULL);
    hWeight = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 80, 150, 25, hWnd, (HMENU)ID_WEIGHT,
                            hInstance, NULL);
    //CreateWindowW(L"Button", L"查询编码", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 100, 50, 25, hWnd, (HMENU)ID_CODE_BTN, hInstance, NULL);
    hBtnAdd = CreateWindowW(L"Button", L"添加", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 120, 50, 25, hWnd,
                            (HMENU)ID_ADD_BTN, hInstance, NULL);
    hBtnSync = CreateWindowW(L"Button", L"同步", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 120, 50, 25, hWnd,
                             (HMENU)ID_SYNC_BTN, hInstance, NULL);
    hBtnAddSync = CreateWindowW(L"Button", L"添加并同步", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, 120, 100, 25, hWnd,
                                (HMENU)ID_ADD_SYNC_BTN, hInstance, NULL);
    hStatusBar = CreateWindowW(STATUSCLASSNAMEW, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)IDC_STATUSBAR,
                               hInstance, NULL);

    g_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                         CLIP_DEFAULT_PRECIS,
                         CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
    EnumChildWindows(hWnd, SetChildFont, (LPARAM) g_hFont);
    SetWindowTextW(hWeight, L"20");
    UpdateSyncButtonState(hWnd);

    // 为编码输入框设置最大长度
    SendMessage(hWord, EM_LIMITTEXT, 10, 0);
    SendMessage(hCode, EM_LIMITTEXT, 4, 0);
    SendMessage(hWeight, EM_LIMITTEXT, 3, 0);
    // 设置子类化（在创建所有编辑框之后）
    g_oldWordProc = (WNDPROC) SetWindowLongPtr(hWord, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc);
    g_oldCodeProc = (WNDPROC) SetWindowLongPtr(hCode, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc);
    g_oldWeightProc = (WNDPROC) SetWindowLongPtr(hWeight, GWLP_WNDPROC, (LONG_PTR) EditSubclassProc);


    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // 将焦点设置到词语输入框
    SetFocus(hWord);
    // 全选已有内容（如果有的话）
    SendMessage(hWord, EM_SETSEL, 0, -1);

    PostMessage(hWnd, WM_LOAD_DICT, 0, 0);
    //SetStatusText(hWnd, L"基本词库加载中……");

    return TRUE;
}

// ========== 检测文件是否存在 ==========
bool FileExists(const std::wstring &filePath) {
    DWORD attrs = GetFileAttributesW(filePath.c_str());
    return (attrs != INVALID_FILE_ATTRIBUTES &&
            !(attrs & FILE_ATTRIBUTE_DIRECTORY));
}

// ========== 检测同步脚本是否存在 ==========
bool IsSyncScriptAvailable() {
    // 默认检测当前目录下的 sync.bat
    return FileExists(SCRIPT_NAME);
}

// ========== 更新同步按钮状态 ==========
void UpdateSyncButtonState(HWND hWnd) {
    bool syncAvailable = IsSyncScriptAvailable();

    // 获取按钮句柄（如果还没保存，可以通过 GetDlgItem 获取）
    HWND hBtnSync = GetDlgItem(hWnd, ID_SYNC_BTN);
    HWND hBtnAddSync = GetDlgItem(hWnd, ID_ADD_SYNC_BTN);

    // 启用或禁用按钮
    EnableWindow(hBtnSync, syncAvailable);
    EnableWindow(hBtnAddSync, syncAvailable);
}

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
    } else {
        // len >= 4
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
    OutputDebugString(line);

    // 获取词典文件路径
    std::wstring dictPath = getConfigValue(CONFIG_KEY_USER_DICT);
    errno_t err;
    FILE *fp = nullptr;
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
    ShellExecuteW(NULL, L"open", SCRIPT_NAME, NULL, NULL, SW_SHOW);
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
        case WM_LOAD_DICT: {
            OutputDebugString(L"开始异步加载基本词库...\n");
            SetStatusText(hWnd, L"基本词库加载中...");

            // 启动后台线程加载词库
            HANDLE hThread = CreateThread(NULL, 0, LoadDictThread, hWnd, 0, NULL);

            if (hThread) {
                CloseHandle(hThread); // 分离线程，让系统回收
            } else {
                SetStatusText(hWnd, L"启动加载线程失败");
            }
            break;
        }
        case WM_LOAD_DICT_COMPLETE: {
            bool success = (bool) wParam;
            size_t dictSize = (size_t) lParam;

            if (success) {
                std::wstring msg = L"基本词库加载完成，共 " +
                                   std::to_wstring(dictSize) + L" 个单字";
                SetStatusText(hWnd, msg);
                OutputDebugString((msg + L"\n").c_str());
                getCode(hWnd);
            } else {
                SetStatusText(hWnd, L"基本词库加载失败");
                OutputDebugString(L"基本词库加载失败\n");
            }
            break;
        }
        case WM_CTLCOLORSTATIC: {
            // 让静态文本背景透明
            HDC hdcStatic = (HDC) wParam;
            SetBkMode(hdcStatic, TRANSPARENT);
            // 可选：设置文本颜色（如果不想要默认黑色）
            // SetTextColor(hdcStatic, RGB(0, 0, 0));
            return (LRESULT) GetStockObject(NULL_BRUSH);
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
            PostQuitMessage(0);
            break;
        }
        default: {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    return 0;
}

// ========== 后台线程加载词库 ==========
DWORD WINAPI LoadDictThread(LPVOID lpParam) {
    HWND hWnd = (HWND) lpParam;

    OutputDebugString(L"后台线程开始加载词库...\n");

    std::wstring baseDictPath = getConfigValue(CONFIG_KEY_BASE_DICT);
    bool success = false;
    size_t dictSize = 0;

    if (!baseDictPath.empty()) {
        success = LoadBaseDict(baseDictPath);
        dictSize = g_charCodeMap.size();
    }

    // 通知主线程加载完成
    PostMessage(hWnd, WM_LOAD_DICT_COMPLETE, (WPARAM) success, (LPARAM) dictSize);

    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    switch (message) {
        case WM_INITDIALOG: {
            return (INT_PTR) TRUE;
        }

        case WM_COMMAND: {
            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR) TRUE;
            }
            break;
        }
    }
    return (INT_PTR) FALSE;
}

// 设置状态栏文本
void SetStatusText(HWND hWnd, const std::wstring &text) {
    HWND hStatusBar = GetDlgItem(hWnd, IDC_STATUSBAR);
    if (hStatusBar) {
        SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM) text.c_str());
    }
}

// ========== 获取最长编码 ==========
std::wstring GetLongestCode(const std::wstring &codeField) {
    std::wstring longestCode;
    size_t start = 0;
    size_t end = 0;

    while (end < codeField.length()) {
        // 查找 '/' 分隔符
        end = codeField.find(L'/', start);
        std::wstring code;

        if (end == std::wstring::npos) {
            // 最后一个编码段
            code = codeField.substr(start);
            start = codeField.length();
        } else {
            code = codeField.substr(start, end - start);
            start = end + 1;
        }

        // 更新最长编码
        if (code.length() > longestCode.length()) {
            longestCode = code;
        }
    }

    return longestCode;
}

// ========== 解析单行 ==========
void ParseDictLine(const wchar_t *line) {
    // 按制表符分割
    std::wstring str(line);
    size_t tab1 = str.find(L'\t');
    if (tab1 == std::wstring::npos) return;

    size_t tab2 = str.find(L'\t', tab1 + 1);
    std::wstring character = str.substr(0, tab1);
    std::wstring codeField = str.substr(tab1 + 1, tab2 - tab1 - 1);

    // 只处理单字（长度必须为1）
    if (character.length() != 1) {
        return;
    }

    // 提取最长编码
    std::wstring longestCode = GetLongestCode(codeField);
    if (longestCode.empty()) {
        return;
    }

    // 存入 unordered_map
    // 如果已存在且新编码更长，则更新
    auto it = g_charCodeMap.find(character);
    if (it == g_charCodeMap.end()) {
        // 不存在，直接插入
        g_charCodeMap.emplace(character, longestCode);
        // 或 g_charCodeMap[character] = longestCode;
    } else if (longestCode.length() > it->second.length()) {
        // 已存在但新编码更长，更新
        it->second = longestCode;
    }
}

// ========== 加载基本词库 ==========
bool LoadBaseDict(const std::wstring &filePath) {
    // 清空已有数据（释放内存）
    g_charCodeMap.clear();
    // 预留空间以减少 rehash 次数
    g_charCodeMap.reserve(20000); // 预估词库大小

    // 打开文件（UTF-8 编码）
    FILE *fp = nullptr;
    errno_t err = _wfopen_s(&fp, filePath.c_str(), L"r, ccs=UTF-8");
    if (err != 0 || fp == nullptr) {
        OutputDebugString((L"无法打开基本词库文件：" + filePath + L"\n").c_str());
        return false;
    }

    // 逐行读取
    wchar_t line[1024];
    bool foundMarker = false; // 是否找到 "..." 标记
    int lineCount = 0;
    int charCount = 0;

    while (fgetws(line, 1024, fp) != nullptr) {
        lineCount++;

        // 去掉行尾的换行符
        size_t len = wcslen(line);
        if (len > 0 && (line[len - 1] == L'\n' || line[len - 1] == L'\r')) {
            line[len - 1] = L'\0';
            len--;
        }
        if (len > 0 && line[len - 1] == L'\r') {
            line[len - 1] = L'\0';
            len--;
        }

        // 检查是否是标记行 "..."
        if (wcscmp(line, L"...") == 0) {
            foundMarker = true;
            OutputDebugString(L"找到标记行 '...'，开始解析词库...\n");
            continue;
        }

        // 如果还没找到标记行，跳过
        if (!foundMarker) {
            continue;
        }

        // 跳过空行或注释行
        if (line[0] == L'\0' || line[0] == L'#') {
            continue;
        }

        // 解析字典行
        ParseDictLine(line);
        charCount++;

        // 每1000行输出一次进度（调试用）
        if (charCount % 1000 == 0) {
            OutputDebugString((L"已解析 " + std::to_wstring(charCount) +
                                L" 行，当前词库大小 " +
                                std::to_wstring(g_charCodeMap.size()) + L"\n").c_str());
        }
    }

    fclose(fp);

    OutputDebugString((L"========================================\n"));
    OutputDebugString((L"基本词库加载完成！\n"));
    OutputDebugString((L"总行数: " + std::to_wstring(lineCount) + L"\n").c_str());
    OutputDebugString((L"单字数: " + std::to_wstring(g_charCodeMap.size()) + L"\n").c_str());
    OutputDebugString((L"========================================\n"));

    return true;
}

// ========== 查询单字编码 ==========
std::wstring GetCharCode(const std::wstring &character) {
    auto it = g_charCodeMap.find(character);
    if (it != g_charCodeMap.end()) {
        return it->second;
    }
    return L""; // 未找到
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

// ========== 获取词库统计信息（调试用） ==========
size_t GetDictSize() {
    return g_charCodeMap.size();
}

double GetLoadFactor() {
    return g_charCodeMap.load_factor();
}
