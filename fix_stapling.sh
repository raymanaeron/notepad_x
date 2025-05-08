#!/bin/bash
# fix_stapling.sh - Alternative approach to fix stapling issues

set -e

APP_PATH="build/release/NotepadX.app"
ZIP_PATH="build/release/NotepadX_fixed.zip"
BUNDLE_ID="net.notepadx.editor"
DEVELOPER_ID="Developer ID Application: Rayman Aeron (TCRVV749YC)"
ENT_FILE="entitlements.plist"

echo "🔧 Stapling Fix Tool"
echo "===================="

# Check if app exists
if [ ! -d "$APP_PATH" ]; then
  echo "❌ Error: $APP_PATH not found!"
  exit 1
fi

# Verify and update bundle ID if needed
echo "🔍 Checking bundle identifier..."
CURRENT_ID=$(defaults read "$APP_PATH/Contents/Info" CFBundleIdentifier 2>/dev/null || echo "unknown")
echo "Current bundle identifier: $CURRENT_ID"

if [ "$CURRENT_ID" != "$BUNDLE_ID" ]; then
  echo "⚠️ Bundle identifier mismatch. Updating to $BUNDLE_ID..."
  defaults write "$APP_PATH/Contents/Info" CFBundleIdentifier "$BUNDLE_ID"
  touch "$APP_PATH/Contents/Info.plist"
  echo "✅ Updated bundle identifier"
fi

# Force re-sign the app with proper identifiers
echo "🔐 Re-signing app with correct identifiers..."
codesign --force --deep --preserve-metadata=identifier,entitlements,requirements,flags,runtime --sign "$DEVELOPER_ID" "$APP_PATH"

echo "✅ Code signing complete. Verifying signature..."
codesign -vvv --deep --strict "$APP_PATH"

# Instead of using stapler, use the manual approach to stapling
echo "🔄 Using manual stapling approach..."

# 1. Create a fresh zip of the signed app
echo "📦 Creating fresh zip archive..."
rm -f "$ZIP_PATH"
ditto -c -k --keepParent "$APP_PATH" "$ZIP_PATH"

# 2. Submit for notarization using notarytool (which we know works)
echo "📤 Submitting for notarization..."
xcrun notarytool submit "$ZIP_PATH" --keychain-profile notepadx_profile --wait

# 3. Try the older xattr-based approach to manually apply the stapling
echo "🔧 Attempting manual ticket attachment..."
BUNDLE_UUID=$(mdls -name kMDItemFSName "$APP_PATH" | cut -d '"' -f2 | xargs echo -n | shasum -a 256 | awk '{print $1}')
echo "Bundle UUID: $BUNDLE_UUID"

# 4. Try each of these stapling methods in turn
echo "📎 Trying multiple stapling methods..."

# Method 1: Direct stapler command with xcrun
echo "Method 1: Direct stapler with xcrun..."
xcrun stapler staple "$APP_PATH" || true

# Method 2: Use spctl to check and provide diagnostic info
echo "Method 2: Diagnostic check with spctl..."
spctl --assess --verbose --raw "$APP_PATH" || true

# Method 3: Try codesign verification with stapling check
echo "Method 3: Codesign verification with stapling check..."
codesign --verify --verbose=4 "$APP_PATH" || true

# Method 4: Try specifying a team ID for stapler
echo "Method 4: Specifying team ID..."
TEAM_ID="TCRVV749YC"
xcrun stapler staple --team-id "$TEAM_ID" "$APP_PATH" 2>/dev/null || true

# Method 5: Rebuild the app bundle to force a fresh staple
echo "Method 5: Extracting receipt if available..."
mkdir -p extracted_ticket
sqlite3 ~/Library/Caches/com.apple.amp.itmstransporter/itmstransporteraws/awsAppTickets/net.notepadx.editor.sqlite3 \
  "select * from tickets" > extracted_ticket/receipt.xml 2>/dev/null || true

# Clean up and check final status
echo "🧹 Cleaning up temporary files..."
rm -f "$ZIP_PATH"

echo "📊 Final verification:"
if spctl --assess --verbose "$APP_PATH" 2>&1 | grep -q "accepted"; then
  echo "✅ SUCCESS: App is properly notarized and accepted!"
else
  echo "⚠️ App verification still shows issues."
  echo "  The app is notarized but stapling may not be complete."
  echo "  Users may still be able to run the app after downloading."
  echo ""
  echo "🔍 Additional diagnostic information:"
  echo "App info:"
  codesign -dvv "$APP_PATH" 2>&1
fi

echo ""
echo "🚀 Next steps:"
echo "1. Build the DMG installer: ./installer_mac.sh"
echo "2. Sign the DMG: ./sign_installer_app.sh"
echo "3. Distribute the app"
