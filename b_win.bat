@echo off
setlocal

REM Set Qt installation path
set QT_PATH=C:\Qt\6.9.0\mingw_64

REM Determine build type (debug or release)
set BUILD_TYPE=debug
set CMAKE_BUILD_TYPE=Debug

if "%1"=="--release" (
    set BUILD_TYPE=release
    set CMAKE_BUILD_TYPE=Release
)

REM Create appropriate build folder
set BUILD_DIR=build\%BUILD_TYPE%

REM Clean old build folder if it exists
if exist %BUILD_DIR% (
    echo Removing existing %BUILD_TYPE% build directory...
    rmdir /S /Q %BUILD_DIR%
)

echo Creating %BUILD_TYPE% build directory...
mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo Configuring project with CMake in %BUILD_TYPE% mode...
cmake ..\..\src -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%\lib\cmake" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%

if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    cd ..\..\
    exit /b 1
)

echo Building project with mingw32-make in %BUILD_TYPE% mode...
mingw32-make

if errorlevel 1 (
    echo [ERROR] Build failed.
    cd ..\..\
    exit /b 1
)

echo Deploying Qt runtime dependencies with windeployqt...
REM Add Qt bin directory to PATH temporarily for windeployqt to find the platform plugins
set PATH=%QT_PATH%\bin;%PATH%

REM First, manually copy the platform plugins
echo Manually copying essential Qt plugins...
if not exist platforms mkdir platforms
copy "%QT_PATH%\plugins\platforms\qwindows.dll" platforms\
copy "%QT_PATH%\bin\Qt6Core.dll" .
copy "%QT_PATH%\bin\Qt6Gui.dll" .
copy "%QT_PATH%\bin\Qt6Widgets.dll" .

REM Now try windeployqt again with simpler options
echo Running windeployqt...
"%QT_PATH%\bin\windeployqt.exe" --no-virtualkeyboard --%BUILD_TYPE% NotepadX.exe

if errorlevel 1 (
    echo Windeployqt reported errors, but we'll continue since we've manually copied core files.
    echo Some features might not work correctly.
)

echo [SUCCESS] %BUILD_TYPE% build and deployment complete.
cd ..\..\

