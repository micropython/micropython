#!/usr/bin/env sh

if [ $PORT = unix-coverage ]; then
  (
  cd tests
  for exp in *.exp; do
    testbase=$(basename $exp .exp)
    echo -e "\nFAILURE $testbase"
    diff -u $testbase.exp $testbase.out
  done
  )
elif [ $PORT = qemu-arm ]; then
  grep "FAIL" ports/qemu-arm/build/console.out
fi
