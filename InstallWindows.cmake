# **** Resolve install and data paths ****
function(resolve_install_paths)

    message(STATUS "Windows build.")

    set(BIN_PATH .)
    set(DOC_PATH .)

    # Add target to copy files to the binary dir.
    add_custom_target(docs ALL
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
        COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.md
        DEPENDS ${BINARY_NAME})

    setup_install_targets(${BIN_PATH} ${DOC_PATH})

endfunction()

# **** Instal targets for Windows ****
function(setup_install_targets BIN_PATH DOC_PATH)

    # Install binaries and game data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${BINARY_NAME}.exe DESTINATION ${BIN_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README.md DESTINATION ${DOC_PATH})

endfunction()

