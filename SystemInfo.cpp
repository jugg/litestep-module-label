#include "common.h"
#include <stdio.h>
#include "processor.h"
#include "SystemInfo.h"
#include "Label.h"

SystemInfo::SystemInfo()
{
	WSADATA wd;
	WSAStartup(0x0101, &wd);

	usingCPU = false;

	am = getLocaleInfo(LOCALE_S1159);
	pm = getLocaleInfo(LOCALE_S2359);

	shortWeekdays[0] = getLocaleInfo(LOCALE_SABBREVDAYNAME7);
	shortWeekdays[1] = getLocaleInfo(LOCALE_SABBREVDAYNAME1);
	shortWeekdays[2] = getLocaleInfo(LOCALE_SABBREVDAYNAME2);
	shortWeekdays[3] = getLocaleInfo(LOCALE_SABBREVDAYNAME3);
	shortWeekdays[4] = getLocaleInfo(LOCALE_SABBREVDAYNAME4);
	shortWeekdays[5] = getLocaleInfo(LOCALE_SABBREVDAYNAME5);
	shortWeekdays[6] = getLocaleInfo(LOCALE_SABBREVDAYNAME6);

	weekdays[0] = getLocaleInfo(LOCALE_SDAYNAME7);
	weekdays[1] = getLocaleInfo(LOCALE_SDAYNAME1);
	weekdays[2] = getLocaleInfo(LOCALE_SDAYNAME2);
	weekdays[3] = getLocaleInfo(LOCALE_SDAYNAME3);
	weekdays[4] = getLocaleInfo(LOCALE_SDAYNAME4);
	weekdays[5] = getLocaleInfo(LOCALE_SDAYNAME5);
	weekdays[6] = getLocaleInfo(LOCALE_SDAYNAME6);

	shortMonths[0] = getLocaleInfo(LOCALE_SABBREVMONTHNAME1);
	shortMonths[1] = getLocaleInfo(LOCALE_SABBREVMONTHNAME2);
	shortMonths[2] = getLocaleInfo(LOCALE_SABBREVMONTHNAME3);
	shortMonths[3] = getLocaleInfo(LOCALE_SABBREVMONTHNAME4);
	shortMonths[4] = getLocaleInfo(LOCALE_SABBREVMONTHNAME5);
	shortMonths[5] = getLocaleInfo(LOCALE_SABBREVMONTHNAME6);
	shortMonths[6] = getLocaleInfo(LOCALE_SABBREVMONTHNAME7);
	shortMonths[7] = getLocaleInfo(LOCALE_SABBREVMONTHNAME8);
	shortMonths[8] = getLocaleInfo(LOCALE_SABBREVMONTHNAME9);
	shortMonths[9] = getLocaleInfo(LOCALE_SABBREVMONTHNAME10);
	shortMonths[10] = getLocaleInfo(LOCALE_SABBREVMONTHNAME11);
	shortMonths[11] = getLocaleInfo(LOCALE_SABBREVMONTHNAME12);

	months[0] = getLocaleInfo(LOCALE_SMONTHNAME1);
	months[1] = getLocaleInfo(LOCALE_SMONTHNAME2);
	months[2] = getLocaleInfo(LOCALE_SMONTHNAME3);
	months[3] = getLocaleInfo(LOCALE_SMONTHNAME4);
	months[4] = getLocaleInfo(LOCALE_SMONTHNAME5);
	months[5] = getLocaleInfo(LOCALE_SMONTHNAME6);
	months[6] = getLocaleInfo(LOCALE_SMONTHNAME7);
	months[7] = getLocaleInfo(LOCALE_SMONTHNAME8);
	months[8] = getLocaleInfo(LOCALE_SMONTHNAME9);
	months[9] = getLocaleInfo(LOCALE_SMONTHNAME10);
	months[10] = getLocaleInfo(LOCALE_SMONTHNAME11);
	months[11] = getLocaleInfo(LOCALE_SMONTHNAME12);
}

SystemInfo::~SystemInfo()
{
	if(usingCPU)
		endProcessorStatistics();

	WSACleanup();

	delete[] am;
	delete[] pm;

	int i;

	for(i = 0; i < 7; i++)
	{
		delete[] shortWeekdays[i];
		delete[] weekdays[i];
	}

	for(i = 0; i < 12; i++)
	{
		delete[] shortMonths[i];
		delete[] months[i];
	}
}

string SystemInfo::processLabelText(const string &text, Label *label, boolean *dynamic)
{
	string output;
	
	int length = text.length();
	int i = 0;
	
	if(dynamic) *dynamic = false;
	
	while(i < length)
	{
		if(text[i] == '[')
		{
			i++;
			
			if(i < length && text[i] == '[')
			{
				output.append(1, '[');
				i++;
				continue;
			}
			
			output.append(process(text, i, length, label, dynamic));

			if(i >= length || text[i++] != ']')
			{
				output = "[syntax error]";
				break;
			}

			/*
			string code;
			
			while(i < length && text[i] != ']')
			{
				code.append(1, text[i]);
				i++;
			}
			
			int index = 0;
			output.append(process(code, index, code.length(), dynamic));
			*/

			// if(i < length) i++;
			continue;
		}
		else if(text[i] == ']')
		{
			i++;
			
			if(i < length && text[i] == ']')
			{
				output.append(1, ']');
				i++;
				continue;
			}
		}
		else if(text[i] == '\\')
		{
			char ch;
			i++;

			if(i < length && text[i] == 'n')
			{
				ch = '\n';
			}
			else
			{
				ch = text[i];
			}

			output.append(1, ch);
			i++;
			continue;
		}
		
		output.append(1, text[i++]);
	}
	
	return output;
}

string SystemInfo::evaluateFunction(const string &functionName, const vector<string> &arguments, Label *label, boolean *dynamic = 0)
{
	string name = lowerCase(functionName);
	
	if(name == "activetask")
		return getActiveTask(dynamic);
	else if(name == "computername")
		return getComputerName(dynamic);
	else if(name == "cpu")
		return getCPU(dynamic);
	else if(name == "date")
		return getDate(arguments, dynamic);
	else if(name == "diskavailable" && arguments.size() >= 1)
		return getDiskAvailable(arguments[0], dynamic);
	else if(name == "diskinuse" && arguments.size() >= 1)
		return getDiskInUse(arguments[0], dynamic);
	else if(name == "disktotal" && arguments.size() >= 1)
		return getDiskTotal(arguments[0], dynamic);
	else if(name == "hostname")
		return getHostName(dynamic);
	else if(name == "ip")
		return getIP(dynamic);
	else if(name == "itime")
		return getInternetTime(dynamic);
	else if(name == "memavailable")
		return getMemAvailable(dynamic);
	else if(name == "meminuse")
		return getMemInUse(dynamic);
	else if(name == "memtotal")
		return getMemTotal(dynamic);
	else if(name == "os")
		return getOS(dynamic);
	else if(name == "randomline" && arguments.size() >= 1)
		return getRandomLine(arguments[0], dynamic);
	else if(name == "swapavailable")
		return getSwapAvailable(dynamic);
	else if(name == "swapinuse")
		return getSwapInUse(dynamic);
	else if(name == "swaptotal")
		return getSwapTotal(dynamic);
	else if(name == "time")
		return getTime(arguments, dynamic);
	else if(name == "uptime")
		return getUptime(arguments, dynamic);
	else if(name == "username")
		return getUserName(dynamic);
	else if(name == "winampsong")
		return getWinampSong(dynamic);
	else if(name == "winamptime")
		return getWinampTime(dynamic);
	else if(name == "winampremaintime")
		return getWinampRemainTime(dynamic);
	else if(name == "winamptotaltime")
		return getWinampTotalTime(dynamic);
	else if(name == "winampstatus")
		return getWinampStatus(dynamic);
	else if(name == "windowtitle" && arguments.size() >= 1)
		return getWindowTitle(arguments[0], dynamic);
	else if(name == "hideifempty" && arguments.size() >= 1)
		return hideIfEmpty(arguments[0], label, dynamic);
	else if(name == "after" && arguments.size() == 2)
		return after(arguments[0], arguments[1]);
	else if(name == "afterlast" && arguments.size() == 2)
		return afterLast(arguments[0], arguments[1]);
	else if(name == "before" && arguments.size() == 2)
		return before(arguments[0], arguments[1]);
	else if(name == "beforelast" && arguments.size() == 2)
		return beforeLast(arguments[0], arguments[1]);
	else if(name == "between" && arguments.size() == 3)
		return between(arguments[0], arguments[1], arguments[2]);
	else if(name == "capitalize")
		return capitalize(arguments[0]);
	else if(name == "lowercase")
		return lowerCase(arguments[0]);
	else if(name == "trim")
		return trim(arguments[0]);
	else if(name == "uppercase")
		return upperCase(arguments[0]);
	
	return "[?]";
}

string SystemInfo::process(const string &text, int &i, int length, Label *label, boolean *dynamic = 0)
{
	// skip whitespace
	while(i < length && isspace(text[i]))
		i++;
	
	// is it a string literal?
	if(i < length && (text[i] == '\"' || text[i] == '\''))
	{
		string literalValue;
		char quoteChar = text[i++];
		
		while(i < length && text[i] != quoteChar)
			literalValue.append(1, text[i++]);
		
		if(i < length) i++;
		return literalValue;
	}
	else // otherwise its a name
	{
		string name;
		vector<string> arguments;
		
		// retrieve name
		while(i < length && (isalnum(text[i]) || text[i] == '-'))
			name.append(1, text[i++]);
		
		// skip whitespace
		while(i < length && isspace(text[i]))
			i++;
		
		// has argument list?
		if(i < length && text[i] == '(')
		{
			// process each argument
			do
			{
				i++;

				// call recursively to handle
				arguments.push_back(process(text, i, length, label, dynamic));
				
				// skip whitespace
				while(i < length && isspace(text[i]))
					i++;
			}
			while(i < length && text[i] == ',');

			// must be a closing parenthesis
			if(i >= length || text[i++] != ')')
				return "[error]";
		}
		
		// get it
		return evaluateFunction(name, arguments, label, dynamic);
	}
}

string SystemInfo::getActiveTask(boolean *dynamic)
{
	char buffer[256] = { 0 };
	HWND activeTask = GetForegroundWindow();

	if(IsAppWindow(activeTask))
		GetWindowText(activeTask, buffer, 256);

	if(dynamic) *dynamic = true;
	return string(buffer);
}

string SystemInfo::getComputerName(boolean *dynamic)
{
	char buffer[32];
	unsigned long bufferLen = 32;

	GetComputerName(buffer, &bufferLen);
	return string(buffer);
}

string SystemInfo::getCPU(boolean *dynamic)
{
	if(!usingCPU)
	{
		startProcessorStatistics();
		usingCPU = true;
	}

	char buffer[16];
	sprintf(buffer, "%d%%", getProcessorUsage());

	if(dynamic) *dynamic = true;
	return string(buffer);
}

string SystemInfo::getDate(const vector<string> &arguments, boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	SYSTEMTIME st;
	getLocalizedTime(&st, arguments.size() >= 2 ? arguments[1] : "");

	if(arguments.size() >= 1)
	{
		// SYSTEMTIME st;
		// GetLocalTime(&st);
		return formatDateTime(arguments[0], st, 0);
	}
	else
	{
		char buffer[64];
		GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, 0, buffer, 64);
		return string(buffer);
	}
}

string SystemInfo::getInternetTime(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	SYSTEMTIME st;
	GetSystemTime(&st);

	int seconds = (((st.wHour + 1) * 60) + st.wMinute) * 60 + st.wSecond;
	int beats = (1000 * seconds) / 86400;

	char output[8];
	sprintf(output, "@%03d", beats);
	return string(output);
}

string SystemInfo::getDiskAvailable(const string &drive, boolean *dynamic)
{
	largeInt freeBytes;
	largeInt totalBytes;

	getDiskFreeSpace(drive, freeBytes, totalBytes);
	
	if(dynamic) *dynamic = true;
	return formatByteSize(freeBytes);
}

string SystemInfo::getDiskInUse(const string &drive, boolean *dynamic)
{
	largeInt freeBytes;
	largeInt totalBytes;

	getDiskFreeSpace(drive, freeBytes, totalBytes);
	
	if(dynamic) *dynamic = true;
	return formatByteSize(totalBytes - freeBytes);
}

string SystemInfo::getDiskTotal(const string &drive, boolean *dynamic)
{
	largeInt freeBytes;
	largeInt totalBytes;

	getDiskFreeSpace(drive, freeBytes, totalBytes);
	
	if(dynamic) *dynamic = true;
	return formatByteSize(totalBytes);
}

string SystemInfo::getHostName(boolean *dynamic)
{
	char hostName[256];
	gethostname(hostName, 256);
	hostent *hostInfo = gethostbyname(hostName);

	if(hostInfo)
		return string(hostInfo->h_name);
	
	return "";
}

string SystemInfo::getIP(boolean *dynamic)
{
	char hostName[256];
	gethostname(hostName, 256);
	hostent *hostInfo = gethostbyname(hostName);

	if(hostInfo)
		return string(inet_ntoa(*((in_addr *) hostInfo->h_addr)));
	
	return "";
}

string SystemInfo::getMemAvailable(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwAvailPhys);
}

string SystemInfo::getMemInUse(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPhys - ms.dwAvailPhys);
}

string SystemInfo::getMemTotal(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPhys);
}

string SystemInfo::getOS(boolean *dynamic)
{
	OSVERSIONINFO verInfo;
	verInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&verInfo);

	if(verInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		if(verInfo.dwMinorVersion >= 90)
			return "Windows ME";
		else if(verInfo.dwMinorVersion >= 10)
			return "Windows 98";
		else
			return "Windows 95";
	}
	else
	{
		if(verInfo.dwMajorVersion >= 5 && verInfo.dwMinorVersion >= 1)
			return "Windows XP";
		else if(verInfo.dwMajorVersion >= 5)
			return "Windows 2000";
		else
			return "Windows NT";
	}
}

string SystemInfo::getRandomLine(const string &file, boolean *dynamic)
{
	FILE *fp = fopen(file.c_str(), "r");
	int lineCount = 0;
	char lineBuffer[1024];

	if(fp)
	{
		while(fgets(lineBuffer, 1024, fp))
		{
			if(lineBuffer[0] != '\n' && lineBuffer[0] != 0)
				lineCount++;
		}

		srand(GetTickCount());
		int line = rand() % lineCount;

		lineCount = 0;
		fseek(fp, 0, SEEK_SET);

		while(fgets(lineBuffer, 1024, fp))
		{
			if(lineBuffer[0] != '\n' && lineBuffer[0] != 0)
			{
				if(lineCount == line)
				{
					int length = strlen(lineBuffer);
					if(lineBuffer[length - 1] == '\n') lineBuffer[length - 1] = 0;
					fclose(fp);
					return string(lineBuffer);
				}

				lineCount++;
			}
		}

		fclose(fp);
		return "[error]";
	}

	return "[Could not open file!]";
}

string SystemInfo::getSwapAvailable(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwAvailPageFile);
}

string SystemInfo::getSwapInUse(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPageFile - ms.dwAvailPageFile);
}

string SystemInfo::getSwapTotal(boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPageFile);
}

string SystemInfo::getTime(const vector<string> &arguments, boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	SYSTEMTIME st;
	getLocalizedTime(&st, arguments.size() >= 2 ? arguments[1] : "");

	if(arguments.size() >= 1)
	{
		// SYSTEMTIME st;
		// GetLocalTime(&st);
		return formatDateTime(arguments[0], st, 0);
	}
	else
	{
		char buffer[64];
		GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, 0, buffer, 64);
		return string(buffer);
	}
}

string SystemInfo::getUptime(const vector<string> &arguments, boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	int ms = GetTickCount();
	int seconds = ms / 1000;
	ms %= 1000;
	int minutes = seconds / 60;
	seconds %= 60;
	int hours = minutes / 60;
	minutes %= 60;
	int days = hours / 24;
	hours %= 24;

	SYSTEMTIME st;

	st.wMonth = -1;
	st.wDay = days;
	st.wDayOfWeek = -1;
	st.wYear = 0;
	st.wHour = hours;
	st.wMinute = minutes;
	st.wSecond = seconds;
	st.wMilliseconds = ms;

	string format = (arguments.size() >= 1) ? arguments[0] : "d days hh:nn:ss";
	return formatDateTime(format, st, 1);
}

string SystemInfo::getUserName(boolean *dynamic)
{
	char buffer[32];
	unsigned long bufferLen = 32;

	GetUserName(buffer, &bufferLen);
	return string(buffer);
}

string SystemInfo::getWinampSong(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWinamp = FindWindow("Winamp v1.x", 0);
	if(!hWinamp) return "";

	char buffer[256];
	GetWindowText(hWinamp, buffer, 256);

	if(strstr(buffer, " - Winamp") == 0)
		return "";

	return trim(between(string(buffer), ".", "-"));
}

string SystemInfo::getWinampTime(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWinamp = FindWindow("Winamp v1.x", 0);
	if(!hWinamp) return "0:00";

	int seconds = SendMessage(hWinamp, WM_USER, 0, 105) / 1000;
	int minutes = seconds / 60;
	seconds %= 60;

	char buffer[32];
	wsprintf(buffer, "%d:%02d", minutes, seconds);
	return string(buffer);
}

string SystemInfo::getWinampRemainTime(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWinamp = FindWindow("Winamp v1.x", 0);
	if(!hWinamp) return "0:00";

	int seconds = SendMessage(hWinamp, WM_USER, 1, 105) - SendMessage(hWinamp, WM_USER, 0, 105) / 1000;
	int minutes = seconds / 60;
	seconds %= 60;

	char buffer[32];
	wsprintf(buffer, "%d:%02d", minutes, seconds);
	return string(buffer);
}

string SystemInfo::getWinampTotalTime(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWinamp = FindWindow("Winamp v1.x", 0);
	if(!hWinamp) return "0:00";

	int seconds = SendMessage(hWinamp, WM_USER, 1, 105);
	int minutes = seconds / 60;
	seconds %= 60;

	char buffer[32];
	wsprintf(buffer, "%d:%02d", minutes, seconds);
	return string(buffer);
}

string SystemInfo::getWinampStatus(boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWinamp = FindWindow("Winamp v1.x", 0);
	if(!hWinamp) return "";

	int status = SendMessage(hWinamp, WM_USER, 0, 104);

	if(status == 1) {
		return "Playing";
	}
	else if(status == 3) {
		return "Paused";
	}
	else {
		return "Stopped";
	}
}

string SystemInfo::getWindowTitle(const string &windowClass, boolean *dynamic)
{
	if(dynamic) *dynamic = true;

	HWND hWnd = FindWindow(windowClass.c_str(), 0);
	if(!hWnd) return "";

	char buffer[256];
	GetWindowText(hWnd, buffer, 256);
	return string(buffer);
}

string SystemInfo::hideIfEmpty(const string &s, Label *label, boolean *dynamic)
{
	if(label)
	{
		if(s.empty() && label->isVisible())
			label->hide();
		else if(!s.empty() && !label->isVisible())
			label->show();
	}
	
	return s;
}

string SystemInfo::after(const string &s, const string &search)
{
	int i = s.find(search);
	i = (i < 0) ? 0 : i + search.length();
	return s.substr(i, s.length() - i);
}

string SystemInfo::afterLast(const string &s, const string &search)
{
	int i = s.rfind(search);
	i = (i < 0) ? 0 : i + search.length();
	return s.substr(i, s.length() - i);
}

string SystemInfo::before(const string &s, const string &search)
{
	int i = s.find(search);
	i = (i < 0) ? s.length() : i;
	return s.substr(0, i);
}

string SystemInfo::beforeLast(const string &s, const string &search)
{
	int i = s.rfind(search);
	i = (i < 0) ? s.length() : i;
	return s.substr(0, i);
}

string SystemInfo::between(const string &s, const string &left, const string &right)
{
	int start = s.find(left);
	start = (start < 0) ? 0 : start + left.length();

	int end = s.rfind(right);
	end = (end < 0) ? s.length() - 1 : end - right.length();

	return (start > end) ? "" : s.substr(start, end - start + 1);
}

string SystemInfo::capitalize(const string &s)
{
	string result;
	
	int length = s.length();
	int i = 0;
	
	while(i < length)
		result.append(1, (i == 0) ? toupper(s[i++]) : tolower(s[i++]));
	
	return result;
}

string SystemInfo::lowerCase(const string &s)
{
	string result;
	
	int length = s.length();
	int i = 0;
	
	while(i < length)
		result.append(1, tolower(s[i++]));
	
	return result;
}

string SystemInfo::trim(const string &s)
{
	int start = 0;
	int end = s.length() - 1;
	
	while(start <= end && isspace(s[start]))
		start++;
	
	while(start <= end && isspace(s[end]))
		end--;
	
	return s.substr(start, end - start + 1);
}

string SystemInfo::upperCase(const string &s)
{
	string result;
	
	int length = s.length();
	int i = 0;
	
	while(i < length)
		result.append(1, toupper(s[i++]));
	
	return result;
}

void SystemInfo::getDiskFreeSpace(const string &drive, largeInt &freeBytes, largeInt &totalBytes)
{
	static BOOL (WINAPI *pfnGetDiskFreeSpaceEx)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER);
	static BOOL fChecked = FALSE;

	if(!fChecked)
	{
		pfnGetDiskFreeSpaceEx = (BOOL (WINAPI *)(LPCSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER)) GetProcAddress(
			GetModuleHandle("KERNEL32.DLL"), "GetDiskFreeSpaceExA");

		fChecked = TRUE;
	}

	if(pfnGetDiskFreeSpaceEx)
	{
		largeInt dummy;

		pfnGetDiskFreeSpaceEx(drive.c_str(),
			(PULARGE_INTEGER) &freeBytes,
			(PULARGE_INTEGER) &totalBytes,
			(PULARGE_INTEGER) &dummy);
	}
	else
	{
		DWORD sectorsPerCluster;
		DWORD bytesPerSector;
		DWORD freeClusters;
		DWORD totalClusters;

		GetDiskFreeSpace(drive.c_str(),
			&sectorsPerCluster,
			&bytesPerSector,
			&freeClusters,
			&totalClusters);

		largeInt bytesPerCluster = (largeInt) bytesPerSector * (largeInt) sectorsPerCluster;

		freeBytes = (largeInt) freeClusters * bytesPerCluster;
		totalBytes = (largeInt) totalClusters * bytesPerCluster;
	}
}

static const char *units[] = {
	"bytes", "KB", "MB", "GB", "TB", "PB", "EB"
};

string SystemInfo::formatByteSize(largeInt byteSize)
{
	int i = 0;
	
	if(byteSize & 0xF000000000000000) i++;
	if(byteSize & 0xFFFC000000000000) i++;
	if(byteSize & 0xFFFFFF0000000000) i++;
	if(byteSize & 0xFFFFFFFFC0000000) i++;
	if(byteSize & 0xFFFFFFFFFFF00000) i++;
	if(byteSize & 0xFFFFFFFFFFFFFC00) i++;
	
	largeInt divisor = 1 << (i * 10);
	largeInt quotient = byteSize / divisor;
	largeInt remainder = ((byteSize % divisor) * 100) / divisor;

	if(quotient > 9 && quotient < 100)
		remainder = remainder / 10;
	else if(quotient >= 100)
		remainder = 0;

	char buffer[32];

	if(remainder > 0)
		wsprintf(buffer, "%d.%02d %s", (int) quotient, (int) remainder, units[i]);
	else
		wsprintf(buffer, "%d %s", (int) quotient, units[i]);

	return string(buffer);
}

static inline int isSequenceChar(int ch)
{
	return (ch == 'd' || ch == 'm' || ch == 'y' || ch == 'h' || ch == 'i' || ch == 'n' || ch == 's');
}

string SystemInfo::formatDateTime(const string &format, const SYSTEMTIME &st, int span)
{
	const char *formatPtr = format.c_str();
	string output;
	int lastChar = 0;
	int sequenceChar = 0;
	int count = 0;
	int ch = *formatPtr++;
	
	while(ch || sequenceChar)
	{
		if(sequenceChar)
		{
			if(ch == sequenceChar)
			{
				count++;
			}
			else if(isalpha(ch))
			{
				output.append(count, sequenceChar);
				output.append(1, ch);
				
				sequenceChar = 0;
				count = 0;
			}
			else
			{
				switch(sequenceChar)
				{
					case 'd':
					{
						switch(count)
						{
							case 1:
							{
								char temp[8];
								sprintf(temp, "%d", st.wDay);
								output.append(temp);
								break;
							}
							
							case 2:
							{
								char temp[8];
								sprintf(temp, "%02d", st.wDay);
								output.append(temp);
								break;
							}
							
							case 3:
							{
								if(st.wDayOfWeek < 7)
									output.append(shortWeekdays[st.wDayOfWeek]);
								
								break;
							}
							
							case 4:
							{
								if(st.wDayOfWeek < 7)
									output.append(weekdays[st.wDayOfWeek]);
									
								break;
							}
							
							default:
							{
								output.append(count, sequenceChar);
								break;
							}
						}
						
						break;
					}
					
					case 'm':
					{
						if(span) break;

						switch(count)
						{
							case 1:
							{
								char temp[8];
								sprintf(temp, "%d", st.wMonth);
								output.append(temp);
								break;
							}
							
							case 2:
							{
								char temp[8];
								sprintf(temp, "%02d", st.wMonth);
								output.append(temp);
								break;
							}
							
							case 3:
							{
								if(st.wMonth > 0 && st.wMonth < 13)
									output.append(shortMonths[st.wMonth - 1]);
								
								break;
							}
							
							case 4:
							{
								if(st.wMonth > 0 && st.wMonth < 13)
									output.append(months[st.wMonth - 1]);
								
								break;
							}
							
							default:
							{
								output.append(count, sequenceChar);
								break;
							}
						}
						
						break;
					}
					
					case 'y':
					{
						if(span) break;

						if(count == 2 || count == 4)
						{
							char temp[8];
							sprintf(temp, "%04d", st.wYear);
							output.append((count == 2) ? temp + 2 : temp);
						}
						else
						{
							output.append(count, sequenceChar);
						}
						
						break;
					}
					
					case 'h':
					{
						if(count == 1 || count == 2)
						{
							char temp[8];
							int hour = span ? st.wHour : (st.wHour == 0) ? 12 : (st.wHour > 12) ? st.wHour - 12 : st.wHour;
							sprintf(temp, (count == 1) ? "%d" : "%02d", hour);
							output.append(temp);
						}
						else
						{
							output.append(count, sequenceChar);
						}
						
						break;
					}
					
					case 'i':
					{
						if(count == 1 || count == 2)
						{
							char temp[8];
							sprintf(temp, (count == 1) ? "%d" : "%02d", st.wHour);
							output.append(temp);
						}
						else
						{
							output.append(count, sequenceChar);
						}
						
						break;
					}
					
					case 'n':
					{
						if(count == 1 || count == 2)
						{
							char temp[8];
							sprintf(temp, (count == 1) ? "%d" : "%02d", st.wMinute);
							output.append(temp);
						}
						else
						{
							output.append(count, sequenceChar);
						}
						
						break;
					}
					
					case 's':
					{
						if(count == 1 || count == 2)
						{
							char temp[8];
							sprintf(temp, (count == 1) ? "%d" : "%02d", st.wSecond);
							output.append(temp);
						}
						else
						{
							output.append(count, sequenceChar);
						}
						
						break;
					}
					
					default:
					{
						output.append(count, sequenceChar);
						break;
					}
				}
				
				if(ch) output.append(1, ch);
				sequenceChar = 0;
				count = 0;
			}
		}
		else
		{
			if(isSequenceChar(ch) && !isalpha(lastChar))
			{
				sequenceChar = ch;
				count = 1;
			}
			else if(ch == 'a' && !isalpha(lastChar))
			{
				const char *p = formatPtr - 1;
				
				// special case: am/pm
				if(strncmp(p, "am/pm", 5) == 0)
				{
					output.append((st.wHour >= 12) ? pm : am);
					formatPtr = formatPtr + 4;
				}
				else
				{
					output.append(1, ch);
				}
			}
			else
			{
				output.append(1, ch);
			}
		}
		
		lastChar = ch;
		if(ch) ch = *formatPtr++;
	}
	
	return output;
}

void SystemInfo::getLocalizedTime(SYSTEMTIME *pst, const string &timezone)
{
	string temp = lowerCase(timezone);
	const char *s = temp.c_str();

	if(s[0] == 'u' && s[1] == 't' && s[2] == 'c')
	{
		s = s + 3;
		GetSystemTime(pst);
	}
	else
	{
		GetLocalTime(pst);
	}

	while(s[0] == ' ')
		s++;

	int dHour = 0;
	int dMinute = 0;
	int sign = 1;

	if(s[0] == '+')
	{
		sign = 1;
		s++;
	}
	else if(s[0] == '-')
	{
		sign = -1;
		s++;
	}

	while(s[0] >= '0' && s[0] <= '9')
	{
		dHour = (dHour * 10) + (s[0] - '0');
		s++;
	}

	if(s[0] == ':')
	{
		s++;

		while(s[0] >= '0' && s[0] <= '9')
		{
			dMinute = (dMinute * 10) + (s[0] - '0');
			s++;
		}
	}

	largeInt base;
	largeInt delta = (largeInt) (sign * ((dHour * 60) + dMinute)) * 600000000;

	SystemTimeToFileTime(pst, (LPFILETIME) &base);
	base = base + delta;
	FileTimeToSystemTime((LPFILETIME) &base, pst);
}

char *SystemInfo::getLocaleInfo(LCTYPE type)
{
	int len = GetLocaleInfo(LOCALE_USER_DEFAULT, type, 0, 0);
	char *buffer = new char[len];
	GetLocaleInfo(LOCALE_USER_DEFAULT, type, buffer, len);
	return buffer;
}

SystemInfo *systemInfo;
