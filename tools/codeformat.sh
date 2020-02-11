#!/bin/bash

if [ ! -e .astylerc ]; then
    echo "Run this from the top-level directory, e.g. ./tools/codeformat.sh"
    exit 1
fi

EXCLUSIONS="py/vm.c"

FILES=`ls py/*.{c,h} extmod/*.{c,h} ports/*/*.{c,h} | grep -v -F "${EXCLUSIONS}"`

astyle --project ${FILES}
