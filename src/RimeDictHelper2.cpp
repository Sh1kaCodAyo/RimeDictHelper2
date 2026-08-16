#define UNICODE
#define _UNICODE
#include "RimeDictHelper2.h"

/**
 * main()
 * @param hInstance
 * @param hPrevInstance
 * @param lpCmdLine
 * @param nCmdShow
 * @return
 */
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadStringW(hInstance, IDC_RIMEDICTHELPER2, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  if (!InitInstance(hInstance, nCmdShow)) {
    return FALSE;
  }

  HACCEL hAccelTable =
      LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_RIMEDICTHELPER2));

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
  hInst = hInstance;

  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);
  int winWidth = 600;
  int winHeight = 300;

  int x = (screenWidth - winWidth) / 3;
  int y = (screenHeight - winHeight) / 3;

  HWND hWnd = CreateWindowW(
      szWindowClass, szTitle,
      WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, x, y, winWidth,
      winHeight, nullptr, nullptr, hInstance, nullptr);

  if (!hWnd) {
    return FALSE;
  }

  CreateWindowW(L"STATIC", L"词条", WS_CHILD | WS_VISIBLE, 30, 20, 60, 25, hWnd,
                nullptr, hInstance, nullptr);
  hWord = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100,
                        20, 150, 25, hWnd, (HMENU)ID_WORD, hInstance, nullptr);
  CreateWindowW(L"STATIC", L"编码", WS_CHILD | WS_VISIBLE, 30, 60, 60, 25, hWnd,
                nullptr, hInstance, nullptr);
  hCode = CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100,
                        60, 150, 25, hWnd, (HMENU)ID_CODE, hInstance, nullptr);
  CreateWindowW(L"STATIC", L"权重", WS_CHILD | WS_VISIBLE, 30, 100, 60, 25,
                hWnd, nullptr, hInstance, nullptr);
  hWeight =
      CreateWindowW(L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, 100, 100,
                    150, 25, hWnd, (HMENU)ID_WEIGHT, hInstance, nullptr);
  hBtnAdd = CreateWindowW(L"Button", L"添加",
                          WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 160, 50,
                          25, hWnd, (HMENU)ID_ADD_BTN, hInstance, nullptr);
  hBtnSync = CreateWindowW(L"Button", L"部署",
                           WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 90, 160, 50,
                           25, hWnd, (HMENU)ID_SYNC_BTN, hInstance, nullptr);
  hBtnAddSync = CreateWindowW(
      L"Button", L"添加并部署", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 160, 160,
      100, 25, hWnd, (HMENU)ID_ADD_SYNC_BTN, hInstance, nullptr);
  hStatusBar =
      CreateWindowW(STATUSCLASSNAMEW, nullptr, WS_CHILD | WS_VISIBLE, 0, 0, 0,
                    0, hWnd, (HMENU)IDC_STATUSBAR, hInstance, nullptr);
  hListView = CreateWindowW(
      WC_LISTVIEWW, L"",
      WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, 310, 20,
      250, 160, hWnd, (HMENU)ID_LISTVIEW, hInstance, nullptr);
  ListView_SetExtendedListViewStyle(hListView, LVS_EX_FULLROWSELECT);

  // 添加列
  LVCOLUMNW col = {0};
  col.mask = LVCF_TEXT | LVCF_WIDTH;
  col.cx = 50; // 编码列宽
  col.pszText = LPWSTR(L"编码");
  ListView_InsertColumn(hListView, 0, &col);
  col.cx = 80; // 词条列宽
  col.pszText = LPWSTR(L"词条");
  ListView_InsertColumn(hListView, 1, &col);
  col.cx = 50; // 权重列宽
  col.pszText = LPWSTR(L"权重");
  ListView_InsertColumn(hListView, 2, &col);
  col.cx = 70; // 来源列宽
  col.pszText = LPWSTR(L"来源");
  ListView_InsertColumn(hListView, 3, &col);

  g_hFont =
      CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
                 DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
  EnumChildWindows(hWnd, SetChildFont, (LPARAM)g_hFont);
  SetWindowTextW(hWeight, L"20");
  UpdateSyncButtonState(hWnd);

  // 为编码输入框设置最大长度
  SendMessage(hWord, EM_LIMITTEXT, 10, 0);
  // SendMessage(hCode, EM_LIMITTEXT, 4, 0);
  SendMessage(hWeight, EM_LIMITTEXT, 3, 0);
  // 从 config.ini 中读取编码最大长度
  std::wstring maxCodeLenStr = getConfigValue(L"MaxCodeLength");
  int maxCodeLen;
  try {
    maxCodeLen = std::stoi(maxCodeLenStr);
  } catch (const std::exception &) {
    maxCodeLen = 4;
  }
  SendMessage(hCode, EM_LIMITTEXT, (WPARAM)maxCodeLen, 0);

  // 设置子类化
  g_oldWordProc = (WNDPROC)SetWindowLongPtr(hWord, GWLP_WNDPROC,
                                            (LONG_PTR)EditSubclassProc);
  g_oldCodeProc = (WNDPROC)SetWindowLongPtr(hCode, GWLP_WNDPROC,
                                            (LONG_PTR)EditSubclassProc);
  g_oldWeightProc = (WNDPROC)SetWindowLongPtr(hWeight, GWLP_WNDPROC,
                                              (LONG_PTR)EditSubclassProc);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  g_hWnd = hWnd;

  SetFocus(hWord);
  SendMessage(hWord, EM_SETSEL, 0, -1);
  PostMessage(hWnd, WM_LOAD_DICT, 0, 0);
  // LoadAllDicts();

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
LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
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
    if (ch == VK_BACK || ch == VK_DELETE || ch == VK_RETURN || ch == VK_TAB ||
        ch == VK_ESCAPE) {
      break; // 让这些字符通过
    }

    // 编码输入框限制
    if (hWnd == hCode) {
      // 只允许英文字母
      if (!iswalpha(ch) || !iswascii(ch)) {
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
      } else {
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
        if (hWnd == hWord) {
          hNext = hCode;
        } else if (hWnd == hCode) {
          hNext = hWeight;
        } else if (hWnd == hWeight) {
          hNext = hWord;
        }
      } else {
        if (hWnd == hWord) {
          hNext = hWeight;
        } else if (hWnd == hCode) {
          hNext = hWord;
        } else if (hWnd == hWeight) {
          hNext = hCode;
        }
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

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                         LPARAM lParam) {
  switch (message) {
  case IDM_EDIT_WEIGHT: {
    // 假设你已获取当前选中词条的权重，并存于变量 initialWeight 中
    int initialWeight = 50; // 示例：从你的数据结构中获取实际值

    // 调用对话框，将初始权重作为 lParam 传入
    int result = DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_WEIGHT_DLG), hWnd,
                                 WeightDialogProc, (LPARAM)initialWeight);

    if (result >= 0 && result <= 99) {
      // 用户点击确定，result 即为修改后的权重
      // 在这里执行更新词条权重的操作
      // 例如：UpdateWeightForSelectedEntry(result);
      MessageBoxW(hWnd, (L"权重已更新为: " + std::to_wstring(result)).c_str(),
                  L"提示", MB_OK);
    } else if (result == -1) {
      // 用户点击取消，不做任何操作
      SetStatusText(hWnd, L"已取消修改");
    }
    break;
  }
  case WM_NOTIFY: {
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->idFrom == ID_LISTVIEW) {
      switch (pnmh->code) {
      case NM_RCLICK: {
        // 1. 获取点击位置（屏幕坐标）
        POINT pt;
        GetCursorPos(&pt);

        // 2. 将屏幕坐标转换为 ListView 的客户区坐标
        HWND hListView = GetDlgItem(hWnd, ID_LISTVIEW);
        POINT ptClient = pt;
        ScreenToClient(hListView, &ptClient);

        // 3. 检测点击在哪一行
        LVHITTESTINFO hitInfo = {0};
        hitInfo.pt = ptClient;
        int itemIndex = ListView_HitTest(hListView, &hitInfo);

        // 4. 如果点击不在任何项目上，不弹出菜单
        if (itemIndex == -1) {
          break;
        }

        // 5. 选中该项目
        ListView_SetItemState(hListView, itemIndex,
                              LVIS_SELECTED | LVIS_FOCUSED,
                              LVIS_SELECTED | LVIS_FOCUSED);

        // 6. 创建并弹出右键菜单
        HMENU hMenu = CreatePopupMenu();
        AppendMenuW(hMenu, MF_STRING, IDM_DELETE_ENTRY, L"删除词条");
        // AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
        AppendMenuW(hMenu, MF_STRING, IDM_EDIT_WEIGHT, L"修改权重");

        // 使用点击时的屏幕坐标 pt
        TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
        DestroyMenu(hMenu);

        // 7. 告知系统我们已经处理了此通知
        SetWindowLongPtr(hWnd, DWLP_MSGRESULT, TRUE);
        return TRUE;
      }
      }
    }
    break;
  }

  case WM_LOAD_DICT: {
    OutputDebugString(L"开始异步加载基本词库...\n");
    SetStatusText(hWnd, L"基本词库加载中...");
    // 启动后台线程加载词库
    HANDLE hThread = CreateThread(nullptr, 0, LoadDictThread, hWnd, 0, nullptr);
    if (hThread) {
      CloseHandle(hThread); // 分离线程，让系统回收
    } else {
      SetStatusText(hWnd, L"启动加载线程失败");
    }
    break;
  }
  case WM_LOAD_DICT_COMPLETE: {
    bool success = (bool)wParam;
    size_t dictSize = (size_t)lParam;

    if (success) {
      defaultMsg =
          L"基本词库加载完成，共 " + std::to_wstring(dictSize) + L" 个单字";
      SetStatusText(hWnd, defaultMsg);
      OutputDebugString((defaultMsg + L"\n").c_str());
      getCode(hWnd);
    } else {
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
    } else {
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
    // case ID_CODE_BTN: {
    //	getCode(hWnd);
    //	break;
    // }
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
    case IDM_DELETE_ENTRY: {
      // 1. 获取当前选中的行
      HWND hListView = GetDlgItem(hWnd, ID_LISTVIEW);
      int selectedIndex = ListView_GetNextItem(hListView, -1, LVNI_SELECTED);

      int value = selectedIndex;
      std::string output = "Value: " + std::to_string(value) + "\n";
      OutputDebugStringA(output.c_str());
      if (selectedIndex != -1) {
        // 2. 执行删除逻辑（该函数需要你自己实现）
        //    DeleteEntryByIndex(hListView, selectedIndex);
        //    删除后，记得刷新右侧列表
        //    UpdateConflictList(hListView, currentCode);
      }
      break;
    }
    case IDM_EDIT_WEIGHT: {
      // 假设你已获取当前选中词条的权重，并存于变量 initialWeight 中
      int initialWeight = 50; // 示例：从你的数据结构中获取实际值

      // 调用对话框，将初始权重作为 lParam 传入
      int result = DialogBoxParamW(hInst, MAKEINTRESOURCE(IDD_WEIGHT_DLG), hWnd,
                                   WeightDialogProc, (LPARAM)initialWeight);

      if (result >= 0 && result <= 99) {
        // 用户点击确定，result 即为修改后的权重
        // 在这里执行更新词条权重的操作
        // 例如：UpdateWeightForSelectedEntry(result);
        MessageBoxW(hWnd, (L"权重已更新为: " + std::to_wstring(result)).c_str(),
                    L"提示", MB_OK);
      } else if (result == -1) {
        // 用户点击取消，不做任何操作
        SetStatusText(hWnd, L"已取消修改");
      }
      break;
    }
    // ... 其他已有命令 ...
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

INT_PTR CALLBACK WeightDialogProc(HWND hDlg, UINT message, WPARAM wParam,
                                  LPARAM lParam) {
  // 使用静态变量或窗口额外存储区来保存初始值和当前值
  static int currentValue = 0; // 可以存储到窗口额外数据中

  switch (message) {
  case WM_INITDIALOG: {
    // 从 lParam 中获取传入的初始权重值
    currentValue = (int)lParam;
    // 设置滑块范围
    SendDlgItemMessage(hDlg, IDC_SLIDER, TBM_SETRANGE, TRUE, MAKELONG(0, 99));
    // 设置滑块位置
    SendDlgItemMessage(hDlg, IDC_SLIDER, TBM_SETPOS, TRUE, currentValue);
    // 更新显示文本
    SetDlgItemText(hDlg, IDC_VALUE_DISPLAY,
                   (L"当前值: " + std::to_wstring(currentValue)).c_str());
    return (INT_PTR)TRUE;
  }

  case WM_HSCROLL: {
    // 当滑块滑动时，获取新位置并更新显示
    int pos = (int)SendDlgItemMessage(hDlg, IDC_SLIDER, TBM_GETPOS, 0, 0);
    currentValue = pos;
    SetDlgItemText(hDlg, IDC_VALUE_DISPLAY,
                   (L"当前值: " + std::to_wstring(pos)).c_str());
    break;
  }

  case WM_COMMAND: {
    if (LOWORD(wParam) == IDOK) {
      // 用户点击确定，获取当前滑块值并结束对话框
      currentValue =
          (int)SendDlgItemMessage(hDlg, IDC_SLIDER, TBM_GETPOS, 0, 0);
      EndDialog(hDlg, currentValue); // 返回值作为结果

      return (INT_PTR)TRUE;
    }
    if (LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, -1); // 返回 -1 表示取消
      return (INT_PTR)TRUE;
    }
    break;
  }
  }
  return (INT_PTR)FALSE;
}

BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam) {
  SendMessage(hChild, WM_SETFONT, (WPARAM)lParam, TRUE);
  return TRUE;
}

// 设置状态栏文本
void SetStatusText(HWND hWnd, const std::wstring &text) {
  HWND hStatusBar = GetDlgItem(hWnd, IDC_STATUSBAR);
  if (hStatusBar) {
    SendMessage(hStatusBar, SB_SETTEXT, 0, (LPARAM)text.c_str());
  }
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
