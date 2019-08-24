#!/usr/bin/env sh

HASH=$(cat Dockerfile | md5sum)
echo "cirrus-ci-community/$(printf $HASH)"
