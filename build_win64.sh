#!/bin/sh

# You should probably use the autotools.

# Order matters here, do not put -lws2_32 before the source files.

rm bin/main64.exe
x86_64-w64-mingw32-gcc -DPFWD_ENABLE_PLUGINS -DPFWD_ENABLE_HEXDUMPS -I . -I src -Wall -pedantic -std=c99 -o bin/main64.exe \
src/main.c src/application.c -lws2_32
