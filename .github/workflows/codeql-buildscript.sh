#!/usr/bin/env bash

sudo apt-get update -y
sudo apt-get install -y libffi-dev # for header ffi.h
source tools/ci.sh && ci_unix_standard_build
