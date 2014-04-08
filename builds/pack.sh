#!/bin/sh

function packVSTForx() {
    src=$1
    bin=$2
    vstr=$3
    post=$4
    target=$5
    folder=$6
    ext=$7
    mkdir -p $target/$folder
    cp -r $bin/vstforx$post.$ext $target/$folder
    cp -r $bin/vstforxInstrument$post.$ext $target/$folder
    cp license.txt $target/$folder
    if [ $target = "mac" ]
    then
	cp $src/vstforx-init.lua $target/$folder/vstforx$post.$ext/
	cp $src/vstforx-helper.lua $target/$folder/vstforx$post.$ext/
	cp $src/vstforx-menusetup.lua $target/$folder/vstforx$post.$ext/
	cp $src/vstforx-init.lua $target/$folder/vstforxInstrument$post.$ext/
	cp $src/vstforx-helper.lua $target/$folder/vstforxInstrument$post.$ext/
	cp $src/vstforx-menusetup.lua $target/$folder/vstforxInstrument$post.$ext/
    else 
	cp $src/vstforx-init.lua $target/$folder
	cp $src/vstforx-helper.lua $target/$folder
	cp $src/vstforx-menusetup.lua $target/$folder
    fi
    cp ../Documents/manual/vstforx.EN/main.pdf $target/$folder/manual.pdf
    cd $target
    zip -r $vstr.zip $folder
    rm -rf $folder
    cd -
}

function packRemote() {
    src=$1
    bin=$2
    vstr=$3
    post=$4
    target=$5
    folder=$6
    ext=$7
    mkdir -p $target/$folder
    cp -r $bin/remoteChannelSender$post.$ext $target/$folder
    cp license.txt $target/$folder
    cd $target
    zip -r $vstr.zip $folder
    rm -rf $folder
    cd -
}

function packStandalone() {
    src=$1
    bin=$2
    ext=$3
    ver=$4
    target=$5
    folder=VSTForx
    mkdir -p $target/$folder/scripts
    cp $bin/standalone$ext $target/$folder
    cp $bin/unit_tests$ext $target/$folder
    cp -r $src/images $target/$folder 
    cp -r $src/testScripts $target/$folder
    cp $src/vstforx-init.lua $target/$folder
    cp $src/vstforx-helper.lua $target/$folder
    cp $src/vstforx-menusetup.lua $target/$folder
    cp $src/scripts/util.lua $target/$folder/scripts
    cp license.txt $target/$folder
    cd $target
    zip -r $ver.zip $folder
    rm -rf $folder
    cd -
}

################################################################################

if [ $# -ne 2 ]
then
  echo 'usage: $0 target[mac|win|win64] version'
  exit 1
fi

version=$2

if [ $1 = "mac" ]
then
    echo mac $version:
    packVSTForx ../src ../src/PluginApps VSTForx-$version-mac '' mac VSTForx vst
    packVSTForx ../src ../src/PluginApps VSTForxDEMO-$version-mac DEMO mac VSTForx vst
    packRemote ../src ../src/PluginApps RemoteChannelSender-$version-mac '' mac VSTForx vst
    packRemote ../src ../src/PluginApps RemoteChannelSenderDEMO-$version-mac _DEMO mac VSTForx vst
    packStandalone ../src ../src/PluginApps '' standalone-$version-mac mac
elif [ $1 = "win" ] 
then
    echo win $version:
    packVSTForx ../src ../src/PluginApps/Release VSTForx-$version-win '' win VSTForx dll
    packVSTForx ../src ../src/PluginApps/Release VSTForxDEMO-$version-win DEMO win VSTForx dll
    packRemote ../src ../src/PluginApps/Release RemoteChannelSender-$version-win '' win VSTForx dll
    packRemote ../src ../src/PluginApps/Release RemoteChannelSenderDEMO-$version-win _DEMO win VSTForx dll
    packStandalone ../src ../src/PluginApps/Release .exe standalone-$version-win win
elif [ $1 = "win64" ] 
then
    echo win64 $version:
    packVSTForx ../src ../src/PluginApps/Release VSTForx-$version-win64 '' win VSTForx64 dll
    packVSTForx ../src ../src/PluginApps/Release VSTForxDEMO-$version-win64 DEMO win VSTForx64 dll
    packRemote ../src ../src/PluginApps/Release RemoteChannelSender-$version-win64 '' win VSTForx64 dll
    packRemote ../src ../src/PluginApps/Release RemoteChannelSenderDEMO-$version-win64 _DEMO win VSTForx64 dll
    packStandalone ../src ../src/PluginApps/Release .exe standalone-$version-win64 win
fi
