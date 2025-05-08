#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
BUNDLE_ID=$(defaults read "$APP_PATH/Contents/Info" CFBundleIdentifier 2>/dev/null || echo "net.notepadx.editor")

echo "🔍 Checking notarization status for app bundle: $APP_PATH"
echo "Bundle identifier: $BUNDLE_ID"

# Examine code signature
echo "📝 Code signature details:"
codesign -dvv "$APP_PATH" || echo "⚠️ App may not be properly signed"

# Check if app is notarized
echo "🏛️ Checking Gatekeeper assessment:"
spctl --assess --verbose=4 "$APP_PATH" || echo "⚠️ App did not pass Gatekeeper assessment"

# Check for stapled ticket
echo "🎫 Checking for stapled notarization ticket:"
stapler validate "$APP_PATH" || echo "⚠️ No valid stapled ticket found"

# Show notarization history
echo "📜 Recent notarization history:"
xcrun notarytool history --keychain-profile notepadx_profile --limit 5

echo "✅ Diagnostic check complete."
echo "If you're still having issues, try manually stapling with:"
echo "xcrun stapler staple \"$APP_PATH\""
