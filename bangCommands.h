#if !defined(__BANGCOMMANDS_H)
#define __BANGCOMMANDS_H

void AddBangCommands(const string &labelName);
void RemoveBangCommands(const string &labelName);

void CreateLabelBangCommand(HWND caller, const char *arguments);
void DebugBangCommand(HWND caller, const char *arguments);

#endif
