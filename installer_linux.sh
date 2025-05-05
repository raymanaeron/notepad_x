#!/bin/bash
# Linux installer build script for NotepadX

# Exit on error
set -e

# Set Qt installation path - adjust according to your installation
QT_PATH="/opt/Qt/6.9.0/gcc_64"
# Use system Qt if installed by the package manager
if [ ! -d "$QT_PATH" ]; then
    QT_PATH="/usr"
fi

# Create build folder for installer
BUILD_DIR="build/installer"
echo "Creating build directory for installer..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure project with CMake for installer
echo "Configuring project for installer..."
cmake ../.. -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
make -j$(nproc)

# Make sure necessary directories exist for resources
echo "Setting up resource directories..."
mkdir -p "AppDir/usr/bin"
mkdir -p "AppDir/usr/share/applications"
mkdir -p "AppDir/usr/share/icons/hicolor/256x256/apps"
mkdir -p "AppDir/usr/share/metainfo"

# Copy executable and resources
echo "Copying application files..."
cp "NotepadX" "AppDir/usr/bin/"
cp -r "../../icons" "AppDir/usr/share/"

# Create desktop file
echo "Creating desktop file..."
cat > "AppDir/usr/share/applications/notepadx.desktop" << EOF
[Desktop Entry]
Type=Application
Name=NotepadX
GenericName=Text Editor
Comment=A cross-platform text editor
Icon=notepadx
Exec=NotepadX %f
Terminal=false
Categories=Utility;TextEditor;Development;Qt;
MimeType=text/plain;
Keywords=Text;Editor;Notepad;
X-AppImage-Name=NotepadX
X-AppImage-Version=1.0.0
X-AppImage-Arch=x86_64
EOF

# Copy icon for desktop integration
cp "../../icons/appicons/app_icon_linux.png" "AppDir/usr/share/icons/hicolor/256x256/apps/notepadx.png"

# Create AppStream metadata file
echo "Creating AppStream metadata..."
cat > "AppDir/usr/share/metainfo/com.elysianedge.notepadx.appdata.xml" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<component type="desktop-application">
  <id>com.elysianedge.notepadx</id>
  <metadata_license>FSFAP</metadata_license>
  <project_license>GPL-3.0-or-later</project_license>
  <name>NotepadX</name>
  <summary>A cross-platform text editor</summary>
  <description>
    <p>NotepadX is a cross-platform text editor with syntax highlighting for multiple programming languages. It features a clean, modern interface with both light and dark themes.</p>
  </description>
  <launchable type="desktop-id">notepadx.desktop</launchable>
  <url type="homepage">https://elysianedge.com/notepadx</url>
  <provides>
    <binary>NotepadX</binary>
  </provides>
  <developer_name>Elysian Edge LLC</developer_name>
  <releases>
    <release version="1.0.0" date="2023-10-01" />
  </releases>
  <content_rating type="oars-1.1" />
</component>
EOF

echo "Copying dependencies..."
# Check for linuxdeploy and use it if available
if command -v linuxdeploy &> /dev/null && command -v linuxdeploy-plugin-qt &> /dev/null; then
    echo "Using linuxdeploy for dependency bundling..."
    # Download AppImageTool if needed
    if [ ! -f linuxdeploy-x86_64.AppImage ]; then
        wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
        chmod +x linuxdeploy-x86_64.AppImage
    fi
    
    if [ ! -f linuxdeploy-plugin-qt-x86_64.AppImage ]; then
        wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
        chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
    fi
    
    # Create AppImage package
    echo "Creating AppImage..."
    export QMAKE="${QT_PATH}/bin/qmake"
    ./linuxdeploy-x86_64.AppImage --appdir=AppDir --plugin qt --output appimage --desktop-file=AppDir/usr/share/applications/notepadx.desktop
else
    echo "linuxdeploy not found, creating Debian package instead..."
    
    # Create directory structure for Debian package
    echo "Creating Debian package structure..."
    DEBPKG="notepadx_1.0.0_amd64"
    mkdir -p "${DEBPKG}/DEBIAN"
    mkdir -p "${DEBPKG}/usr/bin"
    mkdir -p "${DEBPKG}/usr/share/applications"
    mkdir -p "${DEBPKG}/usr/share/icons/hicolor/256x256/apps"
    
    # Copy application files
    cp "NotepadX" "${DEBPKG}/usr/bin/"
    cp "AppDir/usr/share/applications/notepadx.desktop" "${DEBPKG}/usr/share/applications/"
    cp "AppDir/usr/share/icons/hicolor/256x256/apps/notepadx.png" "${DEBPKG}/usr/share/icons/hicolor/256x256/apps/"
    
    # Create control file
    cat > "${DEBPKG}/DEBIAN/control" << EOF
Package: notepadx
Version: 1.0.0
Section: editors
Priority: optional
Architecture: amd64
Depends: libqt6core6, libqt6gui6, libqt6widgets6, libqt6svg6
Maintainer: Elysian Edge LLC <support@elysianedge.com>
Description: NotepadX
 A cross-platform text editor with syntax highlighting
 for multiple programming languages.
EOF
    
    # Build Debian package
    dpkg-deb --build "${DEBPKG}"
fi

echo "[SUCCESS] Linux installer creation complete."
cd ../..
