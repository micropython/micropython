This directory contains assets for other tests to use:

- A .mpy built against the current .mpy version that can be used to test
  freezing without a dependency on mpy-cross (`frozentest.py` and
  `frozentest.mpy`)
- A ROMFS image containing random binary files for testing mounting and reading
  from the partition without having to generate a custom image
  (`random_romfs.bin`, the `romfs_source` directory, and `Makefile`)
