#!/bin/bash
# c_mac.sh - Clean script for macOS builds
# Removes debug and release folders from build directory

# Print header
echo "🧹 Cleaning NotepadX build folders..."

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "No build directory found. Nothing to clean."
    exit 0
fi

# Remove debug folder if it exists
if [ -d "build/debug" ]; then
    echo "Removing debug build folder..."
    rm -rf "build/debug"
    echo "✅ Debug build folder removed."
else
    echo "No debug build folder found."
fi

# Remove release folder if it exists
if [ -d "build/release" ]; then
    echo "Removing release build folder..."
    rm -rf "build/release"
    echo "✅ Release build folder removed."
else
    echo "No release build folder found."
fi

# Also clean installer folder if it exists
if [ -d "build/installer" ]; then
    echo "Removing installer build folder..."
    rm -rf "build/installer"
    echo "✅ Installer build folder removed."
else
    echo "No installer build folder found."
fi

echo "🎉 Cleaning complete! All build folders have been removed."
echo "Run './b_mac.sh' to create a new debug build"
echo "Run './b_mac.sh --release' to create a new release build"
