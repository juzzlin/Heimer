#!/bin/bash

# Simple build script that can be run from Jenkins

(mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug && make) || exit 1

