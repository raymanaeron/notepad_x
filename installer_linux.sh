#!/bin/bash
# Linux installer build script for NotepadX

# Exit on error
set -e

# Set Qt installation path - adjust according to your installation
# For Linux, this may vary significantly based on distribution
QT_PATH="/opt/Qt/6.9.0/gcc_64"
# Use system Qt if installed by the package manager
if [ ! -d "$QT_PATH" ]; then
    QT_PATH="/usr"
fi

# Determine build type (debug or release)
BUILD_TYPE="release"
CMAKE_BUILD_TYPE="Release"

if [ "$1" == "--debug" ]; then
    BUILD_TYPE="debug"
    CMAKE_BUILD_TYPE="Debug"
fi

# Create appropriate build folder
BUILD_DIR="build/${BUILD_TYPE}"

echo "Creating ${BUILD_TYPE} build directory..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo "Configuring project with CMake in ${BUILD_TYPE} mode..."
# For Linux we may need to specify a different generator depending on the system
if [ -f "/usr/bin/ninja" ]; then
    # Use Ninja if available (faster builds)
    cmake ../.. -G Ninja -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_COMMAND="ninja"
else
    # Otherwise use standard Makefiles
    cmake ../.. -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_COMMAND="make -j$(nproc)"
fi

echo "Building project with ${BUILD_COMMAND} in ${BUILD_TYPE} mode..."
$BUILD_COMMAND

# Create necessary directories for resources
echo "Setting up application resources..."
mkdir -p "icons"
mkdir -p "icons/appicons"
mkdir -p "fonts"

# Copy resources using rsync for better handling of directories
echo "Copying resources to executable directory..."
if [ -d "../../icons" ]; then
    rsync -av "../../icons/" "icons/"
fi

# Copy fonts if they exist
if [ -d "../../fonts" ]; then
    rsync -av "../../fonts/" "fonts/"
fi

# Verify icon was copied
echo "Checking app icon..."
if [ -f "icons/appicons/app_icon_linux.png" ]; then
    echo "Linux app icon found at icons/appicons/app_icon_linux.png"
else
    echo "WARNING: Linux app icon not found, app will use default icon"
fi

# Check if we need to deploy Qt dependencies
if [ "$QT_PATH" != "/usr" ]; then
    echo "Deploying Qt runtime dependencies..."
    # Add Qt bin directory to PATH temporarily
    export PATH="${QT_PATH}/bin:$PATH"
    export LD_LIBRARY_PATH="${QT_PATH}/lib:$LD_LIBRARY_PATH"
    
    # Copy plugins and libs
    mkdir -p platforms
    mkdir -p imageformats
    
    echo "Manually copying essential Qt plugins..."
    cp "${QT_PATH}/plugins/platforms/libqxcb.so" platforms/
    cp "${QT_PATH}/plugins/imageformats/libqsvg.so" imageformats/

    # For more comprehensive dependency bundling, a more sophisticated approach is needed
    # This is a starting point and might need adjustment based on your Linux distribution
fi

# Create build folder for installer
INSTALLER_BUILD_DIR="build/installer"
echo "Creating build directory for installer..."
mkdir -p "${INSTALLER_BUILD_DIR}"
cd "${INSTALLER_BUILD_DIR}"

# Configure project with CMake for installer
echo "Configuring project for installer..."
cmake ../.. -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=Release

echo "Building project..."
# Use multiple cores for faster build
make -j$(nproc)

# Make sure necessary directories exist for resources
echo "Setting up resource directories..."
mkdir -p "icons"
mkdir -p "icons/appicons"

# Copy resources using rsync for better handling of directories
echo "Copying resources..."
rsync -av "../../icons/" "icons/"

# Add Qt bin directory to PATH temporarily
export PATH="${QT_PATH}/bin:$PATH"
export LD_LIBRARY_PATH="${QT_PATH}/lib:$LD_LIBRARY_PATH"

# Deploy Qt dependencies (using linuxdeployqt if available)
echo "Deploying Qt runtime dependencies..."
if command -v linuxdeployqt &> /dev/null; then
    linuxdeployqt NotepadX -appimage -verbose=2
else
    # Manual deployment if linuxdeployqt is not available
    mkdir -p platforms
    mkdir -p imageformats
    cp "${QT_PATH}/plugins/platforms/libqxcb.so" platforms/ 2>/dev/null || :
    cp "${QT_PATH}/plugins/imageformats/libqsvg.so" imageformats/ 2>/dev/null || :
fi

# Build the installer packages
echo "Building installer packages..."
# Build both DEB and RPM packages
cpack -G DEB,RPM

echo "Installer creation complete."
cd ../..

# Create installation packages - uncomment if you've set up CPack
# echo "Creating installation packages..."
# cpack -G DEB

echo "[SUCCESS] ${BUILD_TYPE} build and deployment complete."
cd ../..

echo "You can find the executable at: ${BUILD_DIR}/NotepadX"
