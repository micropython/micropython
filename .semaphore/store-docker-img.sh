#!/usr/bin/env bash

set -x

cache restore docker-image-$IMG_TAG
docker load -i img.tar
