// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include "rewrite.h"

typedef HANDLE (WINAPI *fpCreateFileW)(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
	);

fpCreateFileW createFileWPtr = NULL;

HANDLE
WINAPI
HookedCreateFileW(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	if (lpFileName != NULL) {
		if (auto fileNameLength = lstrlen(lpFileName) > 5) {
			auto extPtr = lpFileName + lstrlen(lpFileName) - 5;
			if (lstrcmpW(extPtr, TEXT(".java")) == 0) {
				// よいこは真似しないこと
				lstrcpy((LPWSTR)extPtr, TEXT(".cs"));
			}
		}
	}
	return createFileWPtr(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


void NotepadHook() {
	TCHAR fileNameBuf[MAX_PATH];

	// only for notepad.exe
	if (GetModuleFileName(GetModuleHandle(NULL), fileNameBuf, MAX_PATH) > 0) {
		if (_tcsstr(fileNameBuf, TEXT("notepad.exe")) != NULL) {
			createFileWPtr = (fpCreateFileW)RewriteFunction("kernel32.dll", "CreateFileW", HookedCreateFileW);
		}
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		NotepadHook();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

