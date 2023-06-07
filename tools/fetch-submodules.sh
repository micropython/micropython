#!/usr/bin/env sh
# Pass the directories of submodules to fetch as command-line arguments. For example:
# ./fetch-submodules.sh lib tools
# If no arguments are passed, all submodules will be fetched.
# This script handles being called at other than the top level by making the paths
# for the submodules absolute.

TOP=$(git rev-parse --show-toplevel)

git submodule sync --quiet
# Prefix all the args with the absolute path to the top of the repo.
abs_submodules=""
for d in "$@"; do
    abs_submodules="${abs_submodules} ${TOP}/${d}"
done
echo ${abs_submodules}

# Fetch submodules as partial clones if possible. If that fails due to an older version of git,
# do a shallow init and fetch tags.
git submodule update --init --filter=blob:none ${abs_submodules} || \
    git submodule update --init --depth 1 ${abs_submodules} && \
    git submodule foreach 'git fetch --tags --depth 1' || \
    echo "ERROR: fetch-submodules.sh FAILED"
