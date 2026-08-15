//
// Created by Ftwrjh on 2026/8/15.
//

#include "ScriptAdapter.h"
// ========== 检测文件是否存在 ==========
bool FileExists(const std::wstring& filePath)
{
    DWORD attrs = GetFileAttributesW(filePath.c_str());
    return attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY);
}

// ========== 检测同步脚本是否存在 ==========
bool IsSyncScriptAvailable()
{
    // 默认检测当前目录下的 after.bat
    return FileExists(SCRIPT_NAME);
}

// 等待脚本完成的线程
DWORD WINAPI WaitForScriptThread(LPVOID lpParam)
{
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
