REM filepath: c:\Code\notepad_x\b_win.bat
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

echo Creating %BUILD_TYPE% build directory...
mkdir %BUILD_DIR%
cd %BUILD_DIR%

echo Configuring project with CMake in %BUILD_TYPE% mode...
cmake ..\.. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_PATH%\lib\cmake" -DCMAKE_BUILD_TYPE=%CMAKE_BUILD_TYPE%

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

REM Copy icons folder to build directory
echo Copying icons to executable directory...
if not exist icons mkdir icons
if not exist icons\appicons mkdir icons\appicons

REM Copy the app icon to icons directory
echo Copying Windows app icon...
xcopy /Y "..\..\icons\appicons\app_icon_win.ico" "icons\appicons\"
xcopy /Y /E /I "..\..\icons" "icons\"

REM Explicitly verify ICO file was copied
echo Checking app icon...
if exist icons\appicons\app_icon_win.ico (
    echo Windows app icon found at icons\appicons\app_icon_win.ico
) else (
    echo WARNING: Windows app icon not found, app will use default icon
)

REM Verify icons were copied
dir icons\*.svg

REM Create appicons directory
echo Copying application icons...
if not exist icons\appicons mkdir icons\appicons
xcopy /Y /E /I "..\..\icons\appicons" "icons\appicons\"

REM For Windows, also copy PNG as ICO if ICO doesn't exist
if exist icons\appicons\notepadx_win.png (
    if not exist icons\appicons\notepadx_win.ico (
        echo Creating ICO file from PNG for Windows application icon...
        copy icons\appicons\notepadx_win.png icons\appicons\notepadx_win.ico
        echo Note: For best results, create a proper .ico file with multiple resolutions
    )
)

echo Deploying Qt runtime dependencies with windeployqt...
REM Add Qt bin directory to PATH temporarily for windeployqt to find the platform plugins
set PATH=%QT_PATH%\bin;%PATH%

REM First, manually copy the platform plugins and SVG plugin
echo Manually copying essential Qt plugins...
if not exist platforms mkdir platforms
copy "%QT_PATH%\plugins\platforms\qwindows.dll" platforms\
copy "%QT_PATH%\bin\Qt6Core.dll" .
copy "%QT_PATH%\bin\Qt6Gui.dll" .
copy "%QT_PATH%\bin\Qt6Widgets.dll" .
copy "%QT_PATH%\bin\Qt6Svg.dll" .

REM Create imageformats directory and copy SVG plugin
if not exist imageformats mkdir imageformats
copy "%QT_PATH%\plugins\imageformats\qsvg.dll" imageformats\

REM Now try windeployqt again with simpler options
echo Running windeployqt...
"%QT_PATH%\bin\windeployqt.exe" --no-virtualkeyboard --%BUILD_TYPE% NotepadX.exe

if errorlevel 1 (
    echo Windeployqt reported errors, but we'll continue since we've manually copied core files.
    echo Some features might not work correctly.
)

echo [SUCCESS] %BUILD_TYPE% build and deployment complete.
cd ..\..\