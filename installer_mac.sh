#!/bin/bash
# macOS installer build script for NotepadX

# Exit on error
set -e

# Set Qt installation path - adjust according to your installation
QT_PATH="$HOME/Qt/6.9.0/macos"

# Create build folder for installer
BUILD_DIR="build/installer"
echo "Creating build directory for installer..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure project with CMake for installer
echo "Configuring project for installer..."
cmake ../.. -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
make -j$(sysctl -n hw.ncpu)

# Add resources
echo "Setting up resource directories..."
mkdir -p "NotepadX.app/Contents/Resources/icons"
rsync -av "../../icons/" "NotepadX.app/Contents/Resources/icons/"

# Add Qt bin directory to PATH temporarily
export PATH="${QT_PATH}/bin:$PATH"

# Deploy Qt dependencies
echo "Deploying Qt runtime dependencies..."
macdeployqt NotepadX.app -verbose=2

# Update Info.plist with proper metadata
echo "Updating application metadata..."
PLIST="NotepadX.app/Contents/Info.plist"

# Use PlistBuddy to modify Info.plist
/usr/libexec/PlistBuddy -c "Set :CFBundleIdentifier com.elysianedge.notepadx" $PLIST
/usr/libexec/PlistBuddy -c "Set :CFBundleName NotepadX" $PLIST
/usr/libexec/PlistBuddy -c "Set :CFBundleDisplayName NotepadX" $PLIST
/usr/libexec/PlistBuddy -c "Set :CFBundleShortVersionString 1.0.0" $PLIST
/usr/libexec/PlistBuddy -c "Set :CFBundleVersion 1.0.0" $PLIST
/usr/libexec/PlistBuddy -c "Set :NSHumanReadableCopyright Copyright © 2023 Elysian Edge LLC" $PLIST

# Create DMG installer
echo "Creating DMG installer..."
# Install create-dmg if needed: brew install create-dmg
if command -v create-dmg &> /dev/null; then
    create-dmg \
        --volname "NotepadX" \
        --volicon "../../icons/appicons/app_icon_mac.icns" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "NotepadX.app" 150 190 \
        --hide-extension "NotepadX.app" \
        --app-drop-link 450 190 \
        "NotepadX-1.0.0.dmg" \
        "NotepadX.app"
else
    # Fallback to hdiutil
    echo "create-dmg not found, using hdiutil for simple DMG creation..."
    hdiutil create -volname "NotepadX" -srcfolder "NotepadX.app" -ov -format UDZO "NotepadX-1.0.0.dmg"
fi

echo "[SUCCESS] macOS installer created: NotepadX-1.0.0.dmg"
cd ../..
