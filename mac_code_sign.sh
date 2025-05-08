#!/bin/bash
set -e

APP_PATH="build/release/NotepadX.app"
CERT="Developer ID Application: Rayman Aeron (TCRVV749YC)"
ENT="entitlements.plist"

# Check if entitlements file exists
if [ ! -f "$ENT" ]; then
  echo "⚠️ Entitlements file not found, creating basic entitlements..."
  cat > "$ENT" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.cs.allow-unsigned-executable-memory</key>
    <true/>
    <key>com.apple.security.cs.disable-library-validation</key>
    <true/>
    <key>com.apple.security.cs.allow-dyld-environment-variables</key>
    <true/>
    <key>com.apple.security.automation.apple-events</key>
    <true/>
</dict>
</plist>
EOF
fi

# Verify app existence
if [ ! -d "$APP_PATH" ]; then
  echo "❌ Error: $APP_PATH not found! Please build the app first with ./b_mac.sh --release"
  exit 1
fi

echo "🔧 Removing old signatures from Qt frameworks..."
find "$APP_PATH/Contents/Frameworks" -name "*.framework" -type d | while read -r fw; do
  echo "  ⨯ Removing signature from $fw"
  codesign --remove-signature "$fw"
done

echo "🔐 Signing frameworks..."
find "$APP_PATH/Contents/Frameworks" -name "*.framework" -type d | while read -r fw; do
  echo "  ✅ Signing $fw"
  codesign --force --options runtime --timestamp \
    --entitlements "$ENT" \
    --sign "$CERT" "$fw"
done

echo "🔐 Signing plug-ins..."
find "$APP_PATH/Contents/PlugIns" \( -name "*.dylib" -or -name "*.so" -or -name "*.bundle" \) | while read -r plugin; do
  echo "  ✅ Signing $plugin"
  codesign --force --options runtime --timestamp \
    --entitlements "$ENT" \
    --sign "$CERT" "$plugin"
done

echo "✅ Signing main NotepadX app..."
codesign --force --deep --options runtime --timestamp \
  --entitlements "$ENT" \
  --sign "$CERT" "$APP_PATH"

echo "🧪 Verifying signature..."
codesign --verify --deep --strict --verbose=2 "$APP_PATH"

echo "✅ All signing complete and verified."
