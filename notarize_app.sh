#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
ZIP_PATH="build/release/NotepadX.zip"
PROFILE_NAME="notepadx_profile"

if [ ! -d "$APP_PATH" ]; then
  echo "❌ ERROR: App not found at $APP_PATH"
  exit 1
fi

echo "📦 Creating zip archive for notarization..."
ditto -c -k --keepParent "$APP_PATH" "$ZIP_PATH"

echo "📤 Submitting $ZIP_PATH for notarization..."
xcrun notarytool submit "$ZIP_PATH" \
  --keychain-profile "$PROFILE_NAME" \
  --wait

echo "📎 Stapling notarization ticket to $APP_PATH..."
xcrun stapler staple "$APP_PATH"

echo "✅ App notarized, stapled, and zip created at $ZIP_PATH"
