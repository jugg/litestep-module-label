--------------------------------------------------------------------------
                                Label 1.4
               Kevin Schaffer (Maduin) <kschaffe@kent.edu>
                        Last Modified: 03-20-2001
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

The following is a list of all the valid data sources and any parameters
they take:

activeTask
Title of the currently active task.

computerName
Name assigned to this computer.

cpu
Current total CPU usage percent.

date
Current local date.

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

time
Current local time.

userName
Name of the user currently logged on.

The following is the list of all valid modifier functions. These functions
can be used to modify the output from any of the above data sources. For
instance, the computer name is usually reported in all uppercase letters,
so you could use lowerCase or capitalize to change that.

capitalize(x)
Makes all characters lowercase except for the first which is uppercased.

lowerCase(x)
Makes all characters lowercase.

trim(x)
Removes leading and trailing whitespace.

upperCase(x)
Makes all characters uppercase.


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
