// 窗口相关功能
#pragma once
#include <windowsx.h>
#include "CommonInclude.h"
#include "DictAdapter.h"
#include "ScriptAdapter.h"
#include "Function.h"

extern ATOM MyRegisterClass(HINSTANCE hInstance);
extern BOOL InitInstance(HINSTANCE, int);
extern LRESULT CALLBACK EditSubclassProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam);
extern void SetStatusText(HWND hWnd, const std::wstring& text);
extern void UpdateSyncButtonState(HWND hWnd);
extern INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
