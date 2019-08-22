#!/usr/bin/env sh

git describe --exact-match HEAD 2> /dev/null || echo "master"
