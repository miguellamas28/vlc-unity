#! /bin/bash

# this script assumes there is the libvlcsharp repo next to vlc-unity
dotnet msbuild /libvlcsharp/src/LibVLCSharp/LibVLCSharp.csproj /p:Unity=true,Configuration=Release

mv /libvlcsharp/src/LibVLCSharp/bin/Release/netstandard2.0/LibVLCSharp.dll /Plugins/vlcsharp/LibVLCSharp.dll