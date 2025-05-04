#!/bin/bash

# Set Qt installation path - modify based on your system
QT_PATH="/opt/Qt/6.9.0/gcc_64"
# Fallback to system Qt if the path doesn't exist
if [ ! -d "$QT_PATH" ]; then
    QT_PATH="/usr"
fi

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
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "[ERROR] Build failed."
    cd ../..
    exit 1
fi

# Copy icons folder to build directory
echo "Copying icons to executable directory..."
mkdir -p "icons"
# Use rsync for better directory copying in Linux
rsync -a "../../icons/" "icons/"

# Verify icons were copied
echo "Checking SVG icons..."
ls -la icons/*.svg

echo "Deploying Qt runtime dependencies..."
# Add Qt bin directory to PATH temporarily
export PATH="${QT_PATH}/bin:$PATH"
export LD_LIBRARY_PATH="${QT_PATH}/lib:$LD_LIBRARY_PATH"

# Manually copy essential plugins - modify paths according to your Qt setup
echo "Manually copying essential Qt plugins..."
mkdir -p "platforms"
mkdir -p "imageformats"
cp "${QT_PATH}/plugins/platforms/libqxcb.so" "platforms/" 2>/dev/null || :
cp "${QT_PATH}/plugins/imageformats/libqsvg.so" "imageformats/" 2>/dev/null || :

# Copy core Qt libraries if not using system Qt
if [ "$QT_PATH" != "/usr" ]; then
    cp "${QT_PATH}/lib/libQt6Core.so.6" "." 2>/dev/null || :
    cp "${QT_PATH}/lib/libQt6Gui.so.6" "." 2>/dev/null || :
    cp "${QT_PATH}/lib/libQt6Widgets.so.6" "." 2>/dev/null || :
    cp "${QT_PATH}/lib/libQt6Svg.so.6" "." 2>/dev/null || :
fi

echo "[SUCCESS] ${BUILD_TYPE} build and deployment complete."
cd ../..
