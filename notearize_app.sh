#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
PROFILE_NAME="notepadx_profile"

if [ ! -d "$APP_PATH" ]; then
  echo "❌ ERROR: App not found at $APP_PATH"
  exit 1
fi

echo "📤 Submitting $APP_PATH for notarization..."
xcrun notarytool submit "$APP_PATH" \
  --keychain-profile "$PROFILE_NAME" \
  --wait

echo "📎 Stapling notarization ticket to $APP_PATH..."
xcrun stapler staple "$APP_PATH"

echo "✅ App notarized and stapled successfully."
