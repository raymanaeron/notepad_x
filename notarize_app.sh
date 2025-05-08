#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
ZIP_PATH="build/release/NotepadX.zip"

# Verify the app exists and is properly signed before proceeding
if [ ! -d "$APP_PATH" ]; then
  echo "❌ Error: $APP_PATH not found!"
  echo "Please run ./b_mac.sh --release and ./mac_code_sign.sh first."
  exit 1
fi

# Check and fix bundle identifier - critical for notarization
echo "🔍 Checking bundle identifier..."
BUNDLE_ID=$(defaults read "$APP_PATH/Contents/Info" CFBundleIdentifier 2>/dev/null || echo "unknown")
echo "Current bundle identifier: $BUNDLE_ID"

# Make sure bundle ID matches what's in the code signature
if [ "$BUNDLE_ID" != "net.notepadx.editor" ]; then
  echo "⚠️ Bundle identifier mismatch. Updating to match code signature..."
  defaults write "$APP_PATH/Contents/Info" CFBundleIdentifier "net.notepadx.editor"
  # Touch the Info.plist to ensure changes are applied
  touch "$APP_PATH/Contents/Info.plist"
  echo "Updated bundle identifier to: net.notepadx.editor"
fi

# Verify code signing before proceeding
echo "🔍 Verifying code signature before notarization..."
if ! codesign -v --deep --strict "$APP_PATH"; then
  echo "❌ Error: Code signature verification failed!"
  echo "Please run ./mac_code_sign.sh before notarizing."
  exit 1
fi

# Create a temporary zip file for notarization
echo "📦 Creating temporary zip archive for notarization..."
# Use full path for better reliability
FULL_APP_PATH=$(cd "$(dirname "$APP_PATH")" && pwd)/$(basename "$APP_PATH")
FULL_ZIP_PATH=$(cd "$(dirname "$ZIP_PATH")" && pwd)/$(basename "$ZIP_PATH")

# Remove previous zip if it exists
rm -f "$FULL_ZIP_PATH"

# Use ditto with absolute paths
ditto -c -k --keepParent "$FULL_APP_PATH" "$FULL_ZIP_PATH"

echo "📤 Submitting for notarization (this may take a few minutes)..."
xcrun notarytool submit "$FULL_ZIP_PATH" --keychain-profile notepadx_profile --wait

# Fix permissions of application bundle
echo "🔧 Setting proper permissions on app bundle..."
chmod -R a+rX "$APP_PATH"

# Use xcrun to ensure we're using the correct stapler from Xcode
echo "📎 Stapling the notarization ticket to app bundle..."
if ! xcrun stapler staple "$APP_PATH"; then
  echo "⚠️ Stapling failed. This could be due to:"
  echo "  1. The notarization hasn't fully propagated yet"
  echo "  2. Bundle identifier mismatch"
  echo "  3. Command line tools vs Xcode issue"
  
  echo "🔄 Trying alternative approach with xcrun..."
  # Make sure Xcode is selected, not just command line tools
  sudo xcode-select -s /Applications/Xcode.app/Contents/Developer || true
  
  # Try again with explicit xcrun
  if ! xcrun stapler staple "$APP_PATH"; then
    echo "⚠️ Stapling still failed. Let's wait 60 seconds and try again..."
    sleep 60
    
    if ! xcrun stapler staple "$APP_PATH"; then
      echo "❌ Stapling failed after multiple attempts."
      echo "The app is notarized but not stapled. It should still pass Gatekeeper when downloaded from the internet."
      echo "You can try manually stapling later with:"
      echo "sudo xcode-select -s /Applications/Xcode.app/Contents/Developer"
      echo "xcrun stapler staple \"$APP_PATH\""
    fi
  fi
else
  echo "✅ Stapling successful!"
fi

# Clean up
echo "🧹 Cleaning up temporary files..."
rm -f "$FULL_ZIP_PATH"

echo "✅ Notarization process completed."
echo "To verify, run: spctl --assess -v \"$APP_PATH\""
