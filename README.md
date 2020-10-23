## Heimer

Heimer is a desktop application for creating mind maps and other suitable diagrams. It's written in Qt and targeted for Linux and Windows.

Here is a simple mind map of Heimer itself running on Ubuntu 18.04:

![Heimer screenshot](/screenshots/1.14.0/Heimer.png?raw=true)

<a href="https://www.youtube.com/watch?feature=player_embedded&v=NXJp6tmmZdE">A very short introduction video to Heimer 1.9.0</a>

## Features

* Adjustable grid
* <a href="https://www.youtube.com/watch?feature=player_embedded&v=acQ8CpaCayk">Automatic layout optimization</a>
* Easy-to-use UI
* Export to PNG or SVG
* Forever 100% free
* Full undo/redo
* Nice animations
* Quickly add node text and edge labels
* Save/load in XML-based .ALZ-files
* Translations in English (default), Finnish, French, Italian, Dutch
* Very fast
* Zoom in/out/fit
* Zoom with mouse wheel

## License

Heimer's source code is licensed under GNU GPLv3. 
See COPYING for the complete license text.

All image files, except where otherwise noted, are licensed under
CC BY-SA 3.0: http://creativecommons.org/licenses/by-sa/3.0/

## Installation

See https://github.com/juzzlin/Heimer/releases for available packages.

On Linux distributions that support universal Snap packages you can install Heimer like this:

`$ snap install heimer`

For more information see https://snapcraft.io/heimer and https://docs.snapcraft.io/core/install

Snap is the recommended way to install Heimer on Linux.

## Building the project

Currently the build depends on `Qt5` only (`qt5-default`, `qttools5-dev-tools`, `qttools5-dev`, `libqt5svg5-dev` packages on Ubuntu).

The "official" build system for Linux is `CMake` although `qmake` project files are also provided.

Building for Linux in a nutshell:

`$ mkdir build && cd build`

`$ cmake ..`

`$ make -j4`

Run unit tests:

`$ ctest`

Install locally:

`$ sudo make install`

Debian package (`.deb`) can be created like this:

`$ cpack -G DEB`

See `Jenkinsfile` on how to build other packages in Docker.

## Donations

https://www.patreon.com/juzzlin

