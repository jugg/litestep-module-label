#if !defined(__SYSTEMINFO_H)
#define __SYSTEMINFO_H

class SystemInfo
{
public:

	SystemInfo();
	virtual ~SystemInfo();

	string processLabelText(const string &text, boolean *dynamic = 0);

private:

	boolean usingCPU;

	string evaluateFunction(const string &functionName, const string &argument, boolean *dynamic);
	string process(const string &text, int &i, int length, boolean *dynamic);

	string getActiveTask(const string &argument, boolean *dynamic);
	string getComputerName(const string &argument, boolean *dynamic);
	string getCPU(const string &argument, boolean *dynamic);
	string getDate(const string &argument, boolean *dynamic);
	string getMemAvailable(const string &argument, boolean *dynamic);
	string getMemInUse(const string &argument, boolean *dynamic);
	string getMemTotal(const string &argument, boolean *dynamic);
	string getOS(const string &argument, boolean *dynamic);
	string getSwapAvailable(const string &argument, boolean *dynamic);
	string getSwapInUse(const string &argument, boolean *dynamic);
	string getSwapTotal(const string &argument, boolean *dynamic);
	string getTime(const string &argument, boolean *dynamic);
	string getUserName(const string &argument, boolean *dynamic);

	string formatByteSize(largeInt byteSize);

	string lowerCase(const string &aString);
	string upperCase(const string &aString);
};

extern SystemInfo *systemInfo;

#endif