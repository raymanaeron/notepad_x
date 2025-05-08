# install mac sigining authrotiy certs
curl -O https://www.apple.com/certificateauthority/AppleRootCA-G3.cer                                  
curl -O https://www.apple.com/certificateauthority/AppleWWDRCAG4.cer                                   

sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain AppleRootCA-G3.cer 
sudo security add-trusted-cert -d -r trustAsRoot -k /Library/Keychains/System.keychain AppleWWDRCAG4.cer

# fix the dev id profile
open the cert from the keychain
expand trust
allow everything

# find the signinig ccertificate
security find-identity 

# 1. Build the release app
./b_mac.sh --release

# 2. Notarize and staple the signed .app bundle
./mac_code_sign.sh
./notearize_app.sh

# 3. Build and sign the app bundle for DMG distribution
./installer_mac.sh

# 4. Sign the installer build of the app
./sign_installer_app.sh

# 5. Create the signed DMG
./create_dmg.sh

# 6. Create the signed PKG
./mac_prod_build_.sh




