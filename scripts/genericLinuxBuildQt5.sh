#!/bin/bash

# Assumes, that you have installed (static) qt5 to ~/qt5.

QT5_QMAKE=~/qt5/bin/qmake
if ! which $QT5_QMAKE; then
    echo "$QT5_QMAKE not found."
    exit 1
fi

TAR=tar
if ! which $TAR; then
    echo "$TAR not found."
    exit 1
fi

LRELEASE=lrelease
if ! which $LRELEASE; then
    echo "$LRELEASE not found."
    exit 1
fi

NUM_CPUS=$(cat /proc/cpuinfo | grep processor | wc -l)

# Package naming

NAME=heimer
VERSION=${DUSTRAC_RELEASE_VERSION?"DUSTRAC_RELEASE_VERSION_NOT_SET"}
ARCH=linux-x86_64
QT=qt5

# Build

$QT5_QMAKE && make -j$NUM_CPUS || exit 1

# Update translations

$LRELEASE ./src/game/game.pro && $LRELEASE ./src/editor/editor.pro || exit 1

# Install to packaging dir

PACKAGE_PATH=$NAME-$VERSION-$ARCH-$QT
rm -rf $PACKAGE_PATH
mkdir $PACKAGE_PATH

mkdir -p data/translations
cp -v ./src/game/translations/*.qm data/translations   &&
cp -v ./src/editor/translations/*.qm data/translations &&
cp -v ./src/game/heimer-game $PACKAGE_PATH            &&
cp -v ./src/editor/heimer-editor $PACKAGE_PATH        &&
cp -rv data $PACKAGE_PATH                              || exit 1

TEXT_FILES="AUTHORS CHANGELOG COPYING README.md"
cp -v $TEXT_FILES $PACKAGE_PATH || exit 1

# Copy some needed dependecies

DEPS="ogg vorbis openal"
for lib in $DEPS; do
cp -v $(ldd $PACKAGE_PATH/heimer-game | grep $lib | awk '{print $3}') $PACKAGE_PATH
done

# Create start script for the game

cp $PACKAGE_PATH/heimer-game $PACKAGE_PATH/g.bin
SCRIPT=$PACKAGE_PATH/heimer-game
echo "#!/bin/sh" > $SCRIPT
echo "LD_LIBRARY_PATH=. ./g.bin" >> $SCRIPT
chmod 755 $SCRIPT

# Create start script for the editor

cp $PACKAGE_PATH/heimer-editor $PACKAGE_PATH/e.bin
SCRIPT=$PACKAGE_PATH/heimer-editor
echo "#!/bin/sh" > $SCRIPT
echo "LD_LIBRARY_PATH=. ./e.bin" >> $SCRIPT
chmod 755 $SCRIPT

# Create tgz archive

TGZ_ARCHIVE=$PACKAGE_PATH.tar.gz
rm -f $TGZ_ARCHIVE
$TAR czvf $TGZ_ARCHIVE $PACKAGE_PATH

ls -ltr

echo "Done."

