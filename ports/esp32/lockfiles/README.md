# ESP-IDF Component Lockfiles

This directory contains the exact versions of ESP-IDF components that have been
used to build MicroPython. It is updated by the [component version
solver](https://docs.espressif.com/projects/idf-component-manager/en/latest/guides/version_solver.html).

Unless you change the `main/idf_component.yml` file for MicroPython ESP32, files
in this directory should only change contents if you build using a different
ESP-IDF version to the last time the file was updated.

*Please do not commit changes to these files and submit PRs unless the PR needs
to change versions of components or ESP-IDF.*
