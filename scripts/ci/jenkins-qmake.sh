#!/bin/bash

# Simple build script that can be run from Jenkins

(mkdir -p build && cd build && qmake .. && make) || exit 1

