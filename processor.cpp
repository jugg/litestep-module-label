#include "common.h"
#include "processor.h"
#include "pdh.h"

#undef PdhAddCounter
#undef PdhCloseQuery
#undef PdhCollectQueryData
#undef PdhGetFormattedCounterValue
#undef PdhOpenQuery

boolean isWinNT;
boolean isWin2K;

boolean startProcessorStatistics95();
int getProcessorUsage95();
boolean endProcessorStatistics95();

boolean startProcessorStatisticsNT();
int getProcessorUsageNT();
boolean endProcessorStatisticsNT();

boolean startProcessorStatistics()
{
	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&verInfo);

	if(verInfo.dwPlatformId == VER_PLATFORM_WIN32_NT) isWinNT = true;
	if(isWinNT && verInfo.dwMajorVersion >= 5) isWin2K = true;

	if(isWinNT)
		return startProcessorStatisticsNT();
	else
		return startProcessorStatistics95();
}

int getProcessorUsage()
{
	if(isWinNT)
		return getProcessorUsageNT();
	else
		return getProcessorUsage95();
}

boolean endProcessorStatistics()
{
	if(isWinNT)
		return endProcessorStatisticsNT();
	else
		return endProcessorStatistics95();
}

HKEY hkeyStatData = 0;

boolean startProcessorStatistics95()
{
	HKEY hkeyStartStat;

	long errorCode = RegOpenKeyEx(HKEY_DYN_DATA,
		"PerfStats\\StartStat",
		0,
		KEY_READ,
		&hkeyStartStat);

	if(errorCode == ERROR_SUCCESS)
	{
		unsigned long dataType;
		unsigned long size;

		errorCode = RegQueryValueEx(hkeyStartStat,
			"KERNEL\\CPUUsage",
			0,
			&dataType,
			0,
			&size);

		if(errorCode == ERROR_SUCCESS)
		{
			unsigned char *data = new unsigned char[size];

			errorCode = RegQueryValueEx(hkeyStartStat,
				"KERNEL\\CPUUsage",
				0,
				&dataType,
				data,
				&size);

			delete[] data;

			if(errorCode == ERROR_SUCCESS)
			{
				errorCode = RegOpenKeyEx(HKEY_DYN_DATA,
					"PerfStats\\StatData",
					0,
					KEY_READ,
					&hkeyStatData);

				if(errorCode == ERROR_SUCCESS)
					getProcessorUsage95();
			}
		}

		RegCloseKey(hkeyStartStat);
	}

	return (errorCode == ERROR_SUCCESS);
}

int getProcessorUsage95()
{
	unsigned long dataType;
	unsigned long value;
	unsigned long size = sizeof(unsigned long);

	long errorCode = RegQueryValueEx(hkeyStatData,
		"KERNEL\\CPUUsage",
		0,
		&dataType,
		(unsigned char *) &value,
		&size);

	return (errorCode == ERROR_SUCCESS) ? (int) value : -1;
}

boolean endProcessorStatistics95()
{
	RegCloseKey(hkeyStatData);
	hkeyStatData = 0;

	HKEY hkeyStopStat;

	long errorCode = RegOpenKeyEx(HKEY_DYN_DATA,
		"PerfStats\\StopStat",
		0,
		KEY_READ,
		&hkeyStopStat);

	if(errorCode == ERROR_SUCCESS)
	{
		unsigned long dataType;
		unsigned long size;

		errorCode = RegQueryValueEx(hkeyStopStat,
			"KERNEL\\CPUUsage",
			0,
			&dataType,
			0,
			&size);

		if(errorCode == ERROR_SUCCESS)
		{
			unsigned char *data = new unsigned char[size];

			errorCode = RegQueryValueEx(hkeyStopStat,
				"KERNEL\\CPUUsage",
				0,
				&dataType,
				data,
				&size);

			delete[] data;
		}

		RegCloseKey(hkeyStopStat);
	}

	return (errorCode == ERROR_SUCCESS);
}

typedef PDH_STATUS (WINAPI *PFN_ADDCOUNTER)(HQUERY, LPCTSTR, DWORD, HCOUNTER *);
typedef PDH_STATUS (WINAPI *PFN_CLOSEQUERY)(HQUERY);
typedef PDH_STATUS (WINAPI *PFN_COLLECTQUERYDATA)(HQUERY);
typedef PDH_STATUS (WINAPI *PFN_GETFORMATTEDCOUNTERVALUE)(HCOUNTER, DWORD, DWORD *, PDH_FMT_COUNTERVALUE *);
typedef PDH_STATUS (WINAPI *PFN_OPENQUERY)(LPCTSTR, DWORD, HQUERY *);

HCOUNTER hCounter;
HQUERY hQuery;
HINSTANCE hPdhLib;

PFN_ADDCOUNTER PdhAddCounterP;
PFN_CLOSEQUERY PdhCloseQueryP;
PFN_COLLECTQUERYDATA PdhCollectQueryDataP;
PFN_GETFORMATTEDCOUNTERVALUE PdhGetFormattedCounterValueP;
PFN_OPENQUERY PdhOpenQueryP;

boolean startProcessorStatisticsNT()
{
	hCounter = 0;
	hQuery = 0;

	hPdhLib = LoadLibrary("PDH.DLL");

	if(hPdhLib)
	{
		PdhAddCounterP = (PFN_ADDCOUNTER) GetProcAddress(hPdhLib, "PdhAddCounterA");
		PdhCloseQueryP = (PFN_CLOSEQUERY) GetProcAddress(hPdhLib, "PdhCloseQuery");
		PdhCollectQueryDataP = (PFN_COLLECTQUERYDATA) GetProcAddress(hPdhLib, "PdhCollectQueryData");
		PdhGetFormattedCounterValueP = (PFN_GETFORMATTEDCOUNTERVALUE) GetProcAddress(hPdhLib, "PdhGetFormattedCounterValue");
		PdhOpenQueryP = (PFN_OPENQUERY) GetProcAddress(hPdhLib, "PdhOpenQueryA");

		PDH_STATUS pdhStatus = PdhOpenQueryP(0,
			0,
			&hQuery);

		if(pdhStatus == ERROR_SUCCESS)
		{
			pdhStatus = PdhAddCounterP(hQuery,
				isWin2K ? "\\Processor(_Total)\\% Processor Time" : "\\System\\% Total Processor Time",
				0,
				&hCounter);

			if(pdhStatus == ERROR_SUCCESS)
			{
				pdhStatus = PdhCollectQueryDataP(hQuery);
			}
			else MessageBox(0, "Error: PdhAddCounter", "Label", MB_SETFOREGROUND);
		}
		else MessageBox(0, "Error: PdhOpenQuery", "Label", MB_SETFOREGROUND);

		return (pdhStatus == ERROR_SUCCESS);
	}
	else MessageBox(0, "Error: LoadLibrary", "Label", MB_SETFOREGROUND);

	return false;
}

int getProcessorUsageNT()
{
	PDH_STATUS pdhStatus = PdhCollectQueryDataP(hQuery);

	if(pdhStatus == ERROR_SUCCESS)
	{
		unsigned long counterType;
		PDH_FMT_COUNTERVALUE counterValue;

		pdhStatus = PdhGetFormattedCounterValueP(hCounter,
			PDH_FMT_LONG,
			&counterType,
			&counterValue);

		if(pdhStatus == ERROR_SUCCESS)
			return counterValue.longValue;
	}
	else MessageBox(0, "Error: PdhCollectQueryData", "Label", MB_SETFOREGROUND);

	return -1;
}

boolean endProcessorStatisticsNT()
{
	PDH_STATUS pdhStatus = PdhCloseQueryP(hQuery);
	hCounter = 0;
	hQuery = 0;

	FreeLibrary(hPdhLib);
	hPdhLib = 0;

	return (pdhStatus == ERROR_SUCCESS);
}