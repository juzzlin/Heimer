VERSION=$(git log | grep -m 1 "^\s*[0-9]\.[0-9]*\.[0-9]*" | awk '{print $1}')
git archive --format=tar --prefix=heimer-$VERSION/ HEAD | gzip > heimer-$VERSION.tar.gz
git archive --format=zip --prefix=heimer-$VERSION/ HEAD > heimer-$VERSION.zip

