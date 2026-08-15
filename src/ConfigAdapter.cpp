//
// Created by Ftwrjh on 2026/8/15.
//

#include "ConfigAdapter.h"

std::wstring getConfigValue(LPCWSTR lpKeyName)
{
    wchar_t path[512] = {0};
    GetPrivateProfileStringW(CONFIG_SECTION, lpKeyName, L"", path, 512, CONFIG_FILE);
    return std::wstring(path);
}
