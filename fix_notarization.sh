#!/bin/bash
# fix_notarization.sh - Advanced script to resolve stapling issues with notarized apps

# Set parameters
APP_PATH="build/release/NotepadX.app"
BUNDLE_ID="net.notepadx.editor"
DEVELOPER_ID="Developer ID Application: Rayman Aeron (TCRVV749YC)"
ENT_FILE="entitlements.plist"
TEAM_ID="TCRVV749YC"

# Print header
echo "🔧 Advanced notarization fix tool"
echo "================================="

# Check if entitlements file exists, if not create it
if [ ! -f "$ENT_FILE" ]; then
    echo "Creating entitlements file..."
    cat > "$ENT_FILE" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <false/>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
    <key>com.apple.security.cs.disable-library-validation</key>
    <true/>
</dict>
</plist>
EOF
fi

# Check if app exists
if [ ! -d "$APP_PATH" ]; then
    echo "❌ Error: $APP_PATH not found!"
    echo "Please run ./b_mac.sh --release first"
    exit 1
fi

# Verify bundle ID and fix if needed
echo "🔍 Checking bundle identifier..."
CURRENT_ID=$(defaults read "$APP_PATH/Contents/Info" CFBundleIdentifier 2>/dev/null || echo "unknown")
echo "Current bundle identifier: $CURRENT_ID"

if [ "$CURRENT_ID" != "$BUNDLE_ID" ]; then
    echo "⚠️ Bundle identifier mismatch. Updating to $BUNDLE_ID..."
    defaults write "$APP_PATH/Contents/Info" CFBundleIdentifier "$BUNDLE_ID"
    touch "$APP_PATH/Contents/Info.plist"
    echo "✅ Updated bundle identifier"
fi

# Remove all existing signatures - more thorough approach
echo "🔄 Removing all existing signatures..."
codesign --remove-signature "$APP_PATH"

# Also remove signatures from all components
find "$APP_PATH" -type f \( -name "*.dylib" -o -name "*.so" -o -name "*.framework" \) -exec codesign --remove-signature {} \; 2>/dev/null || true

# Re-sign frameworks and plugins
echo "🔒 Re-signing frameworks and plugins..."
find "$APP_PATH/Contents/Frameworks" -type d -name "*.framework" | while read fw; do
    echo "  ✅ Signing framework: $fw"
    codesign --force --options runtime --timestamp --entitlements "$ENT_FILE" --sign "$DEVELOPER_ID" "$fw"
done

find "$APP_PATH/Contents/PlugIns" -type f \( -name "*.dylib" -o -name "*.so" \) | while read plugin; do
    echo "  ✅ Signing plugin: $plugin"
    codesign --force --options runtime --timestamp --entitlements "$ENT_FILE" --sign "$DEVELOPER_ID" "$plugin"
done

# Sign the main executable specifically
echo "🔒 Signing main executable..."
codesign --force --options runtime --timestamp --entitlements "$ENT_FILE" --sign "$DEVELOPER_ID" "$APP_PATH/Contents/MacOS/NotepadX"

# Now sign the app bundle as a whole
echo "🔒 Signing complete app bundle..."
codesign --force --deep --options runtime --timestamp --entitlements "$ENT_FILE" --sign "$DEVELOPER_ID" "$APP_PATH"

echo "✅ Code signing complete. Verifying signature..."
codesign -vvv --deep --strict "$APP_PATH"

echo "🔄 Creating fresh zip archive for notarization..."
ZIP_PATH="build/release/NotepadX_fix.zip"
rm -f "$ZIP_PATH"
ditto -c -k --keepParent "$APP_PATH" "$ZIP_PATH"

echo "📤 Submitting for notarization with explicit options..."
xcrun altool --notarize-app \
    --primary-bundle-id "$BUNDLE_ID" \
    --username "rfa38@cornell.edu" \
    --password "@keychain:notepadx_profile" \
    --team-id "$TEAM_ID" \
    --file "$ZIP_PATH"

echo "⏳ Notarization in progress... Please check status with:"
echo "xcrun altool --notarization-history 0 -u \"rfa38@cornell.edu\" -p \"@keychain:notepadx_profile\" --team-id \"$TEAM_ID\""
echo "Once notarization is complete, run:"
echo "xcrun stapler staple \"$APP_PATH\""
echo ""
echo "If this still fails, try the alternative stapling approach:"
echo "xcrun altool --staple-app -f \"$APP_PATH\""
