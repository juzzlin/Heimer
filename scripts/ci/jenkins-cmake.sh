#!/bin/bash

# Simple build script that can be run from Jenkins

(mkdir -p build && cd build && cmake .. && make) || exit 1

