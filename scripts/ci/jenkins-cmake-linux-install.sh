#!/bin/bash

# Simple build script that can be run from Jenkins

(mkdir -p build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release -DReleaseBuild=ON && make) || exit 1

