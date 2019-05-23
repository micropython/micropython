#!/bin/sh  

gcc tools/cygwin.c -o build/set_cygwin_path
echo -n "TOPDIR=" > build/cygwin.mk
build/set_cygwin_path $1 $2 >> build/cygwin.mk