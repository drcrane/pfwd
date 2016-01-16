#!/bin/sh

# You should probably use the autotools.

# Order matters here, do not put -lpthread before the source files.

rm bin/main
gcc -Wall -pedantic -std=c99 -D_REENTERANT -o bin/main src/main.c src/application.c -lpthread
