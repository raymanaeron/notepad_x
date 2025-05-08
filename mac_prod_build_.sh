#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
PKG_PATH="build/release/NotepadX.pkg"
CERT="Developer ID Installer: Rayman Aeron (TCRVV749YC)"

echo "📦 Building signed PKG from $APP_PATH..."
productbuild \
  --component "$APP_PATH" /Applications \
  --sign "$CERT" \
  "$PKG_PATH"

echo "✅ PKG created at $PKG_PATH"

echo "📤 Submitting for notarization..."
xcrun notarytool submit "$PKG_PATH" --keychain-profile notepadx_profile --wait

echo "📎 Stapling ticket to PKG..."
xcrun stapler staple "$PKG_PATH"

echo "✅ PKG notarized and stapled successfully."
