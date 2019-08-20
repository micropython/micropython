#!/usr/bin/env bash

TAG=$(git describe --exact-match HEAD 2> /dev/null || echo "master")
echo pycampers/micropython:$TAG
