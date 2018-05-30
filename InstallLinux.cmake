# **** Resolve install and data paths ****
function(resolve_install_paths)

    # Set default install paths for release builds.
    set(DEFAULT_DATA_PATH_BASE share/applications/heimer)
    if(NOT BIN_PATH)
        set(BIN_PATH bin)
    endif()

    message(STATUS "Linux/Unix build with system install targets.")

    message(STATUS "BIN_PATH: " ${BIN_PATH})

    setup_install_targets(${BIN_PATH})

endfunction()

# **** Install targets for Linux ****
function(setup_install_targets BIN_PATH)

    # Install binaries and data
    install(PROGRAMS ${CMAKE_BINARY_DIR}/${BINARY_NAME} DESTINATION ${BIN_PATH})

    # Install .desktop files
    install(FILES ${CMAKE_BINARY_DIR}/heimer.desktop DESTINATION share/applications)

    # Install app store meta data
    install(FILES src/heimer.appdata.xml DESTINATION share/metainfo)

    # Install icons
    install(FILES data/icons/heimer.png DESTINATION share/pixmaps)
    install(FILES data/icons/heimer.png DESTINATION share/icons/hicolor/64x64/apps)

    # CPack config to create e.g. self-extracting packages
    set(CPACK_BINARY_STGZ ON)
    set(CPACK_BINARY_TGZ ON)
    set(CPACK_BINARY_TZ OFF)

    set(QT_VER_STR "qt5")

    set(CPACK_PACKAGE_FILE_NAME "heimer-${VERSION}-linux-${CMAKE_HOST_SYSTEM_PROCESSOR}-${QT_VER_STR}")
    set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
    set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README.md)

    include(CPack)

endfunction()
