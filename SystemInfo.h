#if !defined(__SYSTEMINFO_H)
#define __SYSTEMINFO_H

class Label;

class SystemInfo
{
public:

	SystemInfo();
	virtual ~SystemInfo();

	string processLabelText(const string &text, Label *label, boolean *dynamic = 0);

private:

	boolean usingCPU;

	string evaluateFunction(const string &functionName, const vector<string> &arguments, Label *label, boolean *dynamic);
	string process(const string &text, int &i, int length, Label *label, boolean *dynamic);

	// date and time info
	string getDate(const vector<string> &arguments, boolean *dynamic);
	string getInternetTime(boolean *dynamic);
	string getTime(const vector<string> &arguments, boolean *dynamic);
	string getUptime(const vector<string> &arguments, boolean *dynamic);

	// memory and disk space info
	string getDiskAvailable(const vector<string> &arguments, boolean *dynamic);
	string getDiskInUse(const vector<string> &arguments, boolean *dynamic);
	string getDiskTotal(const vector<string> &arguments, boolean *dynamic);
	string getMemAvailable(const vector<string> &arguments, boolean *dynamic);
	string getMemInUse(const vector<string> &arguments, boolean *dynamic);
	string getMemTotal(const vector<string> &arguments, boolean *dynamic);
	string getSwapAvailable(const vector<string> &arguments, boolean *dynamic);
	string getSwapInUse(const vector<string> &arguments, boolean *dynamic);
	string getSwapTotal(const vector<string> &arguments, boolean *dynamic);

	// power info
	string getPowerSource(const vector<string> &arguments, boolean *dynamic);
	string getBattery(boolean *dynamic);

	// network-related info
	string getComputerName(boolean *dynamic);
	string getHostName(boolean *dynamic);
	string getIP(const vector<string> &arguments, boolean *dynamic);
	string getUserName(boolean *dynamic);

	// Winamp info
	string getWinampSong(boolean *dynamic);
	string getWinampTime(boolean *dynamic);
	string getWinampRemainTime(boolean *dynamic);
	string getWinampTotalTime(boolean *dynamic);
	string getWinampStatus(boolean *dynamic);

	// MotherBoard Monitor 5 (MBM5) info
	string getMBMCPUUsage(const vector<string> &arguments, boolean *dynamic);
	string getMBMFanSpeed(const vector<string> &arguments, boolean *dynamic);
	string getMBMTemperature(const vector<string> &arguments, boolean *dynamic, boolean &bUseFahrenheit);
	string getMBMVoltage(const vector<string> &arguments, boolean *dynamic);

	// miscellaneous info
	string getActiveTask(boolean *dynamic);
	string getCPU(boolean *dynamic);
	string getOS(boolean *dynamic);
	string getRandomLine(const string &file, boolean *dynamic);
	string getWindowTitle(const string &windowClass, boolean *dynamic);

	// formatting functions
	string capitalize(const string &s);
	string lowerCase(const string &s);
	string after(const string &s, const string &search);
	string afterLast(const string &s, const string &search);
	string before(const string &s, const string &search);
	string beforeLast(const string &s, const string &search);
	string between(const string &s, const string &left, const string &right);
	string trim(const string &s);
	string upperCase(const string &s);

	// miscellaneous functions
	string hideIfEmpty(const string &s, Label *label, boolean *dynamic);

	void getDiskFreeSpace(const string &drive, largeInt &freeBytes, largeInt &totalBytes);

	string formatByteSize(largeInt byteSize, largeInt total, int units);
	string formatDateTime(const string &format, const SYSTEMTIME &st, int span);
	void getLocalizedTime(SYSTEMTIME *pst, const string &timezone);

	char *getLocaleInfo(LCTYPE type);

private:

	char *am;
	char *pm;

	char *shortWeekdays[7];
	char *weekdays[7];
	char *shortMonths[12];
	char *months[12];
};

extern SystemInfo *systemInfo;

#endif
