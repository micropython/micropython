Configuration
=============

Standard Library
----------------

The :doc:`../library-reference` is configured at compile time using
defines named ``CONFIG_*``. The default configuration includes most
functionality, as most application wants that. If an application has
special requirements, for example memory constraints, it has to be
configured to remove unnecessaray functionality.

Search order
^^^^^^^^^^^^

Highest priority first.

1. Command line as ``CDEFS_EXTRA="<configuration variable>=<value>"``.

2. A file named ``pumbaa_config.h`` in the application root folder.

3. The default configuration file, :github-blob:`src/pumbaa_config_default.h`.

Variables
^^^^^^^^^

All configuration variables are listed below. Their default values are
defined in :github-blob:`src/pumbaa_config_default.h`.
