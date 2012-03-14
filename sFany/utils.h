/*
* utils.h
*/
#pragma once
#include <string>
#include <Windows.h>

typedef enum _DEBUG_OUTPUT_TYPE {
	E_DEBUG_NULL,
	E_DEBUG_STRING,
	E_DEBUG_LOG,
	E_DEBUG_ALL,
} DEBUG_OUTPUT_TYPE;

int SetDebugOutputType(DEBUG_OUTPUT_TYPE dotOption);

BOOL isAdmin();

BOOL WriteLog(TCHAR* pLogFormat,...);

BOOL WriteLogA(CHAR* pLogFormat,...);
