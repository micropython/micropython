# ESP-IDF Component Lockfiles

This directory contains the exact versions of ESP-IDF components that have been
used to build MicroPython. It is updated by the [component version
solver](https://docs.espressif.com/projects/idf-component-manager/en/latest/guides/version_solver.html).

Unless you change the `main/idf_component.yml` file for MicroPython ESP32, files
in this directory should only change contents if you build using a different
ESP-IDF version to the last time the file was updated.

*Please do not commit changes to these files and submit PRs unless the PR needs
to change versions of components or ESP-IDF.*

## How to update a component

To update a component to the newest version and submit the change back to
MicroPython:

1. Edit `main/idf_component.yml` with the new version.
2. Build and test the change on your preferred board.
3. Build one board for each supported chip
   (i.e. `make BOARD=ESP32_GENERIC; make BOARD=ESP32_GENERIC_S2; make BOARD=ESP32_GENERIC_S3`)
4. Commit the idf_component.yml change and all of the changed lockfiles in this
   directory.

