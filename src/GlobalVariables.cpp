//
// Created by Ftwrjh on 2026/8/15.
//

#include "GlobalVariables.h"

const wchar_t* CONFIG_FILE = L".\\config.ini";
const wchar_t* CONFIG_SECTION = L"Settings";
const wchar_t* CONFIG_KEY_BASE_DICT = L"BaseDictPath";
const wchar_t* CONFIG_KEY_USER_DICT = L"UserDictPath";
const wchar_t* SCRIPT_NAME = L".\\after.bat";
bool enableSync = TRUE, ctrlEnter = FALSE;
HFONT g_hFont;
HINSTANCE hInst;
HWND hWord, hCode, hWeight, hParent, hStatusBar, hBtnAdd, hBtnSync, hBtnAddSync, hListView, g_hWnd;
std::unordered_map<std::wstring, std::wstring> g_charCodeMap;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
WNDPROC g_oldWordProc = nullptr, g_oldCodeProc = nullptr, g_oldWeightProc = nullptr;
std::wstring defaultMsg;