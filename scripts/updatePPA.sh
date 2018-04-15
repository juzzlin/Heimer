VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}

# $VERSION-1 for xenial
# $VERSION-2 for artful

DEBIAN_VERSION=$VERSION-1
rm -rf *${VERSION}*
cp ../DustRacing2D/heimer-$VERSION.tar.gz .
tar xzvf heimer-$VERSION.tar.gz
mv heimer-$VERSION heimer-$DEBIAN_VERSION
cd heimer-$DEBIAN_VERSION
cp -rv packaging/debian .
debuild -S -sa && cd .. && dput ppa:jussi-lind/heimer "heimer_${DEBIAN_VERSION}_source.changes"

