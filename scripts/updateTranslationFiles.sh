#!/bin/bash

LUPDATE=lupdate
if ! which ${LUPDATE}; then
    echo "${LUPDATE} not found."
    exit 1
fi

for LANG in fi; do
    FILE=src/translations/heimer_${LANG}.ts
    ${LUPDATE} src/*.cpp -ts ${FILE}
done

echo "Done."

