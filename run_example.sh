#!/bin/bash
gcc -o /tmp/example_avx256rotl $(dirname $(readlink -m $0))/example_avx256rotl.c -mavx2
/tmp/example_avx256rotl
