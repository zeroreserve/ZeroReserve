ZeroReserve
===========

Friend 2 Friend Payment and Bitcoin exchange

Prerequisite for building:
* RetroShare build and 
* sqlite3 and 
* libbitcoin.

RetroShare is available from http://retroshare.sourceforge.net/.

sqlite3 is probably part of your Linux distribution. If you are on Windows,
get it from http://www.sqlite.org/

Get libbitcoin directly from git because the release doesn't have support for TestNet.
$ git clone https://github.com/spesmilo/libbitcoin.git

Then configure with
$ ./configure --enable-leveldb --enable-testnet

To build ZeroReserve, checkout the sources to the plugins directory of Retroshare and build with:

$ qmake && make clean && make

To install on Windows, drop the resulting DLL into the
%APPDATA%\Retroshare\extensions directory.

To install on Linux or MacOS, drop the resulting shared object into
~/.retroshare/extensions


This is experimental software. Use at your own risk. 

At this stage, leave TestNet on. 

Don't enable real currencies.

Building and running on MacOS may or may not work at this point.

Community Code, Bug Fixes, Other Contributions
==============================================

We love community code, bug fixes, and other forms of contribution. We use GitHub Issues and Pull Requests for contributions to this and all other code. To get started:

   1. Fork this repository.
   2. Clone your fork or add the remote if you already have a clone of the repository.
   3. Create a topic branch for your change.
   4. Make your change and commit. Use a clear and descriptive commit message, spanning multiple lines if detailed explanation is needed.

   5. Push to your fork of the repository and then send a pull request.

   6. A ZeroReserve committer will review your patch and merge it into the main repository or send you feedback.

