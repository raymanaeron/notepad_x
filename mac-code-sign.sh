#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
CERT="Apple Distribution: Rayman Aeron (TCRVV749YC)"
ENT="entitlements.plist"

echo "Signing frameworks..."
find "$APP_PATH/Contents/Frameworks" -name "*.framework" -type d | while read -r framework
do
    echo "Signing $framework..."
    codesign --force --deep --options runtime --entitlements "$ENT" --sign "$CERT" "$framework"
done

echo "Signing plug-ins..."
find "$APP_PATH/Contents/PlugIns" -name "*.dylib" -or -name "*.so" -or -name "*.bundle" | while read -r plugin
do
    echo "Signing $plugin..."
    codesign --force --options runtime --entitlements "$ENT" --sign "$CERT" "$plugin"
done

echo "Signing main app..."
codesign --force --deep --options runtime --entitlements "$ENT" --sign "$CERT" "$APP_PATH"

echo "✅ Done signing $APP_PATH"
