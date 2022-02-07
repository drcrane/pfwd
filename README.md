pfwd A Trivial Port Forwarder
=============================

This is a simple port forwarding application written in C for windows and linux.
All it does is listen and forward any traffic to another host using TCP/IP.
The programme starts a new thread for each connection and uses select to determine 
when activity is present on a socket. This could be improved by removing 
threading.

I wrote this because I wanted to log the conversation between a remote device and 
it's server to see what it was saying. These days it is not too much use because 
most devices will (or at least should) encrypt their comms.

Example Invocations
-------------------

Listen on all IP addresses on port 5000 and forward any incomming connections to
192.168.1.104 port 2030, simple:

	pfwd 0.0.0.0 5000 192.168.1.104 2030

TODO List
---------

 * Have all activity on a single thread (the main thread)
 * Allow the programme to be daemonised
 * Fix autoconf warnings about `AC_HELP_STRING`
 * Don't build `streaminspector` when plugins are disabled

Building
--------

This project has been made to build with GNU autotools (though I do not like 
them they are a defacto standard and they do work). I often find it useful to 
have a standard shell script too, you will find `build_lin.sh`, `build_win32.sh` 
and `build_win64.sh` which may help you if using autotools does not work.

If you have the distribution version of this project then you will have a 
configure script which you may run, along with make, in the normal way. If you 
have cloned this from my repo you will need `autoconf` and `automake` installed 
on your system as you will be required to execute `autoreconf -i` to make the 
`configure` script.

To configure the programme to dump all the relayed traffic in hex and printable
ASCII invoke `configure` like this:

    ./configure --enable-hexdumps

Plugins
-------

The writestream plugin allows one to write all the communication from the
server to the client and from the client to the server to a file for later
inspection. This is the only plugin for now, to enable it:

    ./configure --enable-plugins

A `.log` file will be created for every connection forwarded. The format is
simple and can be decoded with the `streamdecoder` programme in the
`writestream` plugins directory.

Interactive
-----------

`pfwd` provides the ability to drop any transmission from the client to the
server or server to client through an interactive terminal prompt. To enable
the feature at compilation time:

    ./configure --enable-interactive

To use the feature enter one of the following commands to stdin when pfwd is
running:

* `SND` Send data from server to client (default)
* `NOSND` Do not send data from server to client
* `REC` Send data received from client to server (default)
* `NOREC` Do not send data received from client to server

References
----------

Although this is a "Hello World" style programme it was useful to educate
myself in using Autotools. If you would like to know more about autotools then 
please have a look at the following URLs:

* [Autotools: a practitioner's guide to Autoconf, Automake and Libtool](http://freesoftwaremagazine.com/books/autotools_a_guide_to_autoconf_automake_libtool/)
* [Introduction to the Autotools](http://www.dwheeler.com/autotools/)


