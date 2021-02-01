littlefs library
================

The upstream source for the files in this directory is
https://github.com/littlefs-project/littlefs

To generate the separate files with lfs1 and lfs2 prefixes run the following
commands in the top-level directory of the littlefs repository (replace the
version tags with the latest/desired ones, and set `$MPY_DIR`):

    git checkout v1.7.2
    python2 ./scripts/prefix.py lfs1
    cp lfs1*.[ch] $MPY_DIR/lib/littlefs
    git reset --hard HEAD

    git checkout v2.3.0
    python2 ./scripts/prefix.py lfs2
    cp lfs2*.[ch] $MPY_DIR/lib/littlefs
    git reset --hard HEAD
