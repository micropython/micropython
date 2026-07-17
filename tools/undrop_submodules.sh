#!/bin/sh

git config --global user.email "nobody@satoshilabs.com"
git config --global user.name "nobody"
COMMIT=`git log --pretty=oneline | grep "drop unwanted submodules" | head -n1 | grep -v Revert | cut -d ' ' -f 1`
git revert --no-edit "$COMMIT"
