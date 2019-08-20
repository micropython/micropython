#!/usr/bin/env bash

set -x

KEY=docker-image-$IMG_TAG
cache has_key $KEY || exit 0
cache restore $KEY
docker load -i $IMG_TAR
