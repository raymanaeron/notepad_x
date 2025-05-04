cmake_minimum_required(VERSION 3.5)

project(NotepadX LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt packages
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg) # Add SVG support

# Add the src directory to the include path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# Update source file paths to include src/ prefix
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/editorwidget.cpp
    src/editorwidget.h
    src/linenumberarea.cpp
    src/linenumberarea.h
    src/codeeditor.cpp
    src/codeeditor.h
    src/findreplacedialog.cpp
    src/findreplacedialog.h
    src/gotolinedialog.cpp
    src/gotolinedialog.h
    src/highlighting/syntaxhighlighter.cpp
    src/highlighting/syntaxhighlighter.h
    src/highlighting/languagedata.cpp
    src/highlighting/languagedata.h
    src/highlighting/highlighterfactory.cpp
    src/highlighting/highlighterfactory.h
    src/highlighting/languages/cpphighlighter.cpp
    src/highlighting/languages/cpphighlighter.h
    src/highlighting/languages/rusthighlighter.cpp
    src/highlighting/languages/rusthighlighter.h
    src/highlighting/languages/gohighlighter.cpp
    src/highlighting/languages/gohighlighter.h
    src/highlighting/languages/pythonhighlighter.cpp
    src/highlighting/languages/pythonhighlighter.h
    src/highlighting/languages/csharphighlighter.cpp
    src/highlighting/languages/csharphighlighter.h
    src/highlighting/languages/javahighlighter.cpp
    src/highlighting/languages/javahighlighter.h
    src/highlighting/languages/typescripthighlighter.cpp
    src/highlighting/languages/typescripthighlighter.h
    src/highlighting/languages/javascripthighlighter.cpp
    src/highlighting/languages/javascripthighlighter.h
    src/highlighting/languages/htmlhighlighter.cpp
    src/highlighting/languages/htmlhighlighter.h
    src/highlighting/languages/csshighlighter.cpp
    src/highlighting/languages/csshighlighter.h
    src/highlighting/languages/markuphighlighter.cpp
    src/highlighting/languages/markuphighlighter.h
    src/highlighting/languages/kotlinhighlighter.cpp
    src/highlighting/languages/kotlinhighlighter.h
    src/highlighting/languages/objchighlighter.cpp
    src/highlighting/languages/objchighlighter.h
    src/highlighting/languages/swifthighlighter.cpp
    src/highlighting/languages/swifthighlighter.h
    src/highlighting/languages/shellhighlighter.cpp
    src/highlighting/languages/shellhighlighter.h
    src/highlighting/languages/sqlhighlighter.cpp
    src/highlighting/languages/sqlhighlighter.h
    src/highlighting/languages/phpshighlighter.cpp
    src/highlighting/languages/phpshighlighter.h
    src/highlighting/languages/rubyhighlighter.cpp
    src/highlighting/languages/rubyhighlighter.h
    src/highlighting/languages/yamlhighlighter.cpp
    src/highlighting/languages/yamlhighlighter.h
    src/highlighting/languages/jsonhighlighter.cpp
    src/highlighting/languages/jsonhighlighter.h
    src/highlighting/languages/xmlhighlighter.cpp
    src/highlighting/languages/xmlhighlighter.h
    src/highlighting/languages/luahighlighter.cpp
    src/highlighting/languages/luahighlighter.h
    resources.qrc
)

# For Windows, add icon to the executable
if(WIN32)
    # Add resource file for Windows icon
    set(WIN_RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/app_icon.rc)
    set(SOURCES ${SOURCES} ${WIN_RC_FILE})
endif()

# Set GUI application properties for different platforms
if(WIN32)
    # Create a GUI application without console window on Windows
    add_executable(NotepadX WIN32 ${SOURCES})
elseif(APPLE)
    # On macOS, create a regular .app bundle
    add_executable(NotepadX MACOSX_BUNDLE ${SOURCES})
    
    # Add icon resources to the bundle
    set(MACOSX_ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/icons/appicons/app_icon_mac.icns)
    set_source_files_properties(${MACOSX_ICON_FILE} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(NotepadX PRIVATE ${MACOSX_ICON_FILE})
    
    # Set additional bundle properties for macOS
    set_target_properties(NotepadX PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.notepadx.app"
        MACOSX_BUNDLE_BUNDLE_NAME "NotepadX"
        MACOSX_BUNDLE_BUNDLE_VERSION "1.0.0"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_INFO_STRING "NotepadX - A cross-platform text editor"
        MACOSX_BUNDLE_COPYRIGHT "Copyright © 2025"
        MACOSX_BUNDLE_ICON_FILE "app_icon_mac.icns"
    )
else()
    # Regular executable for Linux and other platforms
    add_executable(NotepadX ${SOURCES})
endif()

# Make sure resources.qrc is included in the build
target_sources(NotepadX PRIVATE resources.qrc)

# Link libraries
target_link_libraries(NotepadX PRIVATE 
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
)

# Installer configuration
# Include platform-specific installer configs
include(cmake/windows_installer.cmake OPTIONAL)
include(cmake/macos_installer.cmake OPTIONAL)
include(cmake/linux_installer.cmake OPTIONAL)

# Enable CPack
include(CPack)
