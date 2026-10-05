#!/bin/bash

# Concatenates spdebug library files to one file
TARGET=spdebug.h
HFILE=debug.h
CFILES="debug.c print.c hexdump.c"

cat $HFILE > $TARGET

cat include_c_file_prefix.h >> $TARGET

for cfile in $CFILES; do
	cat $cfile >> $TARGET
done

cat include_c_file_postfix.h >> $TARGET
