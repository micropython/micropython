# AWS CBOR

Subset implementation of CBOR (Concise Binary Object Representation)

## Getting started

`make lib` Clones testing and library dependencies

`make` Builds and tests the CBOR code

`make docs` Generates all doxy files in `./build/doxy/html/`

## MAKE Targets

`default `: Build, test, and report coverage summary

`check `: Run static analysis checks

`clean `: Clean build artifacts

`clean-all `: Removes lib and build directories

`clean-lib `: Removes libraries pulled down by git

`cov-summary `: Provides code coverage summary from LLVM

`coverage `: Build, test, and report full coverage results

`debug `: Build and launch GDB

`docs `: Generates documentation

`hn-check `: Checks for Amazon FreeRTOS variable encoding rules

`hn-fix `: Creates fixes for Amazon FreeRTOS variable encoding rules

`lib `: Clones git repositories, this project is dependent upon

`list-obj `: List object files that will be created

`list-src `: List source files that will be used and those that will be statically checked

`test `: Build and test


## Points of interest
\ref aws_cbor.h for the public interface

\ref glossary has various terms and definitions
