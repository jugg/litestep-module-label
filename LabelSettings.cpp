#include "common.h"
#include "LabelSettings.h"
#include "Font.h"
#include "Texture.h"

NameValuePair justifyValues[] = {
	{ "left", DT_LEFT },
	{ "center", DT_CENTER },
	{ "right", DT_RIGHT },
	{ 0, 0 }
};

NameValuePair bangCommandValues[] = {
	{ "none", 0 },
	{ "minimal", 1 },
	{ "all", 5 },
	{ 0, 0 }
};

LabelSettings::LabelSettings()
{
	const char *name = "AllLabels";
	isDefault = true;

	alwaysOnTop = GetRCBoolean(name, "AlwaysOnTop");
	startHidden = GetRCBoolean(name, "StartHidden");
	bUseFahrenheit = GetRCBoolean(name, "UseFahrenheit");

	skin = GetRCTexture(name, "");
	font = GetRCFont(name, "");
	
	leftBorder = GetRCInt(name, "LeftBorder", 0);
	topBorder = GetRCInt(name, "TopBorder", 0);
	rightBorder = GetRCInt(name, "RightBorder", 0);
	bottomBorder = GetRCInt(name, "BottomBorder", 0);

	bangCommands = GetRCNamedValue(name, "BangCommands", bangCommandValues, 5);

	justify = GetRCNamedValue(name, "Justify", justifyValues, DT_CENTER);
	updateInterval = GetRCInt(name, "UpdateInterval", 1000);

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	width = GetRCDimension(name, "Width", 64, screenX);
	height = GetRCDimension(name, "Height", 64, screenY);
	x = GetRCCoordinate(name, "X", 0, screenX);
	y = GetRCCoordinate(name, "Y", 0, screenY);

	text = GetRCString(name, "Text", "");

	leftClickCommand = GetRCLine(name, "OnLeftClick", "");
	leftDoubleClickCommand = GetRCLine(name, "OnLeftDoubleClick", "");
	middleClickCommand = GetRCLine(name, "OnMiddleClick", "");
	middleDoubleClickCommand = GetRCLine(name, "OnMiddleDoubleClick", "");
	rightClickCommand = GetRCLine(name, "OnRightClick", "");
	rightDoubleClickCommand = GetRCLine(name, "OnRightDoubleClick", "");
	enterCommand = GetRCLine(name, "OnMouseEnter", "");
	leaveCommand = GetRCLine(name, "OnMouseLeave", "");
}

LabelSettings::LabelSettings(const char *name)
{
	isDefault = false;

	alwaysOnTop = GetRCBoolean(name, "AlwaysOnTop", defaultSettings.alwaysOnTop);
	startHidden = GetRCBoolean(name, "StartHidden", defaultSettings.startHidden);
	bUseFahrenheit = GetRCBoolean(name, "UseFahrenheit", defaultSettings.bUseFahrenheit);

	skin = GetRCTexture(name, "", defaultSettings.skin);
	font = GetRCFont(name, "", defaultSettings.font);
	
	leftBorder = GetRCInt(name, "LeftBorder", defaultSettings.leftBorder);
	topBorder = GetRCInt(name, "TopBorder", defaultSettings.topBorder);
	rightBorder = GetRCInt(name, "RightBorder", defaultSettings.rightBorder);
	bottomBorder = GetRCInt(name, "BottomBorder", defaultSettings.bottomBorder);

	bangCommands = GetRCNamedValue(name, "BangCommands", bangCommandValues, defaultSettings.bangCommands);

	justify = GetRCNamedValue(name, "Justify", justifyValues, defaultSettings.justify);
	updateInterval = GetRCInt(name, "UpdateInterval", defaultSettings.updateInterval);

	int screenX = GetSystemMetrics(SM_CXSCREEN);
	int screenY = GetSystemMetrics(SM_CYSCREEN);

	width = GetRCDimension(name, "Width", defaultSettings.width, screenX);
	height = GetRCDimension(name, "Height", defaultSettings.height, screenY);
	x = GetRCCoordinate(name, "X", defaultSettings.x, screenX);
	y = GetRCCoordinate(name, "Y", defaultSettings.y, screenY);

	text = GetRCString(name, "Text", defaultSettings.text);

	leftClickCommand = GetRCLine(name, "OnLeftClick", defaultSettings.leftClickCommand);
	leftDoubleClickCommand = GetRCLine(name, "OnLeftDoubleClick", defaultSettings.leftDoubleClickCommand);
	middleClickCommand = GetRCLine(name, "OnMiddleClick", defaultSettings.middleClickCommand);
	middleDoubleClickCommand = GetRCLine(name, "OnMiddleDoubleClick", defaultSettings.middleDoubleClickCommand);
	rightClickCommand = GetRCLine(name, "OnRightClick", defaultSettings.rightClickCommand);
	rightDoubleClickCommand = GetRCLine(name, "OnRightDoubleClick", defaultSettings.rightDoubleClickCommand);
	enterCommand = GetRCLine(name, "OnMouseEnter", defaultSettings.enterCommand);
	leaveCommand = GetRCLine(name, "OnMouseLeave", defaultSettings.leaveCommand);
}

LabelSettings::~LabelSettings()
{
	if(isDefault)
	{
		delete skin;
		delete font;
	}
}

LabelSettings defaultSettings;
