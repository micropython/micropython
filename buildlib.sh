#!/bin/bash
#rm -rf examples/zvm/build
basepath=$(cd `dirname $0`; pwd)
if [ -f $basepath/examples/zvm/libtvm.a ]; then
    rm $basepath/examples/zvm/libtvm.a
fi
cd $basepath/examples/zvm &&
make -f Makefile.upylib &&
cd $basepath &&
echo "build libtvm finished"
