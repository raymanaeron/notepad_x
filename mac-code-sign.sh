#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
CERT="Developer ID Application: Rayman Aeron (TCRVV749YC)"
ENT="entitlements.plist"

echo "🔧 Removing old signatures from Qt frameworks..."
find "$APP_PATH/Contents/Frameworks" -name "*.framework" -type d | while read -r fw; do
  echo "  ⨯ Removing signature from $fw"
  codesign --remove-signature "$fw"
done

echo "🔐 Signing frameworks..."
find "$APP_PATH/Contents/Frameworks" -name "*.framework" -type d | while read -r fw; do
  echo "  ✅ Signing $fw"
  codesign --force --options runtime --timestamp \
    --entitlements "$ENT" \
    --sign "$CERT" "$fw"
done

echo "🔐 Signing plug-ins..."
find "$APP_PATH/Contents/PlugIns" \( -name "*.dylib" -or -name "*.so" -or -name "*.bundle" \) | while read -r plugin; do
  echo "  ✅ Signing $plugin"
  codesign --force --options runtime --timestamp \
    --entitlements "$ENT" \
    --sign "$CERT" "$plugin"
done

echo "✅ Signing main NotepadX app..."
codesign --force --deep --options runtime --timestamp \
  --entitlements "$ENT" \
  --sign "$CERT" "$APP_PATH"

echo "🧪 Verifying signature..."
codesign --verify --deep --strict --verbose=2 "$APP_PATH"

echo "✅ All signing complete and verified."
