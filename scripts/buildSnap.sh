#!/bin/bash

# Builds Snap package in Docker

CMD="export LANG=en_US.UTF-8 && \
     export LC_ALL=en_US.UTF-8 && \
     cd /heimer/packaging/snap && \
     snapcraft"

if [ -f /.dockerenv ]; then
    echo "Script inside Docker"
    bash -c "${CMD}"
else
    echo "Script outside Docker"
    docker run --privileged -t -v $(pwd):/heimer juzzlin/qt5:16.04 bash -c "${CMD}"
fi

