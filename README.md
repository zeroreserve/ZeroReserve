ZeroReserve
===========

Friend 2 Friend Payment and Bitcoin exchange

Prerequisite for building is a successful RetroShare build and sqlite3 and libnmcrpc
RetroShare is available from http://retroshare.sourceforge.net/
sqlite3 is probably part of your Linux distribution. If you are on Windows,
get it from http://www.sqlite.org/

Get libnmcrpc directly from git:
```
$ git clone git@gitorious.org:libnmcrpc/libnmcrpc.git
```

Then configure with :
```
$ autogen.sh
$ ./configure
```

To build, checkout the sources to the plugins directory of Retroshare and build with:
```
$ qmake && make clean && make
```

To install on Windows, drop the resulting DLL into the
%APPDATA%\Retroshare\extensions directory.

To install on Linux or MacOS, drop the resulting shared object into
~/.retroshare/extensions

Running ZeroReserve requires a running Satoshi Client first:
```
$ ./bitcoind
or
$ ./bitcoin-qt -server     # that shows the GUI

```
This is an example bitcoin.conf with all entries required:
```
testnet=1
rpcuser=anu
rpcpassword=mysupersecretpassword
rpcport=18332
```


This is experimental software. Use at your own risk. At this stage, leave TestNet
on. Don't enable real currencies.

Building and running on MacOS may or may not work at this point. 
