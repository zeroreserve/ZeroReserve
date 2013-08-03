ZeroReserve
===========

Friend 2 Friend Payment and Bitcoin exchange

Prerequisite for building is a successful RetroShare build and sqlite3.
RetroShare is available from http://retroshare.sourceforge.net/
sqlite3 is probably part of your Linux distribution. If you are on Windows
or MacOS, get it from http://www.sqlite.org/

To build, checkout the sources to the plugins directory of Retroshare and build with

$ qmake && make clean && make

To install on Windows, drop the resulting DLL into the
%APPDATA%\Retroshare\extensions directory.

To install on Linux or MacOS, drop the resulting shared object into
~/.retroshare/extensions


This is experimental software. Use at your own risk. At this stage, leave TestNet
on. Don't enable real currencies.
