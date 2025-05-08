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

# to create pkg file
## run once the signing certs are available
./mac-code-sign.sh
./mac-prod-build.sh

## to create dmg file
./installer_mac.sh
./create-dmg.sh

