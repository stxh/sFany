/***********************************
* utils
*
***********************************/
#include "utils.h"
#include <tchar.h>
#include <string>
#include <varargs.h>
#include <fstream>

static const int MAX_BUFFER_LEN = 1024;

DEBUG_OUTPUT_TYPE g_dotDebugOption=E_DEBUG_NULL;

int SetDebugOutputType(DEBUG_OUTPUT_TYPE dotOption) {
	return g_dotDebugOption=dotOption;
}

BOOL WriteLog(TCHAR* pLogFormat,...) {

	TCHAR strMessage[MAX_BUFFER_LEN];

	if (g_dotDebugOption > E_DEBUG_NULL) {
		TCHAR strMsg[MAX_BUFFER_LEN];
		va_list argptr;
		va_start ( argptr, pLogFormat );
		_vsntprintf(strMsg,MAX_BUFFER_LEN,pLogFormat,argptr);

		SYSTEMTIME oT;
		::GetLocalTime(&oT);

		_stprintf(strMessage,__T("%02d-%02d %02d:%02d:%02d %s"),oT.wMonth,oT.wDay,oT.wHour,oT.wMinute,oT.wSecond,strMsg); 
	}

	if (g_dotDebugOption == E_DEBUG_LOG || g_dotDebugOption == E_DEBUG_ALL) { // WRITE_DEBUG_LOG
		TCHAR strModuleFile[MAX_BUFFER_LEN];
		DWORD dwSize = GetModuleFileName(NULL, strModuleFile, MAX_BUFFER_LEN);

		#ifdef _UNICODE
			std::wstring strLogFile(strModuleFile);
			std::wfstream of;
		#else
			std::string strLogFile(strModuleFile);
			std::fstream of;
		#endif

		strLogFile += __T(".log");
		of.open(strLogFile,std::ios::app);
		of << strMessage << std::endl;
	}

	if (g_dotDebugOption == E_DEBUG_STRING || g_dotDebugOption == E_DEBUG_ALL) { //WRITE_DEBUG
		OutputDebugString(strMessage);
	}

	return TRUE;
}

BOOL WriteLogA(CHAR* pLogFormat,...) {

	CHAR strMessage[MAX_BUFFER_LEN];

	if (g_dotDebugOption > E_DEBUG_NULL) {
		CHAR strMsg[MAX_BUFFER_LEN];
		va_list argptr;
		va_start ( argptr, pLogFormat );
		_vsnprintf(strMsg,MAX_BUFFER_LEN,pLogFormat,argptr);

		SYSTEMTIME oT;
		::GetLocalTime(&oT);

		sprintf(strMessage,"%02d-%02d %02d:%02d:%02d %s",oT.wMonth,oT.wDay,oT.wHour,oT.wMinute,oT.wSecond,strMsg); 
	}

	if (g_dotDebugOption == E_DEBUG_LOG || g_dotDebugOption == E_DEBUG_ALL) { // WRITE_DEBUG_LOG
		TCHAR strModuleFile[MAX_BUFFER_LEN];
		DWORD dwSize = GetModuleFileName(NULL, strModuleFile, MAX_BUFFER_LEN);

		#ifdef _UNICODE
			std::wstring strLogFile(strModuleFile);
			std::wfstream of;
		#else
			std::string strLogFile(strModuleFile);
			std::fstream of;
		#endif

		strLogFile += __T(".log");
		of.open(strLogFile,std::ios::app);
		of << strMessage << std::endl;
	}

	if (g_dotDebugOption == E_DEBUG_STRING || g_dotDebugOption == E_DEBUG_ALL) { //WRITE_DEBUG
		std::string strTmp(strMessage);
		std::wstring wstrMessage(strTmp.begin(),strTmp.end());
		OutputDebugString(wstrMessage.c_str());
	}

	return TRUE;
}

BOOL isAdmin() {
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	PSID AdministratorsGroup;
	// Initialize SID.
	if( !AllocateAndInitializeSid( &NtAuthority,
								   2,
								   SECURITY_BUILTIN_DOMAIN_RID,
								   DOMAIN_ALIAS_RID_ADMINS,
								   0, 0, 0, 0, 0, 0,
								   &AdministratorsGroup))
	{
		// Initializing SID Failed.
		return false;
	}
	// Check whether the token is present in admin group.
	BOOL IsInAdminGroup = FALSE;
	if( !CheckTokenMembership( NULL,
							   AdministratorsGroup,
							   &IsInAdminGroup ))
	{
		// Error occurred.
		IsInAdminGroup = FALSE;
	}
	// Free SID and return.
	FreeSid(AdministratorsGroup);
	return IsInAdminGroup;
}