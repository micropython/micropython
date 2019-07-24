#!/bin/bash

rm -rf ../../../.git/hooks/*
ln -s ../../tools/git/hooks/src/pre_commit.py ../../../.git/hooks/pre-commit
ln -s ../../tools/git/hooks/src/commit_msg.py ../../../.git/hooks/commit-msg
