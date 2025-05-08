#!/bin/bash

# Set Qt installation path
QT_PATH="$HOME/Qt/6.9.0/macos"

# Determine build type (debug or release)
BUILD_TYPE="debug"
CMAKE_BUILD_TYPE="Debug"

if [ "$1" == "--release" ]; then
    BUILD_TYPE="release"
    CMAKE_BUILD_TYPE="Release"
fi

# Create appropriate build folder
BUILD_DIR="build/${BUILD_TYPE}"

# Create build directory if it doesn't exist
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

echo "Configuring project with CMake in ${BUILD_TYPE} mode..."
cmake ../.. -DCMAKE_PREFIX_PATH="${QT_PATH}/lib/cmake" -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

if [ $? -ne 0 ]; then
    echo "[ERROR] CMake configuration failed."
    cd ../..
    exit 1
fi

echo "Building project with make in ${BUILD_TYPE} mode..."
make -j$(sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed."
    cd ../..
    exit 1
fi

# Copy icons folder to build directory
echo "Copying icons to executable directory..."
mkdir -p "icons"
# Use rsync for better directory copying in macOS
rsync -a "../../icons/" "icons/"

# Verify icons were copied
echo "Checking SVG icons..."
ls -la icons/*.svg

echo "Deploying Qt runtime dependencies with macdeployqt..."
# Add Qt bin directory to PATH temporarily
export PATH="${QT_PATH}/bin:$PATH"

# Make sure the app bundle exists
if [ ! -d "NotepadX.app" ]; then
    echo "[ERROR] NotepadX.app bundle not found!"
    cd ../..
    exit 1
fi

# Run macdeployqt for comprehensive deployment
echo "Running macdeployqt..."
macdeployqt "NotepadX.app" -verbose=2

echo "[SUCCESS] ${BUILD_TYPE} build and deployment complete."
cd ../..
