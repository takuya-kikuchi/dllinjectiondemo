// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "stdafx.h"
#include "../WaruiDll/rewrite.h"

fpCreateFileW createFileWPtr = NULL;


HRESULT TerminateIfHooked(
)
{
	CONTEXT context;
	STACKFRAME64 stackFrame;

	RtlCaptureContext(&context);

	//
	// Set up stack frame.
	//
	ZeroMemory(&stackFrame, sizeof(STACKFRAME64));
	stackFrame.AddrPC.Offset = context.Rip;
	stackFrame.AddrPC.Mode = AddrModeFlat;
	stackFrame.AddrFrame.Offset = context.Rsp;
	stackFrame.AddrFrame.Mode = AddrModeFlat;
	stackFrame.AddrStack.Offset = context.Rsp;
	stackFrame.AddrStack.Mode = AddrModeFlat;

	auto frameCount = 0;

	while (frameCount < 10)
	{
		if (!StackWalk64(IMAGE_FILE_MACHINE_AMD64, GetCurrentProcess(), GetCurrentThread(), &stackFrame, &context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) {
			break;
		}

		if (stackFrame.AddrPC.Offset != 0)
		{
			HMODULE mod;
			if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCWSTR)stackFrame.AddrPC.Offset, &mod)) {
				TCHAR fileNameBuf[MAX_PATH];
				if (GetModuleFileNameW(mod, fileNameBuf, sizeof(fileNameBuf)) > 0) {
					if (wcsstr(fileNameBuf, TEXT("WaruiDll.dll")) != NULL) {
						MessageBoxW(NULL, TEXT("API Hooking Detected!"), TEXT("Good bye."), MB_OK);
						TerminateProcess(GetCurrentProcess(), 0);
					}
				}
			}
			frameCount++;
		} else {
			break;
		}
	}
	return S_OK;
}

HANDLE
WINAPI
ShieldedCreateFileW(
	_In_ LPCWSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile
)
{
	TerminateIfHooked();
	return createFileWPtr(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


void ShieldNotepad() {
	TCHAR fileNameBuf[MAX_PATH];

	// only for notepad.exe
	if (GetModuleFileName(GetModuleHandle(NULL), fileNameBuf, MAX_PATH) > 0) {
		if (_tcsstr(fileNameBuf, TEXT("notepad.exe")) != NULL) {
			SymInitialize(GetCurrentProcess(), NULL, TRUE);
			// Hooking, a return value is a pointer for HookedCreateFileW in WaruiDll.dll
			createFileWPtr = (fpCreateFileW)RewriteFunction("kernel32.dll", "CreateFileW", ShieldedCreateFileW);
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
		ShieldNotepad();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

