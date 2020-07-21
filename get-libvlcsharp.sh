#! /bin/bash

# this script assumes there is the libvlcsharp repo next to vlc-unity

rm -rf Assets/VLC-Unity-Windows/Plugins/x86_64/LibVLCSharp.dll

dotnet msbuild ../libvlcsharp/src/LibVLCSharp/LibVLCSharp.csproj /p:Unity=true,Configuration=Release

cp ../libvlcsharp/src/LibVLCSharp/bin/Release/netstandard2.0/LibVLCSharp.dll /vlc-unity/Assets/VLC-Unity-Windows/Plugins/x86_64/