==========================================
Random Number Generator Synchronous driver
==========================================

Random Number Generator (RAND) generates a sequence of numbers that can not
be reasonably predicted better than by a random chance.

In some implementation cases, seed is required for the Random Number Generator
to generate random numbers. rand_sync_set_seed is used to update the seed.
If it's actually not required by the generator implementation, the function just
returns ERR_UNSUPPORTED_OP.

Features
--------

* Initialization and de-initialization
* Enabling and Disabling
* Setting seed
* 8-bit and 32-bit random data/data array generation

Applications
------------

* Generate random keys for data encryption

Dependencies
------------

* Random number generation hardware/software

Concurrency
-----------
N/A

Limitations
-----------

N/A

Known issues and workarounds
----------------------------
N/A

