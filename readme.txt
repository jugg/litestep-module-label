--------------------------------------------------------------------------
                                Label 1.61
               Kevin Schaffer (Maduin) <kschaffe@kent.edu>
                        Last Modified: 05-28-2001
--------------------------------------------------------------------------

Overview
--------

Label.dll is a Litestep-loadable module that allows you to define multiple
skinnable labels and place them anywhere on your desktop. Labels can
display static text and dynamically updating system information that can
be updated at run-time using bang commands.


Loading the Module
------------------

Add a new LoadModule line to the step.rc file, similar to the following
(this assumes that label.dll resides in C:\Litestep):

LoadModule "C:\Litestep\label.dll"

Now recycle Litestep and you should see a small blank label in the upper-
left-hand corner of the desktop. The following sections describe how to
configure label.dll to actually do something useful.


Configuration
-------------

The following commands are typically placed in the step.rc. See the
Litestep documentation for help modifying this file. Parameters and their
types are denoted by angle brackets (<, >) and optional parameters are
enclosed in square brackets ([, ]).

LabelText <text>
Sets the text displayed by the label. If no value is provided the label is
left blank. The value can contain both simple text and escape sequences.
Escape sequences are enclosed in square brackets ("Simple text [escape]").
See the discussion of text escape sequences below for more information.

LabelX <number>
Sets the horizontal position of the label in pixels. If the value is
positive then it is relative to the left side of the desktop, if negative
then the right side. Additionally you can make the position relative to
the center of the desktop by appending the character 'c' after the number
("32c", "-128c").

LabelY <number>
Sets the vertical position of the label in pixels. If the value is
positive then it is relative to the top of the desktop, if negative then
the bottom. Additionally you can make the position relative to the center
of the desktop by appending the character 'c' after the number ("32c",
"-128c").

LabelWidth <number>
Sets the horizontal size of the label. This can be an absolute value (in
pixels) or a percentage of the desktop width.

LabelHeight <number>
Sets the vertical size of the label. This can be an absolute value (in
pixels) or a percentage of the desktop height.

LabelAlwaysOnTop
If this command is present then it makes the label "always on top", that
is, the label stays above all application windows.

LabelStartHidden
If this command is present then the label will be initially invisible. It
can later be shown using bang commands.

LabelImage <file>
Sets the name of the skin file. This file must be in Windows bitmap (BMP)
format. If no value is provided then the label is displayed with the a
default background.

LabelImageLeftEdge <number>
Sets the number of pixels on the left edge of the bitmap that will not be
stretched or tiled. If no value is provided then the default is 0.

LabelImageTopEdge <number>
Sets the number of pixels on the top edge of the bitmap that will not be
stretched or tiled. If no value is provided then the default is 0.

LabelImageRightEdge <number>
Sets the number of pixels on the right edge of the bitmap that will not be
stretched or tiled. If no value is provided then the default is 0.

LabelImageBottomEdge <number>
Sets the number of pixels on the bottom edge of the bitmap that will not
be stretched or tiled. If no value is provided then the default is 0.

LabelImageMode "stretch", "tile", "tile-horizontal", or "tile-vertical"
Sets the method used to scale the skin bitmap. If no value is provided
then the default is "stretch".

LabelTransparent
If this command is present then it enables transparency. If a skin is also
in use then areas of the skin bitmap that are pink (RGB 255, 0, 255) will
allow the desktop wallpaper to show through. If no skin is in use and this
command is present then the entire label will be transparent.

LabelFont <font>
Sets the name of the font used to display text. If no value is provided
then the default is "Arial".

LabelFontHeight <number>
Sets the height of the font used to display the text in pixels. If no
value is provided then the default is 15.

LabelFontBold
If this command is present then the font will be bold.

LabelFontItalic
If this command is present then the font will be italic.

LabelFontColor <color>
Sets the color of the font used to display the text. If no value is
provided then the default dpends on whether shadowing is enabled. If it is
enabled then the default is white (the shadow color defaults to black),
otherwise the default is black.

LabelFontShadow
If this command is present then the font will be shadowed.

LabelFontShadowColor <color>
Sets the color used to display the text shadow. If no value is provided
then the default is black.

LabelFontShadowX <number>
Sets the number of pixels in the horizontal direction that the shadow is
offset from the rest of the text; this can be negative. If no value is
provided then the default is 1.

LabelFontShadowY <number>
Sets the number of pixels in the vertical direction that the shadow is
offset from the rest of the text; this can be negative. If no value is
provided then the default is 1.

LabelJustify "left", "center", or "right"
Sets the text justification to be either left-justified, centered, or
right-justified. If no value is provided then the default is "center".

LabelLeftBorder <number>
Sets the number of pixels on the left edge to reserve as a border. If no
value is provided then the default is 0.

LabelTopBorder <number>
Sets the number of pixels on the top edge to reserve as a border. If no
value is provided then the default is 0.

LabelRightBorder <number>
Sets the number of pixels on the right edge to reserve as a border. If no
value is provided then the default is 0.

LabelBottomBorder <number>
Sets the number of pixels on the bottom edge to reserve as a border. If no
value is provided then the default is 0.

LabelUpdateInterval <number>
Sets the amount of time in milliseconds between updates for labels that
contain dynamically updated content. If no value is provided then the
default is 1000 (1 second).

LabelLSBoxName <name>
Sets the name of the box (lsbox.dll) that this label should be loaded
into. The default is no value in which case the label is loaded normally.
See the section below entitled 'Using Labels inside LSBox' for more
information.


Bang Commands
-------------

Bang commands allow you to manipulate labels at run-time through scripts
or other modules. See the Litestep documentation for more information
about bang commands and how to invoke them.

!LabelAlwaysOnTop
Makes the label "always on top", that is, it stays above all application
windows.

!LabelHide
Hides the label, making it invisible.

!LabelMove <x-coordinate> <y-coordinate>
Moves the label.

!LabelPinToDesktop
Pins the label to the desktop so that it stays below all application
windows. This the default state and is the opposite of always on top.

!LabelReposition <x-coordinate> <y-coordinate> <width> <height>
Repositions (moves and resizes) the label.

!LabelResize <width> <height>
Resizes the label.

!LabelSetText [<text>]
Sets the text displayed in the label. If the parameter is not present then
the label will be cleared of any text.

!LabelShow
Shows the label, making it visible.

!LabelToggleAlwaysOnTop
Switches the label back and forth between the always on top and pinned to
desktop states.

!LabelToggle
Switches the label back and forth between the visible and invisible
states.

!LabelUpdate
Forces the label to update its contents.


Events
------

Events allow you execute programs or bang commands based on user
interaction with the label. The following work the same as configuration
settings. Each of them takes the name of program or bang commands as their
value. When the event is triggered, the program or bang command is
executed. Note that in the case of mouse clicks, assigning an action to
both the click and the double click event will not work.

LabelOnLeftClick <action>
Sets an action to perform when the user left clicks on the label.

LabelOnLeftDoubleClick <action>
Sets an action to perform when the user double-clicks the label with the
left mouse button.

LabelOnMiddleClick <action>
Sets an action to perform when the user middle clicks on the label.

LabelOnMiddleDoubleClick <action>
Sets an action to perform when the user double-clicks the label with the
middle mouse button.

LabelOnRightClick <action>
Sets an action to perform when the user right clicks on the label.

LabelOnRightDoubleClick <action>
Sets an action to perform when the user double-clicks the label with the
right mouse button.

LabelOnMouseEnter <action>
Sets an action to perform when the mouse cursor enters the label.

LabelOnMouseLeave <action>
Sets an action to perform when the mouse cursor leaves the label.


Multiple Labels
---------------

Label.dll takes a new approach to configuring multiple instances.
Traditionally a module that supported multiple instance had to use
*Config-style configuration where all the settings for each instance had
to fit on a single line (like shortcut.dll). But labels have too many
settings to put on a single line so that approach was out of the question.
Instead, each label is given an unique name and the settings for the label
all begin with that name. For example, if there is a label named "MyLabel"
then the settings for that label might look like:

MyLabelText "Hello, world!"
MyLabelImage mylabel.bmp
MyLabelHeight 24
MyLabelWidth 96
MyLabelX 64
MyLabelY 64

Simultaneously you can have another label named "MyOtherLabel":

MyOtherLabelText "Click Here!"
MyOtherLabelOnLeftClick !alert "Hello, world!"

You can have any number of labels so long as each one has a different
name. Also be careful that you label names do not conflict with any other
modules you have loaded. For instance, if chronos.dll is loaded then do
not have a label named "Chronos" or else the settings will conflict and
one module will not work correctly. In fact, I recommend that you put
"Label" somewhere in the name to keep things straight, but label.dll
places no restrictions on the name you can choose except that they can
only contain letters and numbers (no symbols or spaces).

By default, label.dll runs in a single-instance mode with only on label
(named "Label") defined. To use multiple labels you need to have one or
more "Labels" commands:

Labels <name-list>
Gives the names of the labels you wish to define. All names must be unique
and can only contain letters and numbers (no symbols or spaces). Separate
names with commas and do not use quotes. If no value is provided then a
single label named "Label" is created by default.

As of version 1.5 it is also possible to create and destroy labels as
Litestep is running. The !LabelCreate bang command takes the name of a
label that is defined in step.rc, but which is not already loaded (it
doesn't appear in any "Labels" lines). !LabelCreate then creates the label
and initializes it with the configuration specified in the step.rc. This
lets you delay creation of labels that will not be visible right away.
Each label that is loaded with have an associated !LabelDestroy bang
command that can be used to unload that label, freeing the resources it is
using.

!LabelCreate <label-name>
Creates and initializes the named label. Note that this is a global bang
command - to create a label named MyLabel use "!LabelCreate MyLabel", not
"MyLabelCreate".

!LabelDestroy
Destroy a label and release any resources in use. This bang command is
defined for each label - to destroy MyLabel use "!MyLabelDestroy".


Text Escape Sequences
---------------------

To include dynamic text in labels, such as system information, you can use
escape sequences mixed in with normal text in labels. Escapes sequences
consist of a data source names and any necessary parameters and possibly
one or more modifier functions all enclosed in square brackets:

LabelText "Just some test... [modifier(dataSource(parameter))]"
ClockLabelText "The time is: [time]"
QuoteLabelText "Quote of the Day: [trim(randomLine('C:\quotes.txt'))]"
SysLabelText "CPU: [cpu], RAM: [memInUse]"

As of version 1.5, labels can have text which spans multiple lines. To
create a line break in the text use the character sequence "\n". If you
need to include a literal backslash then use "\\".

MultiLineLabelText "This is on line 1\nThis is on line 2"
MemLabelText "Memory Info\n\nFree: [memAvailable]\nTotal: [memTotal]"

The following is a list of all the valid data sources and any parameters
they take:

activeTask
Title of the currently active task.

computerName
Name assigned to this computer.

cpu
Current total CPU usage percent.

date('format')
Current local date. Format is optional and is explained below.

diskAvailable('drive')
Amount of space currently available on given drive.

diskInUse('drive')
Amount of space currently in use on given drive.

diskTotal('drive')
Total amount of space on given drive.

hostName
Host name assigned to this computer.

ip
Internet Procotol (IP) address assigned to this computer.

iTime
Current time as Swatch internet time (beats).

memAvailable
Amount of physical memory currently available.

memInUse
Amount of physical memory currently in use.

memTotal
Total amount of physical memory in the system.

os
Name of the operating system running on this computer.

randomLine('file')
Retrieves a randomly selected line from the given file.

swapAvailable
Amount of swap space currently available.

swapInUse
Amount of swap space currently in use.

swapTotal
Total amount of swap space in the system.

time('format')
Current local time. Format is optional and is explained below.

uptime('format')
Current system uptime. Format is optional and is explained below.

userName
Name of the user currently logged on.

winampSong
Song currently loaded in Winamp. If Winamp is not running or there is not
song loaded this will be empty.

winampTime
Time elapsed on the current song in Winamp.

winampRemainTime
Time remaining on the current song in Winamp.

winampTotalTime
Length of the current song in Winamp.

winampStatus
Current status of Winamp. If Winamp is running this will be one of
"Playing", "Paused", or "Stopped". If Winamp is not running this will be
empty.

windowTitle('class')
Title of the window with the given class name.

The following is the list of all valid modifier functions. These functions
can be used to modify the output from any of the above data sources. For
instance, the computer name is usually reported in all uppercase letters,
so you could use lowerCase or capitalize to change that.

after(x, delim)
Extracts the part of x after the first occurance of delim.

afterLast(x, delim)
Extracts the part of x after the last occurance of delim.

before(x, delim)
Extracts the part of x before the first occurance of delim.

beforeLast(x, delim)
Extracts the part of x before the last occurance of delim.

between(x, delim1, delim2)
Extracts the part of x that lies between the first occurance of delim1 and
the last occurance of delim2.

capitalize(x)
Makes all characters lowercase except for the first which is uppercased.

hideIfEmpty(x)
If x evaluates to an empty string and the label is visible then it is
hidden. Otherwise if x evaluates to a non-empty string and the label
is hidden then it will be shown. The argument is passed through unchanged.

lowerCase(x)
Makes all characters lowercase.

trim(x)
Removes leading and trailing whitespace.

upperCase(x)
Makes all characters uppercase.

The [date], [time], and [uptime] data sources can optionally take a format
as a parameter. The following character sequences will be interpreted in
the format (anything else is included verbatim):

m			month (1-12)
mm			month with leading zero (01-12)
mmm			abbreviated month name (Jan, Feb, etc)
mmmm		full month name (January, February, etc)
d			day (1-31)
dd			day with leading zero (01-31)
ddd			abbreviated weekday name (Sun, Mon, etc)
dddd		full weekday name (Sunday, Monday, etc)
yy			2-digit year
yyyy		4-digit year
h			hour (1-12)
hh			hour with leading zero (01-12)
n			minute (0-59)
nn			minute with leading zero (00-59)
s			second (0-59)
ss			second with leading zero (00-59)
am/pm		am/pm indicator

For [uptime] only the day, hour, minute, and second values are valid.


Default Configurations using AllLabels
--------------------------------------

To help reduce redundant commads in the step.rc, label.dll now supports
default settings. By prepending "AllLabels" to the name of the setting you
can make defaults that apply to all labels. For example, the following
sets the a default font which will be used for LabelOne and LabelTwo but
which is overridden by LabelThree:

AllLabelsFont "Tahoma"
AllLabelsFontHeight 12
AllLabelsFontBold
AllLabelsFontColor 255 255 255
AllLabelsJustify left

LabelOneText "Hello!"
LabelOneX 100
LabelOneY 100
LabelOneJustify center

LabelTwoText "Hi!"
LabelTwoX 200
LabelTwoY 100

LabelThreeText "Greetings!"
LabelThreeX 300
LabelThreeY 100
LabelThreeFont "Arial"
LabelThreeFontHeight 11
LabelThreeFontColor 255 255 255


Using Labels inside LSBox
-------------------------

As of version 1.6, label.dll is compatible with BlkHawk's LSBox module for
Litestep. BlkHawk also released a modification of label for use with LSBox
and my code is based on his, however my code adds the LabelLSBoxName
setting (which is required if the label is to be loaded into a box). This
way you can have labels in multiple boxes as well as outside of boxes.

In order to get labels inside of a box first add a line like the following
to the .box file (adjusting the path to label.dll if needed):

*Module label.dll

Then for each label that you wish to put in that box, add a LSBoxName
setting giving the name of the box (LSBoxName setting in the .box file).
For example, to load a label named MyLabel inside a box named MyBox:

MyLabelLSBoxName MyBox

When a label is loaded inside of a box the position (LabelX/Y) are
relative to the upper-left corner of the box. For now negative coordinates
and center-relative coordinates do not work inside a box. Also, label.dll
must be loaded via a LoadModule command in step.rc even if labels are only
used from within boxes.


Disclaimer
----------

Copyright (C) 2001, Kevin Schaffer

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
