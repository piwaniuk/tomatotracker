Quick Start Manual
==================

The documents which are edited in Tomato Tracker are called songs. The main
part of the song are tracks, each of which contains a sequence of phrases.
Phrases in turn are sequences of patterns. A pattern is a smallest reusable
fragment of music and it usually corresponds to a bar.

Because user interface is text based, all input is done with a keyboard.
Different parts of the song are edited in different views:
* Song options view
* Song tracks view
* Phrase view
* Pattern view
* Instrument view

Currently the tracker always requires 1 commandline argument: a path to file
with a song. If the file does not exist, a new song is created. Otherwise the
program attempts to load and existing song from the file.

Song options view
---------

This is the initial view when a new song is created or an existing song is
loaded. A few basic song properties can be edited in this view.

Keybindings:
* arrows - navigate
* S - save song
* Q - exit the program
* e - edit song in song tracks view
* ENTER - edit text fields
* ESC - cancel editing
* ,.<> - edit numeric fields

Song tracks view
---------

The main part of this view is a table with tracks in columns and pattern steps
in rows.

Keybindings:
* arrows - navigate
* n - create new phrase
* e - edit phrase in phrase view
* . - clear phrase
* SPACE - reuse last used phrase
* ENTER - choose phrase from list
* ESC - cancel editing
* q - go back to previous view

Phrase view
-----------

This view allows editing a single phrase. The main part of the view is a list
of patterns.

Keybindings:
* arrows - navigate
* n - create new pattern
* e - edit pattern in pattern view
* . - clear pattern
* SPACE - reuse last used pattern
* ENTER - choose pattern form list
* ESC - cancel editing
* p - toggle play from current position / stop
* q - go back to previous view

Pattern view
------------

The main part of this view is a list of notes in a pattern. The first column
describes note pitch, the second column describes note instrument and the third
column corresponds to note length.

Keybindings:
* arrows - navigate
* zxcvbnm, - white keys
* sdfhjl - black keys
* . - clear note
* ENTER - choose instrument from list
* ESC - cancel editing
* e - edit instrument in instrument view
* ,.<> - edit numeric fields
* [ - octave down
* ] - octave up
* p - toggle play from current position / stop
* q - got back to previous view


Instrument view
--------

This view allow changing properties of an existing instrument.

Keybindings:
* arrows - navigate
* ENTER - edit text fields
* ESC - cancel editing
* ,.<> - edit numeric fields
* q - go back to previous view

