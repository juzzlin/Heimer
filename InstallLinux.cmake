# **** Resolve install and data paths ****
function(resolve_install_paths)

    # Set default install paths for release builds.
    set(DEFAULT_DATA_PATH_BASE share/applications/dementia)
    if(NOT DATA_PATH)
        set(DATA_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/data)
    endif()

    if(NOT BIN_PATH)
        set(BIN_PATH bin)
    endif()

    if(NOT DOC_PATH)
        set(DOC_PATH ${CMAKE_INSTALL_PREFIX}/${DEFAULT_DATA_PATH_BASE}/)
    endif()

    if(ReleaseBuild)
        message(STATUS "Linux release build with system install targets.")
    else()
        message(STATUS "Linux development build with local install targets.")
        set(BIN_PATH .)
        set(DATA_PATH ./data)
        set(DOC_PATH .)

        # Add target to copy runtime files to the binary dir.
        add_custom_target(runtime ALL
            COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/data
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/AUTHORS ${CMAKE_BINARY_DIR}/AUTHORS
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/CHANGELOG ${CMAKE_BINARY_DIR}/CHANGELOG
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/COPYING ${CMAKE_BINARY_DIR}/COPYING
            COMMAND cmake -E copy ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_BINARY_DIR}/README.md
            DEPENDS ${GAME_BINARY_NAME} ${QM_FILES})
    endif()

    message(STATUS "BIN_PATH: " ${BIN_PATH})
    message(STATUS "DATA_PATH: " ${DATA_PATH})
    message(STATUS "DOC_PATH:" ${DOC_PATH})

    # This is the main data path
    add_definitions(-DDATA_PATH="${DATA_PATH}")

    setup_install_targets(${BIN_PATH} ${DATA_PATH} ${DOC_PATH})

endfunction()

# **** Install targets for Linux ****
function(setup_install_targets BIN_PATH DATA_PATH DOC_PATH)

    # Install binaries and data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${BINARY_NAME} DESTINATION ${BIN_PATH})
    install(FILES AUTHORS CHANGELOG COPYING README.md DESTINATION ${DOC_PATH})
    install(DIRECTORY ${CMAKE_BINARY_DIR}/data/translations DESTINATION ${DATA_PATH} FILES_MATCHING PATTERN "*.qm")

    if(ReleaseBuild)
        # Install .desktop files
        install(FILES ${CMAKE_BINARY_DIR}/dementia.desktop DESTINATION share/applications)

        # Install app store meta data
        install(FILES src/dementia.appdata.xml DESTINATION share/metainfo)

        # Install icons
        install(FILES data/icons/icon.png DESTINATION share/pixmaps)
        install(FILES data/icons/icon.png DESTINATION share/icons/hicolor/64x64/apps)
    endif()

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ ON)
    set(CPACK_BINARY_TGZ ON)
    set(CPACK_BINARY_TZ OFF)

    set(QT_VER_STR "qt5")

    set(CPACK_PACKAGE_FILE_NAME "dementia-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

    if(NOT ReleaseBuild)
        include(CPack)
    endif()

endfunction()
