#!/bin/bash
set -e

# Get the test directory (where this script and ramdisk.py are located)
TEST_DIR=$(dirname $0)

# setup 
echo -----
$MPREMOTE run "${TEST_DIR}/ramdisk.py"
$MPREMOTE resume ls

echo -----
echo "empty tree"
$MPREMOTE resume tree :

echo -----
$MPREMOTE resume touch :a.py + touch :b.py  
$MPREMOTE resume mkdir :foo + touch :foo/aa.py + touch :foo/ba.py

echo "small tree - :" 
$MPREMOTE resume tree :

echo -----
echo "no path" 
$MPREMOTE resume tree 

echo -----
echo "path = '.'" 
$MPREMOTE resume tree .

echo -----
echo "path = ':.'" 
$MPREMOTE resume tree :.


echo -----
echo "multiple trees" 
$MPREMOTE resume mkdir :bar + touch :bar/aaa.py + touch :bar/bbbb.py
$MPREMOTE resume mkdir :bar/baz + touch :bar/baz/aaa.py + touch :bar/baz/bbbb.py
$MPREMOTE resume mkdir :bar/baz/quux + touch :bar/baz/quux/aaa.py + touch :bar/baz/quux/bbbb.py
$MPREMOTE resume mkdir :bar/baz/quux/xen + touch :bar/baz/quux/xen/aaa.py

$MPREMOTE resume tree

echo -----
echo single path
$MPREMOTE resume tree :foo

echo -----
echo "multiple paths" 
$MPREMOTE resume tree :foo :bar

echo -----
echo "subtree" 
$MPREMOTE resume tree bar/baz

echo -----
echo mountpoint
$MPREMOTE resume tree :/ramdisk

echo -----
echo non-existent folder : error
$MPREMOTE resume tree :not_there || echo "expect error: $?"

echo -----
echo file : error 
$MPREMOTE resume tree :a.py || echo "expect error: $?"

echo -----
echo "tree -s :"
mkdir -p "${TMP}/data"
dd if=/dev/zero of="${TMP}/data/file1.txt" bs=1 count=20 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file2.txt" bs=1 count=204 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file3.txt" bs=1 count=1096 > /dev/null 2>&1
dd if=/dev/zero of="${TMP}/data/file4.txt" bs=1 count=2192 > /dev/null 2>&1

$MPREMOTE resume cp -r "${TMP}/data" :
$MPREMOTE resume tree -s :
echo -----
echo "tree -s"
$MPREMOTE resume tree -s
echo -----
$MPREMOTE resume tree --human :
echo -----
$MPREMOTE resume tree -s --human : || echo "expect error: $?"
echo -----

