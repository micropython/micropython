#!/usr/bin/env bash

set -x

cache has_key $KEY
cache restore
docker load -i img.tar
