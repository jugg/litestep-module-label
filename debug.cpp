#define WIN32_LEAN_AND_MEAN
#define STRICT

#include <windows.h>
#include "debug.h"

TCHAR szDebugFileName[MAX_PATH] = { 0 };

void DebugSetFileName(LPCTSTR pszFileName)
{
	szDebugFileName[0] = 0;

	if(pszFileName && pszFileName[0])
	{
		strcpy(szDebugFileName, pszFileName);

		SYSTEMTIME st;
		GetLocalTime(&st);

		DebugPrint("\r\n%02d-%02d-%04d %02d:%02d\r\n",
			st.wMonth,
			st.wDay,
			st.wYear,
			st.wHour,
			st.wMinute);
	}
}

void DebugPrint(LPCTSTR pszFormat, ...)
{
	if(!szDebugFileName[0])
		return;

	HANDLE hDebugFile = CreateFile(szDebugFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(hDebugFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hDebugFile,
		0,
		NULL,
		FILE_END);

	TCHAR szBuffer[2048];
	va_list vaArguments;

	va_start(vaArguments, pszFormat);
	wvsprintf(szBuffer, pszFormat, vaArguments);
	lstrcat(szBuffer, TEXT("\r\n"));
	va_end(vaArguments);

	DWORD dwBytesWritten;

	WriteFile(hDebugFile,
		szBuffer,
		lstrlen(szBuffer),
		&dwBytesWritten,
		NULL);

	CloseHandle(hDebugFile);
}
