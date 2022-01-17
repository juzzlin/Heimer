## Heimer

Heimer is a desktop application for creating mind maps and other suitable diagrams. It's written in Qt and targeted for Linux and Windows.

Here is a simple mind map of Heimer itself running on Ubuntu 18.04:

![Heimer screenshot](/screenshots/1.14.0/Heimer.png?raw=true)

<a href="https://www.youtube.com/watch?feature=player_embedded&v=NXJp6tmmZdE">A very short introduction video to Heimer 1.9.0</a>

## Features

* Adjustable grid
* <a href="https://www.youtube.com/watch?feature=player_embedded&v=acQ8CpaCayk">Automatic layout optimization</a>
* Autosave
* Easy-to-use UI
* Export to PNG or SVG
* Forever 100% free
* Full undo/redo
* Nice animations
* Quickly add node text and edge labels
* Save/load in XML-based .ALZ-files
* Translations in English (default), Chinese, Dutch, Finnish, French, German, Italian, Spanish
* Very fast
* Zoom in/out/fit
* Zoom with mouse wheel

## Donations

It takes a lot of effort to develop and maintain Heimer, so if you find the application useful it would be very much appreciated to tip the project and help to keep the development active.

It's easy to donate via PayPal:

<a href="https://paypal.me/pools/c/8yvW5nEMmq">paypal.me/pools/c/8yvW5nEMmq</a>

Thanks! :)

## License

Heimer's source code is licensed under GNU GPLv3. 
See COPYING for the complete license text.

All image files, except where otherwise noted, are licensed under
CC BY-SA 3.0: http://creativecommons.org/licenses/by-sa/3.0/

## Installation

See https://github.com/juzzlin/Heimer/releases for available pre-built packages.

### Linux: Snap

On Linux distributions that support universal Snap packages you can install Heimer like this:

`$ snap install heimer`

For more information see https://snapcraft.io/heimer and https://docs.snapcraft.io/core/install

Snap is the recommended way to install Heimer on Linux.

### Linux: Deb

There are Debian packages for Ubuntu/Debian. Use some graphical tool to install, or as an example on `Ubuntu 20.04`:

`$ sudo apt install ./heimer-2.5.0-ubuntu-20.04_amd64.deb`

### Linux: AppImage

`AppImage` is a "universal" package that can (in theory) be run on all Linux platforms:

Make the image executable e.g. like this:

`$ chmod 755 Heimer-2.4.0-x86_64.AppImage`

Run:

`$ ./Heimer-2.4.0-x86_64.AppImage`

### Windows

For Windows there's an installer and alternatively a ZIP-archive that just contains the Heimer executable.

## Building the project

Currently the build depends on `Qt 5` only (`qt5-default`, `qttools5-dev-tools`, `qttools5-dev`, `libqt5svg5-dev` packages on Ubuntu). Support for `Qt 6` is preliminary and should work with `CMake`.

### Linux / Unix

The "official" build system for Linux is `CMake`.

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

### Windows

The NSIS installer for Windows is currently built in Docker with MXE (http://mxe.cc):

`$ ./scripts/build-windows-nsis`

This is so very cool! A Windowsless Windows build!

Anyway, as the project depends only on Qt SDK you can use your favourite Qt setup to build the project on Windows.

### Docker environments

Needed Dockerfiles can be found at https://github.com/juzzlin/Dockerfiles

