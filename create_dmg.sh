#!/bin/bash
# Simple script to create a DMG for NotepadX.app

# Ensure the script exits immediately if any command fails
set -e

# Sign the installer app before creating the DMG
echo "Signing app before DMG creation..."
./sign_installer_app.sh

# Check if the app exists in the expected location
APP_PATH="build/installer/NotepadX.app"
if [ ! -d "$APP_PATH" ]; then
    echo "ERROR: NotepadX.app not found at $APP_PATH"
    echo "Please build the application first using the installer_mac.sh script."
    exit 1
fi

# Create output directory
OUTPUT_DIR="build/installer"
mkdir -p "$OUTPUT_DIR"

# Set the DMG filename
DMG_FILE="$OUTPUT_DIR/NotepadX-1.0.0.dmg"

# Remove any existing DMG
if [ -f "$DMG_FILE" ]; then
    echo "Removing existing DMG..."
    rm "$DMG_FILE"
fi

echo "Creating DMG for $APP_PATH..."

# Try the simpler approach first with hdiutil
echo "Using hdiutil to create DMG..."
hdiutil create -volname "NotepadX" -srcfolder "$APP_PATH" -ov -format UDZO "$DMG_FILE"

# Check if DMG was created successfully
if [ -f "$DMG_FILE" ]; then
    echo "[SUCCESS] DMG created at: $DMG_FILE"
    du -h "$DMG_FILE"
    
    echo ""
    echo "To use the DMG:"
    echo "1. Double-click the DMG file to mount it"
    echo "2. Drag NotepadX.app to your Applications folder"
    echo "3. Eject the mounted disk image"
else
    echo "[ERROR] Failed to create DMG file!"
    exit 1
fi
