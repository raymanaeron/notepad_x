@echo off
setlocal

REM Set Qt installation path
set QT_PATH=C:\Qt\6.9.0\mingw_64

REM Create build folder for installer
set BUILD_DIR=build\installer
echo Creating build directory for installer...
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

REM Configure project with CMake for installer
echo Configuring project for installer...
cmake ..\.. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%\lib\cmake" -DCMAKE_BUILD_TYPE=Release

echo Building project...
mingw32-make

REM Make sure necessary directories exist for resources
echo Setting up resource directories...
if not exist icons mkdir icons
if not exist icons\appicons mkdir icons\appicons

REM Copy resources
echo Copying resources...
xcopy /Y /E /I "..\..\icons" "icons\"

REM Add Qt bin directory to PATH temporarily
set PATH=%QT_PATH%\bin;%PATH%

REM Deploy Qt dependencies
echo Deploying Qt runtime dependencies...
windeployqt --release NotepadX.exe

REM Build the installer package
echo Building installer...
cpack -G NSIS

echo Installer creation complete.
cd ..\..\
