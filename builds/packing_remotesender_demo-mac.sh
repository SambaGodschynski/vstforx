#!bin/sh

if [ $# -ne 1 ]; then
  echo usage: $0 version
  exit 1
fi
vstr=RemoteChannelSender_DEMO-$1-mac
dst=mac/$vstr
folder=VSTForx/
mkdir -p $dst/$folder
cp -r ../src/PluginApps/remoteChannelSender_DEMO.vst $dst/$folder
cp license.txt $dst/$folder
cd $dst
zip -r $vstr.zip *
mv $vstr.zip ../
rm -f -r ../$vstr
