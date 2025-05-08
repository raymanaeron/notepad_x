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
macdeployqt NotepadX.app -verbose=1

# Update Info.plist with proper metadata - enhanced approach
echo "Updating application metadata..."
PLIST="NotepadX.app/Contents/Info.plist"

# Check if the plist file exists
if [ ! -f "$PLIST" ]; then
    echo "ERROR: Info.plist not found at $PLIST"
    exit 1
fi

# Try to modify some basic properties - CRITICAL for notarization
echo "Setting CFBundleIdentifier..."
defaults write "$PLIST" CFBundleIdentifier "net.notepadx.editor"

echo "Setting CFBundleName..."
defaults write "$PLIST" CFBundleName "NotepadX"

# Add additional required properties for proper notarization
echo "Setting additional properties for notarization..."
defaults write "$PLIST" CFBundleShortVersionString "1.0.0"
defaults write "$PLIST" CFBundleVersion "1"

# Fix permissions for executable
echo "Setting executable permissions..."
chmod +x "NotepadX.app/Contents/MacOS/NotepadX"

echo "Build and app preparation complete!"
echo ""
echo "To create the DMG installer, run the create_dmg.sh script."

# Return to original directory
cd ../..
