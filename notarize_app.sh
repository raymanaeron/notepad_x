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

# Show the actual code signature identifier for debugging
echo "🔍 Inspecting code signature identity..."
codesign -dvv "$APP_PATH" 2>&1 | grep "Identifier"

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
SUBMISSION_OUTPUT=$(xcrun notarytool submit "$FULL_ZIP_PATH" --keychain-profile notepadx_profile --wait)
echo "$SUBMISSION_OUTPUT"

# Extract submission ID for later reference
SUBMISSION_ID=$(echo "$SUBMISSION_OUTPUT" | grep "id:" | head -1 | awk '{print $2}')
if [ -n "$SUBMISSION_ID" ]; then
  echo "Submission ID: $SUBMISSION_ID"
  
  # Get detailed info about the submission
  echo "🔍 Retrieving detailed notarization info..."
  xcrun notarytool info "$SUBMISSION_ID" --keychain-profile notepadx_profile
  
  # Get logs for the submission which can help diagnose issues
  echo "📝 Retrieving notarization log..."
  xcrun notarytool log "$SUBMISSION_ID" --keychain-profile notepadx_profile notarization.log
  echo "Log saved to notarization.log"
fi

# Fix permissions of application bundle
echo "🔧 Setting proper permissions on app bundle..."
chmod -R a+rX "$APP_PATH"

# Re-sign the app with explicit bundle ID to ensure consistency
echo "🔄 Re-signing app with explicit bundle ID..."
codesign --force --deep --options runtime --timestamp \
  --entitlements entitlements.plist \
  --prefix net.notepadx \
  --sign "Developer ID Application: Rayman Aeron (TCRVV749YC)" "$APP_PATH"

# Use xcrun to ensure we're using the correct stapler from Xcode
echo "📎 Stapling the notarization ticket to app bundle..."
if ! xcrun stapler staple "$APP_PATH"; then
  echo "⚠️ Stapling failed. This could be due to:"
  echo "  1. The notarization hasn't fully propagated yet"
  echo "  2. Bundle identifier mismatch"
  echo "  3. Command line tools vs Xcode issue"
  
  # Try manual stapling with spctl to verify the issue
  echo "🔄 Checking app with spctl to verify the bundle ID issue..."
  spctl --assess --verbose --raw "$APP_PATH" 2>&1 || true
  
  echo "⚠️ Stapling still failed. Let's wait 30 seconds and try again..."
  sleep 30
  
  if ! xcrun stapler staple "$APP_PATH"; then
    echo "❌ Stapling failed after retry."
    echo "The app is notarized but not stapled."
    
    # Generate a diagnostic report to help debug
    echo "📊 Generating diagnostic information..."
    echo "App bundle identifier: $BUNDLE_ID"
    echo "Code signature details:"
    codesign -dvv "$APP_PATH" 2>&1
    echo "Team ID in provisioning profile: TCRVV749YC"
    echo ""
    echo "Please try running the following command manually:"
    echo "xcrun stapler staple \"$APP_PATH\""
    echo ""
    echo "If that still fails, you can try the following:"
    echo "1. Clean rebuild: ./c_mac.sh && ./b_mac.sh --release"
    echo "2. Make sure bundle ID in CMakeLists.txt matches: net.notepadx.editor"
    echo "3. Re-run mac_code_sign.sh and then notarize_app.sh"
  fi
else
  echo "✅ Stapling successful!"
fi

# Clean up temporary files
echo "🧹 Cleaning up temporary files..."
rm -f "$FULL_ZIP_PATH"

echo "✅ Notarization process completed."
echo "To verify, run: spctl --assess -v \"$APP_PATH\""
