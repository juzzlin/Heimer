#!/bin/bash

#
# This for configuring static build of Qt5 itself, not the game.
#

PREFIX='/home/juzzlin/qt5'
./configure \
            -opensource \
            -confirm-license \
            -force-pkg-config \
            -fontconfig \
            -release \
            -static \
            -prefix $PREFIX \
            -no-icu \
            -opengl desktop \
            -no-glib \
            -accessibility \
            -nomake examples \
            -nomake tests \
            -qt-zlib \
            -qt-libpng \
            -qt-libjpeg \
            -qt-sql-sqlite \
            -qt-xcb \
            -qt-pcre \
            -pulseaudio \
            -v

