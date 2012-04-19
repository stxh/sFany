// sFany.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "stdafx.h"

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <list>
#include <sstream>

//#include <process.h>

#include "Service.h"
extern LPSERVICE_MAIN_FUNCTION UserServiceMain;	// User Service Main
extern LPSERVICE_USER_FUNCTION UserStopFunction;	// User Stop Function

#include "utils.h"

using namespace std;

BOOL ProcessStarted = TRUE;

typedef struct _sProcInfo {
	basic_string<TCHAR> strCmdLine;
	PROCESS_INFORMATION ProcInfo;
} sProcInfo, *psProcInfo;

list<psProcInfo> g_listProcInfo;

basic_string<TCHAR> g_strIniFile;
basic_string<TCHAR> g_strExeFile;

//*
BOOL StartProcess(psProcInfo pProc)
{
	STARTUPINFO startUpInfo = { sizeof(STARTUPINFO),NULL,NULL,NULL,0,0,0,0,0,0,0,STARTF_USESHOWWINDOW,0,0,NULL,0,0,0};  
	startUpInfo.wShowWindow = SW_HIDE;
	startUpInfo.lpDesktop = NULL;

	if(CreateProcess(NULL,(LPWSTR)pProc->strCmdLine.c_str(),NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS,NULL,NULL,&startUpInfo,&pProc->ProcInfo))
	{
		//Sleep(500);
		//CloseHandle(pProc->ProcInfo.hThread);
		//CloseHandle(pProc->ProcInfo.hProcess);
		return TRUE;
	} 
	else
	{
		WriteLog(TEXT("Failed to start program '%s', error code = %d"),pProc->strCmdLine.c_str(),GetLastError());
		return FALSE;
	}
}

#include <Tlhelp32.h>

BOOL __fastcall KillProcessTree(DWORD myprocID, DWORD dwTimeout)
{
  BOOL bRet = true;
  PROCESSENTRY32 pe;

  memset(&pe, 0, sizeof(PROCESSENTRY32));
  pe.dwSize = sizeof(PROCESSENTRY32);

  HANDLE hSnap = :: CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (::Process32First(hSnap, &pe))
  {
    BOOL bContinue = TRUE;

    // kill child processes
    while (bContinue)
    {
      if (pe.th32ParentProcessID == myprocID)
      {
        //ShowMessage ("Gleich - KILL PID: " + AnsiString(pe.th32ProcessID));

        // Rekursion
        KillProcessTree(pe.th32ProcessID, dwTimeout);

        HANDLE hChildProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);

        if (hChildProc)
        {
          if (WaitForSingleObject(hChildProc, dwTimeout) == WAIT_OBJECT_0)
            bRet = true;
          else
          {
            bRet = TerminateProcess(hChildProc, 0);
          }
          ::CloseHandle(hChildProc);
        }
      }
      bContinue = ::Process32Next(hSnap, &pe);
    }

    // kill the main process
    HANDLE hProc = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, myprocID);

    if (hProc)
    {
        ::TerminateProcess(hProc, 1);
        ::CloseHandle(hProc);
    }
  }
  return bRet;
}

VOID EndProcess(PPROCESS_INFORMATION pProc)
{
	if(pProc->hProcess)
	{
		KillProcessTree(pProc->dwProcessId,5000);
	}
}
//*/

VOID ProcMonitorThread(VOID *)
{
	list<psProcInfo>::iterator proc_Iter;
	for ( proc_Iter = g_listProcInfo.begin( ); proc_Iter != g_listProcInfo.end( ); proc_Iter++ ) {
		psProcInfo pProcInfo=*proc_Iter;
		StartProcess(pProcInfo);
		WriteLog(TEXT("Start process %s\n"),pProcInfo->strCmdLine);
	}
}

// User Main Service Function
VOID WINAPI sFanyStopFuntion(DWORD fdwControl)
{
	WriteLog(TEXT("--> sFanyMain start..."));
	ProcessStarted = FALSE;
	Sleep(1500);

	DWORD dwCode;
	list<psProcInfo>::iterator proc_Iter;
	for ( proc_Iter = g_listProcInfo.begin( ); proc_Iter != g_listProcInfo.end( ); proc_Iter++ ) {
		psProcInfo pProcInfo=*proc_Iter;
		if(pProcInfo->ProcInfo.hProcess && ::GetExitCodeProcess(pProcInfo->ProcInfo.hProcess, &dwCode) )
		{
			if(dwCode == STILL_ACTIVE)
			{
				EndProcess(&pProcInfo->ProcInfo);
				Sleep(1000);
				WriteLog(TEXT("End process %s\n"),pProcInfo->strCmdLine);
			}
		} else {
			dwCode=GetLastError();
		}
	}
}

// User Main Service Function
VOID WINAPI sFanyMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	WriteLog(TEXT("--> sFanyMain start..."));
	
	ProcMonitorThread(NULL);

	DWORD dwCode;
	list<psProcInfo>::iterator proc_Iter;

	while(ProcessStarted)
	{
		for ( proc_Iter = g_listProcInfo.begin( ); proc_Iter != g_listProcInfo.end( ); proc_Iter++ ) {
			psProcInfo pProcInfo=*proc_Iter;
			if(::GetExitCodeProcess(pProcInfo->ProcInfo.hProcess, &dwCode) && pProcInfo->ProcInfo.hProcess != NULL)
			{
				if(dwCode != STILL_ACTIVE)
				{
					if(StartProcess(pProcInfo))
					{
						WriteLog(TEXT("Start process %s\n"),pProcInfo->strCmdLine);
					}
				}
			}
		}
		Sleep(1000);
	}
}
//*/

void DisplayHelp()
{
	cout << "Usage: sFany [option]" << endl;
	cout << "Options:" << endl;
	cout << "\t-v \tView services list" << endl;
	cout << "\t-i \tInstall service" << endl;
	cout << "\t-u \tUninstall service" << endl;
	cout << "\t-k \tKill/stop service" << endl;
	cout << "\t-s \tStart service" << endl << endl;
}

TCHAR strServiceName[MAX_PATH]=__T("sFany");
//basic_string<TCHAR> strServiceName(__T("sFany")) ;

int _tmain(int argc, _TCHAR* argv[])
{
	WriteLog(__T("--> sFany start ==================================================="));

	TCHAR strServiceState[][32] = {
		__T("SERVICE_ERROR"),
		__T("SERVICE_STOPPED"),
		__T("SERVICE_START_PENDING"),
		__T("SERVICE_STOP_PENDING"),
		__T("SERVICE_RUNNING"),
		__T("SERVICE_CONTINUE_PENDING"),
		__T("SERVICE_PAUSE_PENDING"),
		__T("SERVICE_PAUSED")
	};

	// Ini
	std::vector<TCHAR> v(MAX_PATH);
	DWORD dwSize = GetModuleFileName(NULL, v.data(), MAX_PATH);
	g_strExeFile.assign(v.begin(),v.end());

	int nIndex=g_strExeFile.find_last_of(TEXT("."));
	g_strIniFile = g_strExeFile.replace(nIndex,g_strExeFile.length()-nIndex,TEXT(".ini"));

	// Service Name
	int nRet;

	v.empty();
	nRet=::GetPrivateProfileString(TEXT("Service"), TEXT("Name"), TEXT("sFany"), v.data(), MAX_PATH, g_strIniFile.c_str());
	memcpy((void*)strServiceName,v.data(),nRet);

	// Ip in ini
	basic_string<TCHAR> strKey;
	int		n=1;
	while(1) {
		std::vector<TCHAR> v1(MAX_PATH);
		wstringstream ss; 
		ss << TEXT("server") << n;
		ss >> strKey;

		nRet=::GetPrivateProfileString(TEXT("Service"), strKey.c_str(), NULL, v1.data(), MAX_PATH, g_strIniFile.c_str());

		if(!nRet) break;

		psProcInfo pProcInfo=new sProcInfo;

		pProcInfo->strCmdLine.assign(v1.begin(),v1.end());
		pProcInfo->strCmdLine.shrink_to_fit();

		g_listProcInfo.push_back(pProcInfo);
		n++;
		//wcout << strServices << endl;
	};

	if (argc == 2) {
		TCHAR strModuleFile[MAX_PATH];
		DWORD dwSize;
		list<psProcInfo>::iterator proc_Iter;
		if (argv[1][0] == '-') {
			switch(argv[1][1]) {
			case 'i':
				dwSize = GetModuleFileName(NULL, strModuleFile, MAX_PATH);
				Install(strModuleFile, strServiceName);
				break;
			case 'u':
				UnInstall(strServiceName);
				break;
			case 'k':
				KillService(strServiceName);
				break;
			case 's':
				RunService(strServiceName);
				break;
			case 'v':
				for ( proc_Iter = g_listProcInfo.begin( ); proc_Iter != g_listProcInfo.end( ); proc_Iter++ ) {
					psProcInfo pProcInfo=*proc_Iter;
					wcout << TEXT(" cmdlin:")<< pProcInfo->strCmdLine << endl;
				}
				break;
			default:
				DisplayHelp();
				break;
			}
			wcout << strServiceName << " Current State: " << strServiceState[QueryService(strServiceName)] << endl;
			return 0;
		}
	}

	// Setting User Stop Function
	UserStopFunction=sFanyStopFuntion;

	ExecuteService(strServiceName,sFanyMain);

	return 0;
}
