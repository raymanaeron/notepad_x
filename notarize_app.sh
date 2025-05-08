#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
ZIP_PATH="build/release/NotepadX.zip"

# Create a temporary zip file for notarization
echo "📦 Creating temporary zip archive for notarization..."
ditto -c -k --keepParent "$APP_PATH" "$ZIP_PATH"

echo "📤 Submitting for notarization (this may take a few minutes)..."
xcrun notarytool submit "$ZIP_PATH" --keychain-profile notepadx_profile --wait

echo "📎 Stapling ticket to application..."
xcrun stapler staple "$APP_PATH"

echo "🧪 Verifying stapled ticket..."
stapler validate "$APP_PATH"

# Clean up the temporary zip file
echo "🧹 Cleaning up temporary files..."
rm "$ZIP_PATH"

echo "✅ App notarized and stapled successfully."
