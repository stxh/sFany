//  Service.cpp : Defines the entry point for the console application.
//
#pragma once
#include <windows.h>
#include "utils.h"

/** Window Service Functions**/
DWORD Install(const TCHAR* pPath, const TCHAR* pName);
VOID UnInstall(const TCHAR* pName);
BOOL KillService(const TCHAR* pName);
BOOL RunService(const TCHAR* pName);
DWORD  QueryService(const TCHAR* pName);
VOID ExecuteService(const TCHAR* pName,LPSERVICE_MAIN_FUNCTION UserServiceMain);

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceHandler(DWORD fdwControl);

typedef VOID (WINAPI *LPSERVICE_USER_FUNCTION)(DWORD fdwControl);