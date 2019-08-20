#!/usr/bin/env bash

set -x

KEY=docker-image-$IMG_TAG
cache has_key $KEY
echo $?
cache restore $KEY
docker load -i img.tar
