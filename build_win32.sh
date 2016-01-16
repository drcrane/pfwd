#!/bin/sh

# You should probably use the autotools.

# Order matters here, do not put -lws2_32 before the source files.

rm bin/main32.exe
i686-w64-mingw32-gcc -Wall -pedantic -std=c99 -o bin/main32.exe \
src/main.c src/application.c -lws2_32
