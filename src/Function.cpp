//
// Created by Ftwrjh on 2026/8/15.
//

#define UNICODE
#define _UNICODE
#include "Function.h"
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

