#include "common.h"
#include "processor.h"
#include "SystemInfo.h"

SystemInfo::SystemInfo()
{
	usingCPU = false;
}

SystemInfo::~SystemInfo()
{
	if(usingCPU)
		endProcessorStatistics();
}

string SystemInfo::processLabelText(const string &text, boolean *dynamic)
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
			
			string code;
			
			while(i < length && text[i] != ']')
			{
				code.append(1, text[i]);
				i++;
			}
			
			int index = 0;
			output.append(process(code, index, code.length(), dynamic));

			if(i < length) i++;			
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
		
		output.append(1, text[i++]);
	}
	
	return output;
}

string SystemInfo::evaluateFunction(const string &functionName, const string &argument, boolean *dynamic = 0)
{
	string name = lowerCase(functionName);
	
	if(name == "activetask")
		return getActiveTask(argument, dynamic);
	else if(name == "computername")
		return getComputerName(argument, dynamic);
	else if(name == "cpu")
		return getCPU(argument, dynamic);
	else if(name == "date")
		return getDate(argument, dynamic);
	else if(name == "memavailable")
		return getMemAvailable(argument, dynamic);
	else if(name == "meminuse")
		return getMemInUse(argument, dynamic);
	else if(name == "memtotal")
		return getMemTotal(argument, dynamic);
	else if(name == "os")
		return getOS(argument, dynamic);
	else if(name == "swapavailable")
		return getSwapAvailable(argument, dynamic);
	else if(name == "swapinuse")
		return getSwapInUse(argument, dynamic);
	else if(name == "swaptotal")
		return getSwapTotal(argument, dynamic);
	else if(name == "time")
		return getTime(argument, dynamic);
	else if(name == "username")
		return getUserName(argument, dynamic);
	else if(name == "lowercase")
		return lowerCase(argument);
	else if(name == "uppercase")
		return upperCase(argument);
	
	return "[?]";
}

string SystemInfo::process(const string &text, int &i, int length, boolean *dynamic = 0)
{
	// skip whitespace
	while(i < length && isspace(text[i]))
		i++;
	
	// is name or string literal?
	if(i < length && (text[i] == '\"' || text[i] == '\''))
	{
		string literalValue;
		char quoteChar = text[i++];
		
		while(i < length && text[i] != quoteChar)
			literalValue.append(1, text[i++]);
		
		if(i < length) i++;
		return literalValue;
	}
	else
	{
		string name;
		string argument;
		
		// retrieve name
		while(i < length && isalnum(text[i]))
			name.append(1, text[i++]);
		
		// skip whitespace
		while(i < length && isspace(text[i]))
			i++;
		
		// has argument list?
		if(i < length && text[i] == '(')
		{
			i++;
			
			// call recursively to handle
			argument = process(text, i, length, dynamic);
			
			// skip whitespace
			while(i < length && isspace(text[i]))
				i++;
			
			// must be a closing parenthesis
			if(i >= length || text[i] != ')')
				return "[error]";
		}
		
		// get it
		return evaluateFunction(name, argument, dynamic);
	}
}

string SystemInfo::getActiveTask(const string &argument, boolean *dynamic)
{
	char buffer[256] = { 0 };
	HWND activeTask = GetForegroundWindow();

	if(IsAppWindow(activeTask))
		GetWindowText(activeTask, buffer, 256);

	if(dynamic) *dynamic = true;
	return string(buffer);
}

string SystemInfo::getComputerName(const string &argument, boolean *dynamic)
{
	char buffer[32];
	unsigned long bufferLen = 32;

	GetComputerName(buffer, &bufferLen);
	return string(buffer);
}

string SystemInfo::getCPU(const string &argument, boolean *dynamic)
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

string SystemInfo::getDate(const string &argument, boolean *dynamic)
{
	char buffer[64];
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, 0, 0, buffer, 64);

	if(dynamic) *dynamic = true;
	return string(buffer);
}

string SystemInfo::getMemAvailable(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwAvailPhys);
}

string SystemInfo::getMemInUse(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPhys - ms.dwAvailPhys);
}

string SystemInfo::getMemTotal(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPhys);
}

string SystemInfo::getOS(const string &argument, boolean *dynamic)
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
		if(verInfo.dwMajorVersion >= 5)
			return "Windows 2000";
		else
			return "Windows NT";
	}
}

string SystemInfo::getSwapAvailable(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwAvailPageFile);
}

string SystemInfo::getSwapInUse(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPageFile - ms.dwAvailPageFile);
}

string SystemInfo::getSwapTotal(const string &argument, boolean *dynamic)
{
	MEMORYSTATUS ms;
	ms.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);

	if(dynamic) *dynamic = true;
	return formatByteSize(ms.dwTotalPageFile);
}

string SystemInfo::getTime(const string &argument, boolean *dynamic)
{
	char buffer[64];
	GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, 0, 0, buffer, 64);

	if(dynamic) *dynamic = true;
	return string(buffer);
}

string SystemInfo::getUserName(const string &argument, boolean *dynamic)
{
	char buffer[32];
	unsigned long bufferLen = 32;

	GetUserName(buffer, &bufferLen);
	return string(buffer);
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
		wsprintf(buffer, "%d.%d %s", (int) quotient, (int) remainder, units[i]);
	else
		wsprintf(buffer, "%d %s", (int) quotient, units[i]);

	return string(buffer);
}

string SystemInfo::lowerCase(const string &aString)
{
	string resultString;
	
	int length = aString.length();
	int i = 0;
	
	while(i < length)
		resultString.append(1, tolower(aString[i++]));
	
	return resultString;
}

string SystemInfo::upperCase(const string &aString)
{
	string resultString;
	
	int length = aString.length();
	int i = 0;
	
	while(i < length)
		resultString.append(1, toupper(aString[i++]));
	
	return resultString;
}

SystemInfo *systemInfo;
