@echo off
setlocal enabledelayedexpansion

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

REM Set the specific NSIS path
set NSIS_PATH=C:\Program Files (x86)\NSIS
echo Using NSIS from: %NSIS_PATH%

REM Add NSIS to PATH temporarily
set PATH=%NSIS_PATH%;%PATH%

REM Build the installer using a direct NSIS script
echo Building installer using makensis...

REM Create a simple NSIS script file directly
echo Creating direct_installer.nsi...

REM Create the script by writing directly to the file (avoids echo issues)
(
echo Name "NotepadX"
echo OutFile "NotepadX-1.0.0-Setup.exe"
echo InstallDir "$PROGRAMFILES64\NotepadX"
echo RequestExecutionLevel admin
echo.
echo Section
echo     SetOutPath "$INSTDIR"
echo     File "NotepadX.exe"
echo     File "*.dll"
echo.    
echo     SetOutPath "$INSTDIR\icons"
echo     File /r "icons\*.*"
echo     SetOutPath "$INSTDIR\platforms"
echo     File /r "platforms\*.*"
echo     SetOutPath "$INSTDIR\imageformats"
echo     File /r "imageformats\*.*"
echo.
echo     WriteUninstaller "$INSTDIR\uninstall.exe"
echo     CreateDirectory "$SMPROGRAMS\NotepadX"
echo     CreateShortcut "$SMPROGRAMS\NotepadX\NotepadX.lnk" "$INSTDIR\NotepadX.exe"
echo     CreateShortcut "$DESKTOP\NotepadX.lnk" "$INSTDIR\NotepadX.exe"
echo SectionEnd
echo.
echo Section "Uninstall"
echo     Delete "$INSTDIR\NotepadX.exe"
echo     Delete "$INSTDIR\*.dll"
echo     RMDir /r "$INSTDIR\icons"
echo     RMDir /r "$INSTDIR\platforms"
echo     RMDir /r "$INSTDIR\imageformats"
echo     Delete "$INSTDIR\uninstall.exe"
echo     RMDir "$INSTDIR"
echo     Delete "$SMPROGRAMS\NotepadX\NotepadX.lnk"
echo     Delete "$DESKTOP\NotepadX.lnk"
echo     RMDir "$SMPROGRAMS\NotepadX"
echo SectionEnd
) > simple_installer.nsi

REM Run makensis with this script
echo Running makensis with simple_installer.nsi...
makensis.exe simple_installer.nsi

if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to create installer with makensis
    echo The application has been built and can be found at: %CD%\NotepadX.exe
) else (
    echo [SUCCESS] Installer created successfully.
    echo Installer location: %CD%\NotepadX-1.0.0-Setup.exe
)

cd ..\..\