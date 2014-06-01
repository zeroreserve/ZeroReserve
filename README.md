ZeroReserve
===========

Friend 2 Friend Payment and Bitcoin exchange

Prerequisite for building is
* a successful RetroShare build. http://retroshare.sourceforge.net/
* sqlite3 http://www.sqlite.org/
* jsoncpp version 0.6rc or higher: svn co http://svn.code.sf.net/p/jsoncpp/code/trunk/jsoncpp
* libcurl http://curl.haxx.se/

On most Linux Distros, sqlite, curl and jsoncpp is included. On Debian 7, simply do

```
# apt-get install libjsoncpp-dev libsqlite3-dev libcurl4-openssl-dev
```

To build, checkout the sources to the plugins directory of Retroshare and build with:
```
$ cd retroshare-code/plugins
$ git clone git://github.com/zeroreserve/ZeroReserve.git
$ cd ZeroReserve
$ qmake && make clean && make
```

To install on Windows, drop the resulting DLL into the
%APPDATA%\Retroshare\extensions directory.

To install on Linux or MacOS, drop the resulting shared object into
~/.retroshare/extensions

Running ZeroReserve requires a running [Satoshi Client](https://bitcoin.org/en/download) first:
```
$ ./bitcoind
or
$ ./bitcoin-qt -server     # that shows the GUI *and* permits json-rpc conncetions

```
On first startup, the Satoshi client creates the directory structure for the blockchain in
$HOME/.bitcoin on Unix and %APPDATA%\Bitcoin on Windows. Create a suitable bitcoin.conf in
this directory. This is an example bitcoin.conf with all entries required:
```
testnet=1
rpcuser=anu
rpcpassword=mysupersecretpassword
rpcport=18332
```


This is experimental software. Use at your own risk. At this stage, leave TestNet
on. Don't enable real currencies.

Building and running on MacOS may or may not work at this point. 
