#!/usr/bin/env bash

set -x

docker save -o img.tar $IMG_TAG
cache store docker-image-$IMG_TAG img.tar
