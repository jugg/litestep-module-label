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
	string getDiskAvailable(const string &drive, boolean *dynamic);
	string getDiskInUse(const string &drive, boolean *dynamic);
	string getDiskTotal(const string &drive, boolean *dynamic);
	string getMemAvailable(boolean *dynamic);
	string getMemInUse(boolean *dynamic);
	string getMemTotal(boolean *dynamic);
	string getSwapAvailable(boolean *dynamic);
	string getSwapInUse(boolean *dynamic);
	string getSwapTotal(boolean *dynamic);

	// network-related info
	string getComputerName(boolean *dynamic);
	string getHostName(boolean *dynamic);
	string getIP(boolean *dynamic);
	string getUserName(boolean *dynamic);

	// winamp info
	string getWinampSong(boolean *dynamic);
	string getWinampTime(boolean *dynamic);
	string getWinampRemainTime(boolean *dynamic);
	string getWinampTotalTime(boolean *dynamic);
	string getWinampStatus(boolean *dynamic);

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

	string formatByteSize(largeInt byteSize);
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
