# TOSU

### It is pronounced "toss" because you will be tosuing your keyboard out the window.

## ABOUT

**Tosu** (Typing Osu) is a rhythm game where you press all letters of the keyboard to the music!

Currently being remade from the ground up, a second time! (How do i keep doing this)

Not much game exists yet.

## EXPORTING

The steps listed here are for exporting on Linux, on other platforms you will need to
export it according to what [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart)
says for that platform. (Other than the last step this is pretty much copied from there anyway)

 - Download [raylib-quickstart](https://github.com/raylib-extras/raylib-quickstart)
 - Clear the contents of the `src` folder
 - Clone this repository into the `src` folder
 - In the `build` folder type `./premake5 gmake` into the console
 - In the root folder type `make`
 - The exported build should be located in `bin`
 - Copy the `resources` folder next to the executable

And then you are done!

## HOW TO PLAY

The is no gameplay loop yet, you have access to a rudimentary editor.

### EDITOR

 - `Enter` - Start gameplay scene with current chart
 - `Ctrl + S` - Save chart to `test.chart`
 - `Ctrl + L` - Load `test.chart`
 - `Ctrl + O` - Clear chart
 - `Ctrl + P` - Prints all notes in a chart
 - `N` - Insert normal notes mode
 - `H` - Insert hold notes mode
 - `M` - Insert mines mode
 - `E` - Edit mode
 - `Esc` - Exit mode
 - `Backspace` - Remove current note
 - `1` - Start placing notes in color 1
 - `2` - Start placing notes in color 2
 - `3` - Start placing notes in color 3
 - `Left Arrow` - Move back in timeline
 - `Right Arrow` - Move foward in timeline
 - `Shift` - Move in the timeline slower
 - `Alt` - Move in the timeline faster
 - `Up Arrow` - Jump to next note
 - `Down Arrow` - Jump to previous note
 - `Ctrl + Left Arrow` - Move to the first note
 - `Ctrl + Right Arrow` - Move to the last note

#### EDIT MODE

 - Moving along the timeline also moves the current note,
   use `Up Arrow` and `Down Arrow` to switch notes
 - `Tab` - Start editing a hold note's duration, press again to switch back to editing timeline position
 - `[Letters]` - Change the note'n key to key pressed
 - `1` - Change note's color to 1
 - `2` - Change note's color to 2
 - `3` - Change note's color to 3
 - `Esc` - Exit mode

#### INSERT MODE

 - `[Letters]` - Add note with this key to current position in the timeline
 - `Esc` - Exit mode

### GAMEPLAY

 - `Enter` - Return to editor
 - `[Letters]` - Hit at specific times according to chart

## DEVELOPED WITH

 - Raylib
 - C language

