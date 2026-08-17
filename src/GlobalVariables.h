//
// Created by Ftwrjh on 2026/8/15.
//
#pragma once
#include "CommonInclude.h"

// Global Variables:
extern const wchar_t* CONFIG_FILE;
extern const wchar_t* CONFIG_SECTION;
extern const wchar_t* CONFIG_KEY_BASE_DICT;
extern const wchar_t* CONFIG_KEY_USER_DICT;
extern const wchar_t* SCRIPT_NAME;
extern bool enableSync, ctrlEnter;
extern HFONT g_hFont;
extern HINSTANCE hInst;
extern HWND hWord, hCode, hWeight, hParent, hStatusBar, hBtnAdd, hBtnSync, hBtnAddSync, hListView, g_hWnd;
extern std::unordered_map<std::wstring, std::wstring> g_charCodeMap;
extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
extern WNDPROC g_oldWordProc, g_oldCodeProc, g_oldWeightProc;
extern std::wstring defaultMsg;
