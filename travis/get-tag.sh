#!/usr/bin/env bash

git describe --exact-match HEAD 2> /dev/null || echo "master"
