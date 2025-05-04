# Linux packaging configuration
if(UNIX AND NOT APPLE)
    # General settings
    set(CPACK_PACKAGE_NAME "notepadx")
    set(CPACK_PACKAGE_VENDOR "NotepadX")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A cross-platform text editor")
    set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/installer/linux/description.txt")
    set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    
    # Component-based installation
    set(CPACK_COMPONENTS_ALL Applications)
    set(CPACK_COMPONENT_APPLICATIONS_DISPLAY_NAME "NotepadX")
    
    # DEB specific
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your.email@example.com>")
    set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libqt6core6, libqt6gui6, libqt6widgets6, libqt6svg6")
    set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "")
    
    # RPM specific
    set(CPACK_RPM_PACKAGE_LICENSE "GPL-3.0")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")
    set(CPACK_RPM_PACKAGE_REQUIRES "qt6-qtbase >= 6.0.0, qt6-qtsvg >= 6.0.0")
    
    # Install desktop file and icons
    install(FILES "${CMAKE_SOURCE_DIR}/linux/notepadx.desktop"
            DESTINATION "share/applications"
            COMPONENT Applications)
    
    # Install icons in various sizes
    install(FILES "${CMAKE_SOURCE_DIR}/icons/appicons/notepadx_linux.png"
            DESTINATION "share/icons/hicolor/256x256/apps"
            RENAME "notepadx.png"
            COMPONENT Applications)
            
    # Set supported generators for Linux
    set(CPACK_GENERATOR "DEB;RPM")
endif()
