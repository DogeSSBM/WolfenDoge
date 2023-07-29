# WolfenDoge
A Wolfenstein inspired, psudo 3D ray caster and map editor.

<!-- ![Demo](./Demos/Main.gif) -->
<img src="./Demos/Main.gif" height="50%" width="50%">

## Editor
WolfenDoge features a builtin editor that allows you to make changes on the fly.

<!-- ![Demo](./Demos/Editor.gif) -->
<img src="./Demos/Editor.gif" height="50%" width="50%">

## How to Use the Editor
While in game, the WolfenDoge map editor can accessed by pressing Ctrl+E.
While in the map editor, you can return to the main game by pressing Ctrl+Q.
WolfenDoge maps are composed of various types of segments (as in line segments).
Every segment contains at least two coordinates. These define where the segments ends are.
Editing the position segment coordinates is easy, simply just left click.
The coordinate closest to the mouse cursor, and the segment containing it will be selected.

New segments may be created by right clicking and dragging.
Segments may be deleted by pressing Delete.

While a segment is selected, the following actions are possible -
Pressing Esc will un-select the segment.
Pressing R will change your selected coordinate to the other in the segment.
The currently selected coordinate can be moved around by left clicking and dragging.

## Files

<!-- ![Demo](./Demos/Files.gif) -->
<img src="./Demos/Files.gif" height="50%" width="50%">

WolfenDoge maps may be saved while in game or the map editor.
To save the map in it's current state (including all changes made in the editor.) press Ctrl+S.
Maps will be saved in the Maps folder(duh), the file name will be output to stdout.
Map files can be loaded when starting the game from the commandline as arguments.
Ex: $ ./main.out './Maps/plus.bork'
