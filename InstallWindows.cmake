# **** Instal targets for Windows ****
function(setup_install_targets)

    message(STATUS "Windows build.")

    set(BIN_PATH .)
    set(DOC_PATH .)

    # Add target to copy files to the binary dir.
    add_custom_target(docs ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.md
        DEPENDS ${BINARY_NAME})

    # Add target to copy data files to the binary dir.
    add_custom_target(data ALL
        COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
        DEPENDS ${BINARY_NAME})

    install(PROGRAMS ${CMAKE_BINARY_DIR}/${BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README.md DESTINATION ${DOC_PATH})

    set(CPACK_PACKAGE_INSTALL_DIRECTORY Heimer)

    set(CPACK_NSIS_MUI_ICON ${CMAKE_SOURCE_DIR}/data/icons/heimer.ico)
    set(CPACK_NSIS_MUI_UNIICON ${CMAKE_SOURCE_DIR}/data/icons/heimer.ico)
    set(CPACK_NSIS_DISPLAY_NAME Heimer)
    set(CPACK_NSIS_PACKAGE_NAME Heimer)
    set(CPACK_NSIS_HELP_LINK http://juzzlin.github.io/Heimer/)
    set(CPACK_NSIS_URL_INFO_ABOUT http://juzzlin.github.io/Heimer/)

    set(CPACK_NSIS_CREATE_ICONS_EXTRA
        "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${BINARY_NAME}.lnk' '$INSTDIR\\\\${BINARY_NAME}.exe'"
    )

    set(CPACK_NSIS_DELETE_ICONS_EXTRA
        "Delete '$SMPROGRAMS\\\\$START_MENU\\\\${BINARY_NAME}.lnk'"
    )

    include(CPack)

endfunction()

