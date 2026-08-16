// DictTypes.h
#pragma once
#include "CommonInclude.h"
// #include <string>
// #include <vector>

struct DictEntry {
    std::wstring text;      // 词条文字
    int weight;             // 权重（如果有）
    int source;             // 0 = 基本词库, 1 = 用户词库, 2 = 扩展词库
    // std::wstring filePath;  // 来源文件路径（方便后续删除或修改）
    int lineNumber;         // 所在行号（方便定位）
    std::wstring code;      // 词条编码
};

extern std::unordered_map<std::wstring, std::vector<DictEntry>> g_codeToEntries;