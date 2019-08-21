#!/bin/bash
basepath=$(cd `dirname $0`; pwd)
cd $basepath/examples/zvm &&
make clean -f Makefile.upylib &&
make -f Makefile.upylib &&
cd $basepath &&
echo "build libtvm finished"
