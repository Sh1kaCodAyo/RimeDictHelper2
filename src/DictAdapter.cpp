//
// Created by Ftwrjh on 2026/8/15.
//
#define UNICODE
#define _UNICODE
#include "DictAdapter.h"

// ========== 加载基本词库 ==========
bool LoadBaseDict(const std::wstring& filePath)
{
    // 清空已有数据（释放内存）
    g_charCodeMap.clear();
    // 预留空间以减少 rehash 次数
    g_charCodeMap.reserve(20000); // 预估词库大小

    // 打开文件（UTF-8 编码）
    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, filePath.c_str(), L"r, ccs=UTF-8");
    if (err != 0 || fp == nullptr)
    {
        OutputDebugString((L"无法打开基本词库文件：" + filePath + L"\n").c_str());
        return false;
    }

    // 逐行读取
    wchar_t line[1024];
    bool foundMarker = false; // 是否找到 "..." 标记
    int lineCount = 0;
    int charCount = 0;

    while (fgetws(line, 1024, fp) != nullptr)
    {
        lineCount++;

        // 去掉行尾的换行符
        size_t len = wcslen(line);
        if (len > 0 && (line[len - 1] == L'\n' || line[len - 1] == L'\r'))
        {
            line[len - 1] = L'\0';
            len--;
        }
        if (len > 0 && line[len - 1] == L'\r')
        {
            line[len - 1] = L'\0';
            len--;
        }

        // 检查是否是标记行 "..."
        if (wcscmp(line, L"...") == 0)
        {
            foundMarker = true;
            OutputDebugString(L"找到标记行 '...'，开始解析词库...\n");
            continue;
        }

        // 如果还没找到标记行，跳过
        if (!foundMarker)
        {
            continue;
        }

        // 跳过空行或注释行
        if (line[0] == L'\0' || line[0] == L'#')
        {
            continue;
        }

        // 解析字典行
        ParseDictLine(line);
        charCount++;

        // 每1000行输出一次进度（调试用）
        if (charCount % 1000 == 0)
        {
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

// ========== 后台线程加载词库 ==========
DWORD WINAPI LoadDictThread(LPVOID lpParam)
{
    HWND hWnd = (HWND)lpParam;
    OutputDebugString(L"后台线程开始加载词库...\n");
    std::wstring baseDictPath = getConfigValue(CONFIG_KEY_BASE_DICT);
    bool success = false;
    size_t dictSize = 0;
    if (!baseDictPath.empty())
    {
        success = LoadBaseDict(baseDictPath);
        dictSize = g_charCodeMap.size();
    }
    LoadAllDicts();
    // 通知主线程加载完成
    PostMessage(hWnd, WM_LOAD_DICT_COMPLETE, (WPARAM)success, (LPARAM)dictSize);
    return 0;
}

// ========== 解析单行 ==========
void ParseDictLine(const wchar_t* line)
{
    // 按制表符分割
    std::wstring str(line);
    size_t tab1 = str.find(L'\t');
    if (tab1 == std::wstring::npos) return;

    size_t tab2 = str.find(L'\t', tab1 + 1);
    std::wstring character = str.substr(0, tab1);
    std::wstring codeField = str.substr(tab1 + 1, tab2 - tab1 - 1);

    // 只处理单字（长度必须为1）
    if (character.length() != 1)
    {
        return;
    }

    // 提取最长编码
    std::wstring longestCode = GetLongestCode(codeField);
    if (longestCode.empty())
    {
        return;
    }

    // 存入 unordered_map
    // 如果已存在且新编码更长，则更新
    auto it = g_charCodeMap.find(character);
    if (it == g_charCodeMap.end())
    {
        // 不存在，直接插入
        g_charCodeMap.emplace(character, longestCode);
        // 或 g_charCodeMap[character] = longestCode;
    }
    else if (longestCode.length() > it->second.length())
    {
        // 已存在但新编码更长，更新
        it->second = longestCode;
    }
}

// ========== 获取最长编码 ==========
std::wstring GetLongestCode(const std::wstring& codeField)
{
    std::wstring longestCode;
    size_t start = 0;
    size_t end = 0;

    while (end < codeField.length())
    {
        // 查找 '/' 分隔符
        end = codeField.find(L'/', start);
        std::wstring code;

        if (end == std::wstring::npos)
        {
            // 最后一个编码段
            code = codeField.substr(start);
            start = codeField.length();
        }
        else
        {
            code = codeField.substr(start, end - start);
            start = end + 1;
        }

        // 更新最长编码
        if (code.length() > longestCode.length())
        {
            longestCode = code;
        }
    }

    return longestCode;
}

// ========== 查询单字编码 ==========
std::wstring GetCharCode(const std::wstring& character)
{
    auto it = g_charCodeMap.find(character);
    if (it != g_charCodeMap.end())
    {
        return it->second;
    }
    return L""; // 未找到
}

// reload
std::wstring GetCharCode(wchar_t ch)
{
    std::wstring key(1, ch);
    auto it = g_charCodeMap.find(key);
    if (it != g_charCodeMap.end())
    {
        return it->second;
    }
    return L"";
}

/**
 * 自动生成词组编码
 * @param hWnd
 */
void getCode(HWND hWnd)
{
    //GetCharCode
    wchar_t word[256];
    GetDlgItemTextW(hWnd, ID_WORD, word, 256);
    if (wcslen(word) == 0)
    {
        SetWindowTextW(hCode, L"");
        // SetStatusText(hWnd, L"等待输入词语");
        SetStatusText(hWnd, defaultMsg);
        return;
    }
    // 输入的词语
    std::wstring wordsw(word);

    // 词语转编码
    std::wstring codesw;
    size_t len = wordsw.length();
    if (len == 1)
    {
        codesw = GetCharCode(wordsw);
    }
    else if (len == 2)
    {
        std::wstring codesw1 = GetCharCode(wordsw[0]);
        std::wstring codesw2 = GetCharCode(wordsw[1]);
        codesw = codesw1.substr(0, 2) + codesw2.substr(0, 2);
    }
    else if (len == 3)
    {
        std::wstring codesw1 = GetCharCode(wordsw[0]);
        std::wstring codesw2 = GetCharCode(wordsw[1]);
        std::wstring codesw3 = GetCharCode(wordsw[2]);
        codesw = codesw1.substr(0, 1) + codesw2.substr(0, 1) + codesw3.substr(0, 2);
    }
    else
    {
        // len >= 4
        std::wstring codesw1 = GetCharCode(wordsw[0]);
        std::wstring codesw2 = GetCharCode(wordsw[1]);
        std::wstring codesw3 = GetCharCode(wordsw[2]);
        std::wstring codesw4 = GetCharCode(wordsw[len - 1]);
        codesw = codesw1.substr(0, 1) + codesw2.substr(0, 1) + codesw3.substr(0, 1) + codesw4.substr(0, 1);
    }

    int count = CountDuplicatesForCode(codesw);
    if (count == 0)
    {
        SetStatusText(hWnd, L"当前编码无重码");
    }
    else
    {
        SetStatusText(hWnd, (L"当前编码有 " + std::to_wstring(count) + L" 个词条").c_str());
    }

    // 显示编码
    SetWindowTextW(hCode, codesw.c_str());
}

// 加载词库文件（通用）
bool LoadDictFile(const std::wstring& filePath, int source)
{
    FILE* fp = nullptr;
    errno_t err = _wfopen_s(&fp, filePath.c_str(), L"r, ccs=UTF-8");
    if (err != 0 || fp == nullptr)
    {
        return false;
    }

    wchar_t line[1024];
    bool foundMarker = false;

    while (fgetws(line, 1024, fp) != nullptr)
    {
        // 去掉行尾换行符（省略，参考 LoadBaseDict 中的处理）
        // 跳过空行、注释行、YAML头部、非单字等（省略）

        std::wstring str(line);
        size_t tab1 = str.find(L'\t');
        if (tab1 == std::wstring::npos) continue;
        size_t tab2 = str.find(L'\t', tab1 + 1);
        if (tab2 == std::wstring::npos) continue;

        std::wstring text = str.substr(0, tab1);
        std::wstring code = str.substr(tab1 + 1, tab2 - tab1 - 1);
        int weight = _wtoi(str.substr(tab2 + 1).c_str());

        // 存入索引（保留所有词条，不限于单字）
        g_codeToEntries[code].push_back({text, weight, source});
    }

    fclose(fp);
    return true;
}

// 加载所有词库（在程序启动时调用）
void LoadAllDicts()
{
    g_codeToEntries.clear();

    // 加载基本词库
    std::wstring basePath = getConfigValue(CONFIG_KEY_BASE_DICT);
    if (!basePath.empty())
    {
        LoadDictFile(basePath, 0);
    }

    // 加载用户词库
    std::wstring userPath = getConfigValue(CONFIG_KEY_USER_DICT);
    if (!userPath.empty())
    {
        LoadDictFile(userPath, 1);
    }
}

// 统计重码数量
int CountDuplicatesForCode(const std::wstring& code)
{
    auto it = g_codeToEntries.find(code);
    if (it == g_codeToEntries.end())
    {
        return 0; // 无此编码
    }
    return (int)it->second.size(); // 返回该编码下的词条数量
}

// 统计所有重码（可选）
int CountAllDuplicates()
{
    int duplicateCount = 0;
    for (auto& entry : g_codeToEntries)
    {
        if (entry.second.size() > 1)
        {
            duplicateCount += (int)entry.second.size();
        }
    }
    return duplicateCount;
}
