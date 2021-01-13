//  Service.cpp : Defines the entry point for the console application.
//
#include <string>
#include <iostream>

#include "Service.h"

using namespace std;

#pragma warning(disable : 4996)

/** Window Service **/
TCHAR pServiceName[MAX_PATH];

SERVICE_TABLE_ENTRY		lpServiceStartTable[] = 
{
	{pServiceName, ServiceMain},
	{NULL, NULL}
};

SERVICE_STATUS_HANDLE   hServiceStatusHandle; 
SERVICE_STATUS          ServiceStatus; 
LPSERVICE_MAIN_FUNCTION UserServiceMain=NULL;
LPSERVICE_USER_FUNCTION UserStopFunction=NULL;

DWORD Install(const TCHAR* pPath, const TCHAR* pName)
{  
	DWORD dwReturn;
	WriteLog(TEXT("-->sFany Install Path=[%s] Name=[%s] error code = %d"), pPath, pName, GetLastError());
	
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_CREATE_SERVICE); 
	if (schSCManager==0) {
		dwReturn = GetLastError();
		WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d"), dwReturn);
		return dwReturn;
	}

	SC_HANDLE schService = CreateService
	( 
		schSCManager,	/* SCManager database      */ 
		pName,			/* name of service         */ 
		pName,			/* service name to display */ 
		SERVICE_ALL_ACCESS,        /* desired access          */ 
		SERVICE_WIN32_OWN_PROCESS|SERVICE_INTERACTIVE_PROCESS , /* service type            */ 
		SERVICE_AUTO_START,      /* start type              */ 
		SERVICE_ERROR_NORMAL,      /* error control type      */ 
		pPath,			/* service's binary        */ 
		NULL,                      /* no load ordering group  */ 
		NULL,                      /* no tag identifier       */ 
		NULL,                      /* no dependencies         */ 
		NULL,                      /* LocalSystem account     */ 
		NULL
	);                     /* no password             */ 

	if (schService==0) {
		WriteLog(TEXT("-->Service Failed to create service %s, error code = %d"), pName, GetLastError());
	} else {
		WriteLog( TEXT("-->Service Service %s installed"), pName);
		CloseServiceHandle(schService); 
	}
	return CloseServiceHandle(schSCManager);	
}

VOID UnInstall(const TCHAR* pName)
{
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d"), GetLastError());
	}
	else
	{
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			WriteLog( TEXT("-->Service OpenService failed, error code = %d"), GetLastError());
		}
		else
		{
			if(!DeleteService(schService)) 
			{
				WriteLog(TEXT("-->Service Failed to delete service %s"), pName);
			}
			else 
			{
				WriteLog(TEXT("-->Service Service %s removed"),pName);
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager);	
	}
}

BOOL KillService(const TCHAR* pName) 
{ 
	// kill service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d"), GetLastError());
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			WriteLog(TEXT("-->Service OpenService failed, error code = %d"), GetLastError());
		}
		else
		{
			// call ControlService to kill the given service
			SERVICE_STATUS status;
			if(ControlService(schService,SERVICE_CONTROL_STOP,&status))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				WriteLog(TEXT("-->Service ControlService failed, error code = %d"), GetLastError());
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

BOOL RunService(const TCHAR* pName) 
{ 
	// run service with given name
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS); 
	if (schSCManager==0) 
	{
		WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d line:%d"), GetLastError(),__LINE__);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SERVICE_ALL_ACCESS);
		if (schService==0) 
		{
			WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d line:%d"), GetLastError(), __LINE__);
		}
		else
		{
			// call StartService to run the service
			if(StartService(schService, 0, (const TCHAR**)NULL))
			{
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager); 
				return TRUE;
			}
			else
			{
				WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d line:%d"), GetLastError(), __LINE__); 
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return FALSE;
}

DWORD  QueryService(const TCHAR* pName) {
    SERVICE_STATUS_PROCESS ssStatus; 
    DWORD dwBytesNeeded;

    // Get a handle to the SCM database. 
 
	SC_HANDLE schSCManager = OpenSCManager( NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE ); 
	if (schSCManager==0) 
	{
		WriteLog(TEXT("-->Service OpenSCManager failed, error code = %d line:%d"), GetLastError(), __LINE__);
	}
	else
	{
		// open the service
		SC_HANDLE schService = OpenService( schSCManager, pName, SC_MANAGER_ENUMERATE_SERVICE );
		if (schService==0) 
		{
			WriteLog(TEXT("-->Service OpenService failed, error code = %d line:%d"), GetLastError(), __LINE__);
		}
		else
		{
			// Check the status in case the service is not stopped. 

			if (!QueryServiceStatusEx( 
					schService,                     // handle to service 
					SC_STATUS_PROCESS_INFO,         // information level
					(LPBYTE) &ssStatus,             // address of structure
					sizeof(SERVICE_STATUS_PROCESS), // size of structure
					&dwBytesNeeded ) )              // size needed if buffer is too small
			{
				WriteLog(TEXT("-->Service QueryServiceStatusEx failed (%d)"), GetLastError());
			} else {
				CloseServiceHandle(schService); 
				CloseServiceHandle(schSCManager);
				return ssStatus.dwCurrentState;
			}
			CloseServiceHandle(schService); 
		}
		CloseServiceHandle(schSCManager); 
	}
	return 0;
}


VOID ExecuteService(const TCHAR* pName,LPSERVICE_MAIN_FUNCTION UserFunc)
{
	lpServiceStartTable[0].lpServiceName = (TCHAR*) pName;
	UserServiceMain=UserFunc;
	if(!StartServiceCtrlDispatcher(lpServiceStartTable))
	{
		WriteLog(TEXT("-->Service StartServiceCtrlDispatcher failed, error code = %d"), GetLastError());
	}
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
	DWORD   status = 0; 
    DWORD   specificError = 0xfffffff; 
 
    ServiceStatus.dwServiceType        = SERVICE_WIN32; 
    ServiceStatus.dwCurrentState       = SERVICE_START_PENDING; 
    ServiceStatus.dwControlsAccepted   = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE; 
    ServiceStatus.dwWin32ExitCode      = 0; 
    ServiceStatus.dwServiceSpecificExitCode = 0; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0; 
 
	WriteLog(TEXT("-->Service  ServiceMain start..."));

    hServiceStatusHandle = RegisterServiceCtrlHandler(pServiceName, ServiceHandler); 
    if (hServiceStatusHandle==0) 
    {
		WriteLog(TEXT("-->Service RegisterServiceCtrlHandler failed, error code = %d"), GetLastError());
        return; 
    } 
 
    // Initialization complete - report running status 
    ServiceStatus.dwCurrentState       = SERVICE_RUNNING; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		WriteLog(TEXT("-->Service SetServiceStatus failed, error code = %d"), GetLastError());
    } 

	if (UserServiceMain) {
		WriteLog(TEXT("-->Service callback UserServiceMain"));
		UserServiceMain(dwArgc, lpszArgv);
	}

    ServiceStatus.dwCurrentState       = SERVICE_STOPPED; 
    ServiceStatus.dwCheckPoint         = 0; 
    ServiceStatus.dwWaitHint           = 0;  
    if(!SetServiceStatus(hServiceStatusHandle, &ServiceStatus)) 
    { 
		WriteLog(TEXT("-->Service SetServiceStatus failed, error code = %d"), GetLastError());
    } 
}

VOID WINAPI ServiceHandler(DWORD fdwControl)
{
	WriteLog(TEXT("-->Service ServiceHandle fdwControl: 0x%0X"),fdwControl);
	switch(fdwControl) 
	{
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			ServiceStatus.dwWin32ExitCode = 0; 
			ServiceStatus.dwCurrentState  = SERVICE_STOPPED; 
			ServiceStatus.dwCheckPoint    = 0; 
			ServiceStatus.dwWaitHint      = 0;
			if (UserStopFunction) {
				UserStopFunction(fdwControl);
			}
			break; 
		case SERVICE_CONTROL_PAUSE:
			ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
			break;
		case SERVICE_CONTROL_CONTINUE:
			ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
			break;
		case SERVICE_CONTROL_INTERROGATE:
			break;
		default:
			break;
	};

	if (!SetServiceStatus(hServiceStatusHandle,  &ServiceStatus)) 
	{ 
		WriteLog(TEXT("-->Service SetServiceStatus failed, error code = %d"), GetLastError());
    } 
}
