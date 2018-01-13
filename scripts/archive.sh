VERSION=$(git log | grep -m 1 "^\s*[0-9]\.[0-9]*\.[0-9]*" | awk '{print $1}')
git archive --format=tar --prefix=dementia-$VERSION/ HEAD | gzip > dementia-$VERSION.tar.gz
git archive --format=zip --prefix=dementia-$VERSION/ HEAD > dementia-$VERSION.zip

