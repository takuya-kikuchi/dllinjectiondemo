// WaruiDll.cpp : DLL �A�v���P�[�V�����p�ɃG�N�X�|�[�g�����֐����`���܂��B
//
#include "stdafx.h"

LRESULT WINAPI HookProc(int code, WPARAM wParam, LPARAM lParam) {
	return NULL;
}

void Inject() {
	SetWindowsHookEx(WH_CALLWNDPROC, HookProc, GetModuleHandle(__TEXT("WaruiDll")), 0);
}