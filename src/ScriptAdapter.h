//
// Created by Ftwrjh on 2026/8/15.
//
#pragma once
#include "CommonInclude.h"


extern bool FileExists(const std::wstring& filePath);
extern bool IsSyncScriptAvailable();
extern DWORD WINAPI WaitForScriptThread(LPVOID lpParam);
