#!/bin/bash

# Builds Snap package in Docker

CMD="export LANG=en_US.UTF-8 && \
     export LC_ALL=en_US.UTF-8 && \
     mkdir -p /heimer/build-snap && \
     cd /heimer/build-snap && \
     cp ../snapcraft.yaml . && \
     snapcraft clean gnu-heimer -s pull && snapcraft"

if [ -f /.dockerenv ]; then
    echo "Script inside Docker"
    bash -c "${CMD}"
else
    echo "Script outside Docker"
    docker run --privileged -t -v $(pwd):/heimer juzzlin/qt5:16.04 bash -c "${CMD}"
fi

