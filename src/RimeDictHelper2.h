#pragma once

#include "CommonInclude.h"

extern ATOM MyRegisterClass(HINSTANCE hInstance);
extern BOOL InitInstance(HINSTANCE, int);
extern bool FileExists(const std::wstring& filePath);
extern bool IsSyncScriptAvailable();
extern bool LoadBaseDict(const std::wstring& filePath);
extern DWORD WINAPI LoadDictThread(LPVOID lpParam);
extern DWORD WINAPI WaitForScriptThread(LPVOID lpParam);
extern INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK EditSubclassProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern std::wstring GetCharCode(const std::wstring& character);
extern std::wstring GetCharCode(wchar_t ch);
extern std::wstring GetLongestCode(const std::wstring& codeField);
extern int add(HWND hWnd);
extern void sync(HWND hWnd);
extern void addAndSync(HWND hWnd);
extern void getCode(HWND hWnd);
extern void ParseDictLine(const wchar_t* line);
extern void SetStatusText(HWND hWnd, const std::wstring& text);
extern void UpdateSyncButtonState(HWND hWnd);
extern BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam);
extern std::wstring getConfigValue(LPCWSTR lpKeyName);

