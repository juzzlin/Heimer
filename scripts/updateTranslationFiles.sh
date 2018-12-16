#!/bin/bash

if [[ ! "$1" ]]; then
    echo "Usage: ./updateTranslationFiles.sh [LANG]"
    exit 1
fi

LANG=$1

LUPDATE=lupdate
if ! which ${LUPDATE}; then
    echo "${LUPDATE} not found."
    exit 1
fi

LRELEASE=lrelease
if ! which ${LRELEASE}; then
    echo "${LRELEASE} not found."
    exit 1
fi

LINGUIST=linguist
if ! which ${LINGUIST}; then
    echo "${LINGUIST} not found."
    exit 1
fi

echo "Updating files for ${LANG}"

TS_FILE=src/translations/heimer_${LANG}.ts
QM_FILE=data/translations/heimer_${LANG}.qm
${LUPDATE} src/*.cpp -ts ${TS_FILE}

echo "Opening ${LINGUIST}.."

${LINGUIST} ${TS_FILE}

echo "Releasing.."

${LRELEASE} ${TS_FILE} -qm ${QM_FILE}

echo "Done."

