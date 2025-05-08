#!/bin/bash
# altool_notarize.sh - Alternative notarization approach using altool instead of notarytool

set -e

APP_PATH="build/release/NotepadX.app"
ZIP_PATH="build/release/NotepadX_altool.zip"
BUNDLE_ID="net.notepadx.editor"
TEAM_ID="TCRVV749YC"

# Verify the app exists
if [ ! -d "$APP_PATH" ]; then
  echo "❌ Error: $APP_PATH not found!"
  exit 1
fi

# Create zip archive
echo "📦 Creating zip archive for notarization..."
ditto -c -k --keepParent "$APP_PATH" "$ZIP_PATH"

# Submit for notarization using altool
echo "📤 Submitting for notarization using altool..."
UPLOAD_RESULT=$(xcrun altool --notarize-app \
  --primary-bundle-id "$BUNDLE_ID" \
  --username "rfa38@cornell.edu" \
  --password "@keychain:notepadx_profile" \
  --team-id "$TEAM_ID" \
  --file "$ZIP_PATH" 2>&1)

echo "$UPLOAD_RESULT"

# Extract RequestUUID
REQUEST_UUID=$(echo "$UPLOAD_RESULT" | grep 'RequestUUID' | awk '{print $3}')

if [ -z "$REQUEST_UUID" ]; then
  echo "❌ Failed to extract RequestUUID from altool output"
  exit 1
fi

echo "✅ Notarization request submitted with UUID: $REQUEST_UUID"
echo "⏳ Waiting for notarization to complete..."

# Loop to check status
while true; do
  STATUS=$(xcrun altool --notarization-info "$REQUEST_UUID" \
    --username "rfa38@cornell.edu" \
    --password "@keychain:notepadx_profile" 2>&1 | grep "Status:" || echo "Status: unknown")
  
  echo "Current status: $STATUS"
  
  if [[ "$STATUS" == *"success"* ]]; then
    echo "✅ Notarization completed successfully!"
    break
  elif [[ "$STATUS" == *"invalid"* || "$STATUS" == *"failed"* ]]; then
    echo "❌ Notarization failed!"
    exit 1
  fi
  
  echo "⏳ Still waiting for notarization to complete... checking again in 30 seconds"
  sleep 30
done

# Staple the ticket
echo "📎 Stapling notarization ticket to app..."
xcrun stapler staple "$APP_PATH"

# Verify stapling
xcrun stapler validate "$APP_PATH"

echo "🧹 Cleaning up..."
rm -f "$ZIP_PATH"

echo "✅ App notarized and stapled successfully!"
echo "To verify, run: spctl --assess -v \"$APP_PATH\""
