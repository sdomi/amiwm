## Background module

To get background images (aka backdrops) on your screens,
use the Background module. The Module command should come
after the Screen command, and the initstring should be the
filename of the picture.  Example:

Screen "Workbench Screen"
Module "Background" "~/megumin.png"

The Background module is really a shell script that requires the
program 'display' from imagemagick package. Currently, it doesn't
support any standard imagemagick parameters.

## Background_stretch module

Same as the Background module, but stretches your backdrop to size of
your screen. Example:

Screen "Workbench Screen"
Module "Background_stretch" "~/megumin.png"

Please note that this module will just stretch or downscale the image
(depending on the size of the image) to your display ratio.
If you don't want your image to look ugly, try the next module..

## Background_resize module

Same as the Background_stretch, but it cares about your display ratio.
For example, if you've got a 16:9 screen but your backdrop image is
4:3, it will get stretched, but it will still be displayed as a 4:3
picture. The remaining part of the screen will be used to show the same
image, but repeated.

## Keyboard module

With the Keyboard module, you can bind window manager functions to keys
on the keyboard. The initstring should consist of keybindings on the
form

modifiers<keysym>:where:func

### modifiers

Modifiers is 0 or more of:
  Shift Control Meta Lock
  Mod1 Mod2 Mod3 Mod4 Mod5
  Button1 Button2 Button3 Button4 Button5

The modifiers listed must be pressed together with the key to activate
the binding.

### keysym

Keysym is a standard X keysym name, like F1 or Prior.

### where

where is either "all", or one or more of window, frame, icon, root
separated with '|' (bar).  This controls in what areas of the screen
that the key will be active.

### func


The function to perform when the key is pressed.
Currently the following are defined:

rotatescreens  -  Move the frontmost screen to the back
front          -  Move the window in which the key is pressed to the front
back           -  Move the window in which the key is pressed to the back
iconify        -  Iconify the window in which the key is pressed


#### Example

Module "Keyboard" "\
	Meta<F1>:window|frame:front\
	Meta<F2>:window|frame:iconify\
	Meta<F3>:window|frame:back"

This binds the keys Meta-F1, Meta-F2 and Meta-F3 to front, back and
iconify respectively.  The will only have effect inside windows and in
window frames.  (These are the only places that front/iconfy/back has
effect anyway.)

## Filesystem module

This is an april fools joke module, but at least it's a starting
point for building out an icon on the desktop!

Module "Filesystem" "/path/to/open"

## Launcher module

This is a simple prototype launcher program that will place an
icon on the current workbench screen so it can be double clicked
to run.

The module must be placed after the Screen definition you wish
for it to show up in.

The format is:

Module "Launcher" "(<label>) (<icon>) (<command and args>)"

The icon is one of the .info icons in the amiwm IconDir.

