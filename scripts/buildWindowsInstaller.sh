#!/bin/bash

# Builds NSIS installer for Windows in Docker

QMAKE=/opt/mxe/usr/bin/i686-w64-mingw32.static-qmake-qt5

CMD="export LANG=en_US.UTF-8 && export LC_ALL=en_US.UTF-8 && export PATH='$PATH':/opt/mxe/usr/bin && cd /heimer && mkdir -p build-windows-docker && cd build-windows-docker && ${QMAKE} .. && make -j3"

if [ -f /.dockerenv ]; then
    echo "Script inside Docker"
    bash -c "${CMD}"
else
    echo "Script outside Docker"
    docker run --privileged -t -v $(pwd):/heimer juzzlin/mxe-qt5:latest bash -c "${CMD}"
fi

