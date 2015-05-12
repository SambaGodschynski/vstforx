#!/bin/sh


if [ -z $FRX_SAMBAG_COMMIT ]
then
  echo "missing sambag (git) commit hash value. use export FRX_SAMBAG_COMMIT=xy."
  exit 1
fi

if [ -z $FRX_VSTFORX_COMMIT ]
then
  echo "missing vstforx (git) commit hash value. use export FRX_VSTFORX_COMMIT=xy."
  exit 1
fi

if [ -z $FRX_CLIB_LOC ]
then
  echo "missing clib location. use export FRX_CLIB_LOC=xy."
  exit 1
fi

if [ -z $FRX_SAMBAG_LOC ]
then
  echo "missing sambag location. use export FRX_SAMBAG_LOC=xy."
  exit 1
fi


if [ -z $FRX_VSTFORX_LOC ]
then
  echo "missing vstforx location. use export FRX_VSTFORX_LOC=xy."
  exit 1
fi

ROOT=$(pwd)
FRX_LOC=$FRX_VSTFORX_LOC
SAMBAG_LOC=$FRX_SAMBAG_LOC
DEPL=builds/$FRX_VSTFORX_COMMIT

log()
{
    echo "[[[ $(date) ]]] $1" >> $ROOT/build.log
}


build()
{
  log "building $1 $2"
  tmpdir=$(pwd)
  cd $1
  make clean
  git checkout $2
  if [ $? -ne 0 ]
  then
      log "git checkout failed"
      exit 1
  fi
  rm CMakeCache.txt
  cd scripts
  python cmakeWalker.py
  cd ..
  sh build.sh -i $FRX_CLIB_LOC -g "Unix Makefiles" -d $3
  make
  cd $tmpdir
}
build $SAMBAG_LOC $FRX_SAMBAG_COMMIT $@
build $FRX_LOC $FRX_VSTFORX_COMMIT $@

#deploy
mkdir -p $DEPL
cp -r $FRX_LOC/src/PluginApps/vstforx.app $DEPL/vstforx.vst
cp -r $FRX_LOC/src/PluginApps/standalone.app $DEPL
cp -r $FRX_LOC/src/unit_tests $DEPL

#make sure that we end in our root folder
cd $ROOT