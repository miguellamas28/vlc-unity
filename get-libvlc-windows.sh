#! /bin/bash
sourceLocation="Assets/PluginSource"

rm ./build.7z > /dev/null
rm -rf ./build > /dev/null
rm -rf $sourceLocation/sdk > /dev/null
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/libvlc.dll > /dev/null
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/libvlccore.dll > /dev/null
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/hrtfs > /dev/null 
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/locale > /dev/null
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/lua > /dev/null 
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/plugins > /dev/null


7z x vlc-4.0.0-dev-win64.7z -o./build

cp -R ./build/vlc-4.0.0-dev/{libvlc.dll,libvlccore.dll,hrtfs,locale,lua,plugins} Assets/VLC-Unity-Windows/Plugins/x86_64
rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/lua/http # contains unnecessary js files which make the local Unity Store validator fail
cp -r ./build/vlc-4.0.0-dev/sdk/ $sourceLocation/