.. _releaseandversioning:

Release and versioning
======================

MicroPython releases are numbered ``MAJOR.MINOR.MICRO`` (for example, ``1.30.0``). Development
builds carry a ``-preview`` suffix.

The ``1.x`` line aims to preserve the documented Python-level API that existing programs use. It does not imply stability for internal or C-level interfaces.

Breaking changes are deferred to 2.0 and collected behind ``MICROPY_PREVIEW_VERSION_2``, which
defaults to off. A default build retains the ``1.x`` API; enabling the option selects the in-progress
2.0 API. The :doc:`2.0 migration guide </reference/micropython2_migration>` records these
changes as they are introduced.

Releases are feature-driven rather than tied to a fixed calendar. MicroPython maintains a single
evolving release line, not long-term-support branches: security and bug fixes ship in the next
release, and are not backported to older ``1.x`` releases.

Port support levels are documented separately in :ref:`support_tiers`.
