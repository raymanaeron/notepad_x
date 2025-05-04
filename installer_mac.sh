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

# Make sure necessary directories exist for resources
echo "Setting up resource directories..."
mkdir -p "NotepadX.app/Contents/Resources/icons"
mkdir -p "NotepadX.app/Contents/Resources/icons/appicons"

# Copy resources using rsync for better handling of directories
echo "Copying resources..."
rsync -av "../../icons/" "NotepadX.app/Contents/Resources/icons/"

# Add Qt bin directory to PATH temporarily
export PATH="${QT_PATH}/bin:$PATH"

# Deploy Qt dependencies
echo "Deploying Qt runtime dependencies..."
macdeployqt NotepadX.app -verbose=1

# Build the installer package (DMG)
echo "Building DMG package..."
cpack -G DragNDrop

echo "Installer creation complete."
cd ../..
