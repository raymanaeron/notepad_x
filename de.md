# Pre-req
- QT 6.9 or above
- https://nsis.sourceforge.io/Download

# mac icons
# First make sure you have a PNG icon
# Then convert it to ICNS format using iconutil
mkdir -p icon.iconset
# You need multiple sizes for a proper ICNS file
sips -z 16 16 your_icon.png --out icon.iconset/icon_16x16.png
sips -z 32 32 your_icon.png --out icon.iconset/icon_32x32.png
sips -z 128 128 your_icon.png --out icon.iconset/icon_128x128.png
sips -z 256 256 your_icon.png --out icon.iconset/icon_256x256.png
sips -z 512 512 your_icon.png --out icon.iconset/icon_512x512.png

# Convert the iconset to icns
iconutil -c icns icon.iconset

# Move to the correct location
mv icon.icns icons/appicons/app_icon_mac.icns

