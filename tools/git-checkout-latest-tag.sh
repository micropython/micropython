#!/bin/bash
git fetch --tags
git checkout $(git describe --tags `git rev-list --tags --max-count=1`)
