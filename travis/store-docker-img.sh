#!/usr/bin/env bash

set -x

docker save -o $IMG_TAR $IMG_TAG
cache store docker-image-$IMG_TAR img.tar
