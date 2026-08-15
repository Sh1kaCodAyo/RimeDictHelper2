//
// Created by Ftwrjh on 2026/8/15.
//

#pragma once
#include "CommonInclude.h"

#define MAX_LOADSTRING 100
#define ID_WORD 101
#define ID_CODE 102
#define ID_WEIGHT 103
#define ID_CODE_BTN 115
#define ID_ADD_BTN 112
#define ID_SYNC_BTN 113
#define ID_ADD_SYNC_BTN 114
#define IDC_STATUSBAR 120
#define WM_LOAD_DICT (WM_USER + 100)
#define WM_LOAD_DICT_COMPLETE (WM_USER + 101)
#define WM_SCRIPT_COMPLETE (WM_USER + 102)

// Global Variables:
extern const wchar_t* CONFIG_FILE;
extern const wchar_t* CONFIG_SECTION;
extern const wchar_t* CONFIG_KEY_BASE_DICT;
extern const wchar_t* CONFIG_KEY_USER_DICT;
extern const wchar_t* SCRIPT_NAME;
extern bool enableSync;
extern HFONT g_hFont;
extern HINSTANCE hInst;
extern HWND hWord, hCode, hWeight, hParent, hStatusBar, hBtnAdd, hBtnSync, hBtnAddSync, g_hWnd;
extern std::unordered_map<std::wstring, std::wstring> g_charCodeMap;
extern WCHAR szTitle[MAX_LOADSTRING];
extern WCHAR szWindowClass[MAX_LOADSTRING];
extern WNDPROC g_oldWordProc, g_oldCodeProc, g_oldWeightProc;
