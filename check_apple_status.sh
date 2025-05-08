#!/bin/bash
# check_apple_status.sh - Check Apple notarization server status and your notarization history

echo "🍎 Apple Notarization Service Status Check"
echo "=========================================="

# Check if the server is up using ping
echo "Checking connection to Apple servers..."
ping -c 3 notary.apple.com >/dev/null 2>&1
if [ $? -eq 0 ]; then
  echo "✅ Connection to Apple servers is working"
else
  echo "⚠️ Cannot reach Apple servers - there might be network issues"
fi

# Check system status (using Apple's status API)
echo ""
echo "Checking Apple Developer System Status..."
curl -s https://www.apple.com/support/systemstatus/ | grep -A 3 "Developer" || echo "Could not retrieve system status"

# Try to use notarytool with very basic commands to check API health
echo ""
echo "Testing notarytool basic functionality..."
xcrun notarytool info --help >/dev/null 2>&1
if [ $? -eq 0 ]; then
  echo "✅ notarytool appears to be functioning correctly"
else
  echo "⚠️ notarytool command may have issues"
fi

# Check if the keychain profile exists
echo ""
echo "Checking for keychain profile 'notepadx_profile'..."
security find-generic-password -s "notarytool-password" -a "notepadx_profile" >/dev/null 2>&1
if [ $? -eq 0 ]; then
  echo "✅ Keychain profile 'notepadx_profile' exists"
else
  echo "⚠️ Keychain profile 'notepadx_profile' not found"
  echo "  You may need to run ./notary_tool_one_time_setup.sh again"
fi

# Check recent notarization history
echo ""
echo "Attempting to retrieve recent notarization history..."
xcrun notarytool history --keychain-profile notepadx_profile --limit 3

echo ""
echo "✅ Status check complete"
