#pragma once

#include "CommonInclude.h"
#include "DictAdapter.h"
#include "ScriptAdapter.h"

extern ATOM MyRegisterClass(HINSTANCE hInstance);
extern BOOL InitInstance(HINSTANCE, int);
extern INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK EditSubclassProc(HWND, UINT, WPARAM, LPARAM);
extern LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
extern int add(HWND hWnd);
extern void sync(HWND hWnd);
extern void addAndSync(HWND hWnd);
extern void getCode(HWND hWnd);
extern void SetStatusText(HWND hWnd, const std::wstring& text);
extern void UpdateSyncButtonState(HWND hWnd);
extern BOOL CALLBACK SetChildFont(HWND hChild, LPARAM lParam);
