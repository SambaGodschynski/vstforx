#!/bin/sh

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# THIS IS NOT THE BUILD SCRIPT USED BY JENKINS
# IF YOU WANT TO CHANGE THE CI BUILD SETTINGS GO
# TO THE RELATED JENKNGS PRODUCT SETTINGS
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

function __nameWorkaround() {
  if [ -d $2 ]
  then
    rm -r $2 
  fi
  mv $1 $2
  echo "BNDL????" > $2/Contents/PkgInfo
}


function build() {
  ORG=$(pwd)
  cd ~/workspace/sambag
  sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
  make
  cd ~/workspace/vstforx
  sh build.sh -i ~/clibs -g "Unix Makefiles" -f "-DCMAKE_OSX_ARCHITECTURES=x86_64;i386" $@
  make
  cd src/PluginApps
  __nameWorkaround vstforx.app vstforx.vst
  __nameWorkaround vstforxInstrument.app vstforxInstrument.vst
  __nameWorkaround vstforxInstrument_2Out.app vstforxInstrument_2Out.vst
  __nameWorkaround vstforxInstrument_4Out.app vstforxInstrument_4Out.vst
  __nameWorkaround vstforxInstrument_8Out.app vstforxInstrument_8Out.vst
  __nameWorkaround vstforxDEMO.app vstforxDEMO.vst
  __nameWorkaround vstforxInstrumentDEMO.app vstforxInstrumentDEMO.vst
  __nameWorkaround vstforxInstrumentDEMO_2Out.app vstforxInstrumentDEMO_2Out.vst
  __nameWorkaround vstforxInstrumentDEMO_4Out.app vstforxInstrumentDEMO_4Out.vst
  __nameWorkaround vstforxInstrumentDEMO_8Out.app vstforxInstrumentDEMO_8Out.vst

  cd $ORG
}
export -f __nameWorkaround
export -f build
echo build command exported.
