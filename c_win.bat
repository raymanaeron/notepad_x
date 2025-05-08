@echo off
:: filepath: c:\Code\notepad_x\c_win.bat
:: c_win.bat - Clean script for Windows builds
:: Removes debug and release folders from build directory

:: Print header
echo 🧹 Cleaning NotepadX build folders...

:: Check if build directory exists
if not exist build (
    echo No build directory found. Nothing to clean.
    exit /b 0
)

:: Remove debug folder if it exists
if exist build\debug (
    echo Removing debug build folder...
    rmdir /s /q build\debug
    echo ✅ Debug build folder removed.
) else (
    echo No debug build folder found.
)

:: Remove release folder if it exists
if exist build\release (
    echo Removing release build folder...
    rmdir /s /q build\release
    echo ✅ Release build folder removed.
) else (
    echo No release build folder found.
)

:: Also clean installer folder if it exists
if exist build\installer (
    echo Removing installer build folder...
    rmdir /s /q build\installer
    echo ✅ Installer build folder removed.
) else (
    echo No installer build folder found.
)

echo 🎉 Cleaning complete! All build folders have been removed.
echo Run 'b_win.bat' to create a new debug build
echo Run 'b_win.bat --release' to create a new release build