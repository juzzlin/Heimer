CMAKE=/opt/mxe/usr/bin/i686-w64-mingw32.static-cmake

sudo docker run --privileged -t -v $(pwd):/heimer juzzlin/mxe-qt5:latest bash -c "export PATH='$PATH':/opt/mxe/usr/bin && cd heimer && mkdir -p build-mxe-docker && cd build-mxe-docker && ${CMAKE} .. && ${CMAKE} --build . --target all -- -j2"

