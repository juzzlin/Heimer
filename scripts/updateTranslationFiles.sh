#!/bin/bash

LUPDATE=lupdate
if ! which ${LUPDATE}; then
    echo "${LUPDATE} not found."
    exit 1
fi

for LANG in fi; do
    FILE=src/editor/translations/heimer-editor_${LANG}.ts
    ${LUPDATE} src/editor/*.cpp -ts ${FILE}
done

echo "Done."

