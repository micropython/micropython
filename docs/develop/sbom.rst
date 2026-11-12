.. _sbom:

Software bill of materials (SBOM)
=================================

``make sbom`` writes a software bill of materials for a firmware build, in SPDX 2.3
and CycloneDX 1.6 JSON formats. It needs `uv <https://docs.astral.sh/uv/>`_ installed::

    cd ports/stm32
    make BOARD=PYBV10 sbom

This writes ``build-PYBV10/sbom.spdx.json`` and ``build-PYBV10/sbom.cdx.json``
(building the firmware first if needed). Ports with more than one image write one
SBOM per image: alif per core (``build-<BOARD>/<core>/``), psoc-edge also for the
secure boot image (``build-<BOARD>/secboot/``).

The SBOM lists what was linked into that firmware, so it depends on the board,
variant and options. It includes:

* MicroPython itself, and each submodule, or library with its own licence file,
  that was linked, with the submodule's version.
* The toolchain libraries linked statically, such as libgcc, newlib and
  picolibc.
* Frozen Python modules, including micropython-lib packages and C modules
  added by the frozen manifest or ``USER_C_MODULES``.
* On esp32, the ESP-IDF components that were linked, with the version, CPE and
  licence reported by Espressif's ``esp-idf-sbom``.

The licence of each component is the combination of the licences of its source
files that were linked. It comes from the ``SPDX-License-Identifier`` tag in
the file, the frozen manifest's licence metadata, a ``REUSE.toml`` annotation,
the ``[licenses]`` table at the end of the top level ``LICENSE`` file, or the
licence file in the component's directory.

``make sbom`` fails, and writes nothing, if a linked file has no licence, can't be
traced back to a source file, or belongs to more than one component. Fix it by adding
an SPDX tag to the file or an entry to the ``[licenses]`` table. For sources outside
the MicroPython tree, pass their directory with ``--src-root``::

    make BOARD=PYBV10 sbom SBOM_FLAGS="--src-root /path/to/sources"

CMake ports (esp32, rp2) pick up ``USER_C_MODULES`` automatically.

A project's own code is licensed with a ``REUSE.toml`` in each ``--src-root`` directory,
with the licence text in a ``LICENSES`` directory beside it. A board directory outside
the tree is its own component, so it needs its own ``REUSE.toml``.

The same ``REUSE.toml`` can also declare the licence of individual files in a third party
directory below it that has none of its own, by their path relative to the ``REUSE.toml``.
A declaration inside the third party directory takes priority.

The licence file of a component is recognised when it is a single short file holding the
standard MIT, ISC, BSD-2-Clause, BSD-3-Clause, Apache-2.0 or MPL-2.0 text. Any other licence
file makes ``LicenseRef-<component>``, with the file as its text.

Builds with LTO fail because the objects can't be traced. Build with ``LTO=0``
instead (nrf, samd21 and some stm32 boards enable LTO by default).

Running by hand
---------------

``tools/sbom.py`` can also be run on the GNU ld or LLD (clang) map file of any
build::

    tools/sbom.py ports/stm32/build-PYBV10/firmware.map --spdx fw.spdx.json --cyclonedx fw.cdx.json

See ``tools/sbom.py --help`` for the options to pass the frozen manifest
(``--frozen``, ``-v``) and ESP-IDF build (``--idf``).
