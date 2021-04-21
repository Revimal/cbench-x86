<!-- SPDX-License-Identifier:	MIT -->

# cbench-x86
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Header-only library](https://img.shields.io/badge/Header--only_library-green.svg)](https://en.wikipedia.org/wiki/Header-only)

# About 'cbench-x86'
This is a simple header file targeting evaluation of computation overhead between C code snippets.
It also contains some helper macros for implementing a benchmark easier, such as the CPU cache line alignment.

# Supported environment
* AMD64 (x86-64); must be compiled with GCC/CLANG.
i386(x86) and MSVC supports are in the plan.

# Example code
Reference 'example_avx256rotl.c' source file.
It is an example code for the performance comparison of an AVX2 based 'bit-rotate-to-left' implementation to the traditional 'bit-rotate-to-left'.

# Contributing guide
#### Bug reporting
* **Be sure the bug was not already reported on [Issues](https://github.com/Revimal/cbench-x86/issues).**
* If the issue seems not duplicated, **open a new one from [Here](https://github.com/Revimal/cbench-x86/issues/new).**
* Attaching additional information such as core-dump makes maintainers happier.

#### Pull request
* **Open a new [Pull Request](https://github.com/Revimal/cbench-x86/pulls).**

#### Contributing rewards
* When we meet someday, I'll buy you a pint of chilling beer.