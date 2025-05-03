@echo off
setlocal

REM Set Qt installation path
set QT_PATH=C:\Qt\6.9.0\mingw_64

REM Clean old build folder if it exists
if exist build (
    echo Removing existing build directory...
    rmdir /S /Q build
)

echo Creating build directory...
mkdir build
cd build

echo Configuring project with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%\lib\cmake"

if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

echo Building project with mingw32-make...
mingw32-make

if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo Deploying Qt runtime dependencies with windeployqt...
"%QT_PATH%\bin\windeployqt.exe" NotepadX.exe

if errorlevel 1 (
    echo [ERROR] windeployqt failed.
    exit /b 1
)

echo [SUCCESS] Build and deployment complete.
pause
