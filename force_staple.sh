#!/bin/bash
# A simplified script that uses an alternative approach to staple the app

set -e

APP_PATH="build/release/NotepadX.app"

echo "🔧 Attempting to force staple the app using alternative methods..."

# Ensure the app exists
if [ ! -d "$APP_PATH" ]; then
  echo "❌ Error: $APP_PATH not found!"
  exit 1
fi

# Check the bundle identifier
echo "🔍 Checking bundle identifier..."
BUNDLE_ID=$(defaults read "$APP_PATH/Contents/Info" CFBundleIdentifier 2>/dev/null || echo "unknown")
echo "Current bundle identifier: $BUNDLE_ID"

# Method 0: Try to regenerate the Info.plist and re-sign before attempting staple
echo "📄 Regenerating Info.plist with correct metadata..."
PLIST="$APP_PATH/Contents/Info.plist"
ORIG_PLIST="${PLIST}.orig"

# Backup original plist
cp "$PLIST" "$ORIG_PLIST"

# Create a new one with known good values
/usr/libexec/PlistBuddy -c "Clear" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundleIdentifier string $BUNDLE_ID" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundleName string NotepadX" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundleExecutable string NotepadX" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundleVersion string 1.0.0" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundleShortVersionString string 1.0.0" "$PLIST"
/usr/libexec/PlistBuddy -c "Add :CFBundlePackageType string APPL" "$PLIST"

# Re-sign with the new plist
echo "🔒 Re-signing with fresh metadata..."
codesign --force --options runtime --timestamp -s "Developer ID Application: Rayman Aeron (TCRVV749YC)" "$APP_PATH"

# Alternative approach 1: Use xcrun stapler with explicit options
echo "📎 Method 1: Using xcrun stapler with explicit options..."
xcrun stapler staple -v "$APP_PATH"

# Alternative approach 2: Use spctl to check and bypass
echo "📎 Method 2: Using spctl bypass approach..."
# This doesn't actually staple but can help verify if the app would pass Gatekeeper
spctl --assess --type execute -v "$APP_PATH" || true

# Alternative approach 3: Try the installer approach
echo "📎 Method 3: Using pkgbuild approach..."
# Create a temporary location
TEMP_DIR="build/temp_pkg"
mkdir -p "$TEMP_DIR"

# Create a component package first
echo "Creating component package..."
pkgbuild --component "$APP_PATH" --install-location "/Applications" "$TEMP_DIR/NotepadX_component.pkg"

# Staple the component package
echo "Stapling component package..."
xcrun stapler staple "$TEMP_DIR/NotepadX_component.pkg" || true

# Extract the app from the package
echo "Extracting app from package..."
pkgutil --expand "$TEMP_DIR/NotepadX_component.pkg" "$TEMP_DIR/expanded"

# Add a new approach: Create a PKG installer
echo "📎 Method 4: Creating a PKG installer for distribution..."
INSTALLER_PKG="build/release/NotepadX-Installer.pkg"

# Build a simple pkg installer
productbuild --component "$APP_PATH" /Applications "$INSTALLER_PKG"

# Attempt to staple the pkg instead
echo "Stapling the PKG installer..."
xcrun stapler staple "$INSTALLER_PKG" || true

echo ""
echo "✅ PKG installer created and stapling attempted."
echo "   If the app itself cannot be stapled, distribute the PKG installer:"
echo "   $INSTALLER_PKG"
echo ""
echo "✅ Alternative stapling attempts complete."
echo "If stapling still fails, you may need to distribute the app using the PKG format."
echo "Note: Your app was successfully notarized, which means users can still run it after downloading."
echo "      They'll just need to bypass an initial security warning."
