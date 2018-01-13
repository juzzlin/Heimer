#!/bin/bash

LUPDATE=lupdate
if ! which ${LUPDATE}; then
    echo "${LUPDATE} not found."
    exit 1
fi

for LANG in cs de fi fr it; do
    ${LUPDATE} src/game/*.cpp src/game/menu/*.cpp -ts src/game/translations/dementia-game_${LANG}.ts
    ${LUPDATE} src/editor/*.cpp -ts src/editor/translations/dementia-editor_${LANG}.ts
done

echo "Done."

