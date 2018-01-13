VERSION=${DUSTRAC_RELEASE_VERSION?"is not set."}

# $VERSION-1 for xenial
# $VERSION-2 for artful

DEBIAN_VERSION=$VERSION-1
rm -rf *${VERSION}*
cp ../DustRacing2D/dementia-$VERSION.tar.gz .
tar xzvf dementia-$VERSION.tar.gz
mv dementia-$VERSION dementia-$DEBIAN_VERSION
cd dementia-$DEBIAN_VERSION
cp -rv packaging/debian .
debuild -S -sa && cd .. && dput ppa:jussi-lind/dementia "dementia_${DEBIAN_VERSION}_source.changes"

