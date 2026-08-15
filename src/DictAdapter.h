//
// Created by Ftwrjh on 2026/8/15.
//
#pragma once
#include "CommonInclude.h"
#include "ConfigAdapter.h"

extern bool LoadBaseDict(const std::wstring& filePath);
extern DWORD WINAPI LoadDictThread(LPVOID lpParam);
extern void ParseDictLine(const wchar_t* line);
extern std::wstring GetLongestCode(const std::wstring& codeField);
extern std::wstring GetCharCode(const std::wstring& character);
extern std::wstring GetCharCode(wchar_t ch);
