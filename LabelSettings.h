#if !defined(__LABELSETTINGS_H)
#define __LABELSETTINGS_H

class LabelSettings
{
private:

	boolean isDefault;

public:

	LabelSettings();
	LabelSettings(const char *name);
	virtual ~LabelSettings();

	boolean alwaysOnTop;
	boolean startHidden;
	boolean bUseFahrenheit;

	Texture *skin;
	Font *font;

	int leftBorder;
	int topBorder;
	int rightBorder;
	int bottomBorder;

	int bangCommands;
	int justify;
	int updateInterval;

	int width;
	int height;
	int x;
	int y;

	int scrollInterval;
	int scrollSpeed;
	int scrollPadLength;
	boolean scroll;

//	boolean trueTransparency;

	string text;

	string leftClickCommand;
	string leftDoubleClickCommand;
	string middleClickCommand;
	string middleDoubleClickCommand;
	string rightClickCommand;
	string rightDoubleClickCommand;
	string wheelDownCommand;
	string wheelUpCommand;
	string enterCommand;
	string leaveCommand;
	string dropCommand;
};

extern LabelSettings* defaultSettings;

#endif
