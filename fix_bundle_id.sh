#!/bin/bash
# fix_bundle_id.sh - Fix bundle identifier in CMakeLists.txt and Info.plist files

echo "🔧 Checking and fixing bundle identifier in project files..."

# Update CMakeLists.txt
if [ -f "CMakeLists.txt" ]; then
  echo "Updating CMakeLists.txt..."
  # Create a backup
  cp CMakeLists.txt CMakeLists.txt.backup
  
  # Replace any bundle identifier
  sed -i '' 's/MACOSX_BUNDLE_GUI_IDENTIFIER "com\.notepadx\.app"/MACOSX_BUNDLE_GUI_IDENTIFIER "net.notepadx.editor"/' CMakeLists.txt
  sed -i '' 's/MACOSX_BUNDLE_GUI_IDENTIFIER "com\.elysianedge\.notepadx"/MACOSX_BUNDLE_GUI_IDENTIFIER "net.notepadx.editor"/' CMakeLists.txt
  echo "✅ CMakeLists.txt updated"
fi

# Check if there are any .app bundles and fix their Info.plist
for APP_PATH in $(find . -name "*.app" -type d); do
  PLIST="$APP_PATH/Contents/Info.plist"
  if [ -f "$PLIST" ]; then
    echo "Updating $PLIST..."
    defaults write "$PLIST" CFBundleIdentifier "net.notepadx.editor"
    touch "$PLIST"
    echo "✅ Updated $PLIST"
  fi
done

echo "🔍 Checking current bundle identifiers in Info.plist files:"
find . -name "Info.plist" -path "*/Contents/Info.plist" | while read plist; do
  id=$(defaults read "$plist" CFBundleIdentifier 2>/dev/null || echo "Not set")
  echo "$plist: $id"
done

echo "✅ Bundle identifier fix completed."
echo "Please run './c_mac.sh' and then rebuild with './b_mac.sh --release'"
