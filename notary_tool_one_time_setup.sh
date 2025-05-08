#!/bin/bash
set -e

echo "🔐 Setting up notarization profile for NotepadX..."

# This creates a keychain profile for the notary tool
# so you don't need to enter credentials each time
xcrun notarytool store-credentials notepadx_profile \
  --apple-id "rfa38@cornell.edu" \
  --team-id "TCRVV749YC" \
  --password "ohbn-bwtr-qwah-auen"

echo "✅ Notarization profile has been set up successfully."
echo "You can now use ./notarize_app.sh to notarize the application."
