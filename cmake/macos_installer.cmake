# macOS DMG package configuration
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
    set(CPACK_PACKAGE_NAME "NotepadX")
    set(CPACK_PACKAGE_VENDOR "NotepadX")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NotepadX - A cross-platform text editor")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    
    # DMG background and icon positioning
    set(CPACK_DMG_BACKGROUND_IMAGE "${CMAKE_SOURCE_DIR}/installer/mac/dmg-background.png")
    set(CPACK_DMG_DS_STORE_SETUP_SCRIPT "${CMAKE_SOURCE_DIR}/installer/mac/dmg-setup.scpt")
    
    # Bundle directory structure
    set(CPACK_BINARY_DRAGNDROP ON)
    
    # Add a symlink to /Applications
    set(CPACK_DMG_VOLUME_NAME "NotepadX")
    set(CPACK_DMG_FORMAT "UDZO")
    set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/icons/appicons/app_icon_mac.icns")
endif()
