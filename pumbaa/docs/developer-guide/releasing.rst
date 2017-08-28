Releasing
=========

Follow these steps to create a new release:

1. Write the new version in ``VERSION.txt``. The version should hace
   the format ``<major>.<minor>.<revision>``.

   Increment ``<major>`` for non-backwards compatible changes.

   Increment ``<minor>`` for new features.

   Increment ``<revision>`` for bug fixes.

2. Run the test suites and generate the documentation.

   .. code:: text

      make test

3. Generate files for Arduino.

   .. code:: text

      make arduino

4. Add the new releases to
   ``make/arduino/<family>/package_pumbaa_<family>_index.json``. The
   sha256 sums of the zip-archives are calculated by ``make arduino``
   and written to ``pumbaa-arduino/*.sha256``.

6. Commit the changes, and tag the commit with the new version.

7. Push the new commit and tag.

8. Copy the Pumbaa Arduino releases to SourceForge.

   .. code:: text

      scp pumbaa-arduino/pumbaa-arduino-sam-*.zip <user>@frs.sourceforge.net:/home/frs/project/pumbaa-arduino/sam
      scp make/arduino/sam/package_pumbaa_sam_index.json <user>@frs.sourceforge.net:/home/frs/project/pumbaa-arduino/sam

9. Done.
