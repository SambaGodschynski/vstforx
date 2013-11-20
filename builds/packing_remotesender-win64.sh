#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=RemoteChannelSender-$1-win64
dst=win64/$vstr
folder="VSTForx (x86_64)/"
mkdir -p $dst/"$folder"
cp ../src/PluginApps/Release/remoteChannelSender.dll "$dst/$folder/remoteChannelSender (x86_64).dll"
cp license.txt $dst/"$folder"
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
