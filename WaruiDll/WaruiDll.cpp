// WaruiDll.cpp : DLL アプリケーション用にエクスポートされる関数を定義します。
//
#include "stdafx.h"

LRESULT WINAPI HookProc(int code, WPARAM wParam, LPARAM lParam) {
	return NULL;
}

void Inject() {
	SetWindowsHookEx(WH_CALLWNDPROC, HookProc, GetModuleHandle(__TEXT("WaruiDll")), 0);
}