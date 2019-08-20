#!/usr/bin/env bash

set -x

KEY=docker-image-$IMG_TAG
cache has_key $KEY
cache restore
docker load -i img.tar
