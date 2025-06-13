#!/bin/bash
set -e

# Get the test directory (where this script and ramdisk.py are located)
TEST_DIR=$(dirname $0)

$MPREMOTE exec "import os; os.VfsFat" || { echo "SKIP (ramdisk not supported)"; exit 0; }

# setup 
echo -----
$MPREMOTE soft-reset run "${TEST_DIR}/ramdisk.py"
$MPREMOTE ls

echo -----
echo "empty tree"
$MPREMOTE tree :

echo -----
$MPREMOTE touch :a.py + touch :b.py  
$MPREMOTE mkdir :foo + touch :foo/aa.py + touch :foo/ba.py

echo "small tree - :" 
$MPREMOTE tree :

echo -----
echo "no path" 
$MPREMOTE tree 

echo -----
echo "path = '.'" 
$MPREMOTE tree .

echo -----
echo "path = ':.'" 
$MPREMOTE tree :.


echo -----
echo "multiple trees" 
$MPREMOTE mkdir :bar + touch :bar/aaa.py + touch :bar/bbbb.py
$MPREMOTE mkdir :bar/baz + touch :bar/baz/aaa.py + touch :bar/baz/bbbb.py
$MPREMOTE mkdir :bar/baz/quux + touch :bar/baz/quux/aaa.py + touch :bar/baz/quux/bbbb.py
$MPREMOTE mkdir :bar/baz/quux/xen + touch :bar/baz/quux/xen/aaa.py

$MPREMOTE tree

echo -----
echo single path
$MPREMOTE tree :foo

echo -----
echo "multiple paths" 
$MPREMOTE tree :foo :bar

echo -----
echo "subtree" 
$MPREMOTE tree bar/baz

echo -----
echo mountpoint
$MPREMOTE tree :/ramdisk

echo -----
echo non-existent folder : error
$MPREMOTE tree :not_there || echo "expect error: $?"

echo -----
echo file : error 
$MPREMOTE tree :a.py || echo "expect error: $?"

echo -----
echo "tree -s :"
mkdir -p "${TMP}/data"
dd if=/dev/zero of="${TMP}/data/file1.txt" bs=1 count=20 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file2.txt" bs=1 count=204 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file3.txt" bs=1 count=1096 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file4.txt" bs=1 count=2192 > /dev/null 2>&1

$MPREMOTE cp -r "${TMP}/data" :
$MPREMOTE tree -s :
echo -----
echo "tree -s"
$MPREMOTE tree -s
echo -----
$MPREMOTE tree --human :
echo -----
$MPREMOTE tree -s --human : || echo "expect error: $?"
echo -----

