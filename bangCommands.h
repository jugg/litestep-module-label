#if !defined(__BANGCOMMANDS_H)
#define __BANGCOMMANDS_H

struct bangcmddef {
	const char *Name;
	BangCommandEx *Command;
};

void AddBangCommands(const string &labelName, int &bangs);
void RemoveBangCommands(const string &labelName, int &bangs);

void CreateLabelBangCommand(HWND caller, const char *arguments);
//LsBox Support - blkhawk
void LsBoxHookBangCommand(HWND caller, const char *arguments);
void DebugBangCommand(HWND caller, const char *arguments);

#endif
