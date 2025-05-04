# Windows NSIS installer configuration
if(WIN32)
    # Find NSIS
    find_program(NSIS_MAKE "makensis")
    
    set(CPACK_GENERATOR "NSIS")
    set(CPACK_PACKAGE_NAME "NotepadX")
    set(CPACK_PACKAGE_VENDOR "NotepadX")
    set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NotepadX - A cross-platform text editor")
    set(CPACK_PACKAGE_VERSION "1.0.0")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "NotepadX")
    
    # NSIS specific settings
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/icons/appicons/app_icon_win.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/icons/appicons/app_icon_win.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "NotepadX.exe")
    set(CPACK_NSIS_DISPLAY_NAME "NotepadX")
    set(CPACK_NSIS_HELP_LINK "https://notepadx.com")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://notepadx.com")
    set(CPACK_NSIS_CREATE_ICONS_EXTRA "CreateShortCut '$DESKTOP\\\\NotepadX.lnk' '$INSTDIR\\\\NotepadX.exe'")
    
    # File associations
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
        WriteRegStr HKCR '\\\\.txt' '' 'NotepadX.Document'
        WriteRegStr HKCR 'NotepadX.Document' '' 'NotepadX Document'
        WriteRegStr HKCR 'NotepadX.Document\\\\DefaultIcon' '' '$INSTDIR\\\\NotepadX.exe,0'
        WriteRegStr HKCR 'NotepadX.Document\\\\shell\\\\open\\\\command' '' '$INSTDIR\\\\NotepadX.exe \\\"%1\\\"'
    ")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
        DeleteRegKey HKCR '\\\\.txt'
        DeleteRegKey HKCR 'NotepadX.Document'
    ")
endif()
