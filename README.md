ZeroReserve
===========

Friend 2 Friend Payment and Bitcoin exchange

[For Windows, there is a precompiled ZeroReserve / Retroshare bundle](https://mega.co.nz/#!MVIykABI!fFkRbtdOQDfrIKc0Kke2yfFpS6T17z7q6YxelQ6BJMc). You can install that and proceed with the [Setup](https://github.com/zeroreserve/ZeroReserve/wiki/Getting-started-with-Zero-Reserve), ignoring everything below.

Prerequisite for building is
* a successful RetroShare build. http://retroshare.sourceforge.net/
* sqlite3 http://www.sqlite.org/ (use package from your distro, see below)
* jsoncpp version 0.6rc or higher: svn co http://svn.code.sf.net/p/jsoncpp/code/trunk/jsoncpp (included in Debian)
* libcurl http://curl.haxx.se/ (use package from your distro, see below)
* Boost http://www.boost.org (use package from your distro, see below)

First, you need to build Retroshare. [Use these build instructions](http://retroshare.sourceforge.net/wiki/index.php/UnixCompile). Note: there is no need to build retroshare-nogui.

On most Linux Distros, Boost, sqlite, curl and jsoncpp are included. On Debian 7, simply do

```
# apt-get install libjsoncpp-dev libsqlite3-dev libcurl4-openssl-dev libboost-all-dev
```

To build Zero Reserve, checkout the sources to the plugins directory of Retroshare and build with:
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

If you want DHT, run the following command, replacing $ID with your id (a hex string looking like "3f40a66fa91aba29487cc6ac938d0687"). If you don't know what DHT is, you also want run this command:

```
$ echo "85.10.202.165 28839" > ~/.retroshare/$ID/bdboot.txt
```

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
