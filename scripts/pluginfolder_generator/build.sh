#!/bin/sh

export PATH=$PATH:"$(pwd)/../../src/PlugBox/src"

src=testVstFolder


function usage() 
{
	echo $1 --plattform #--arch
}

#passing arguments
while [ "$1" != "" ]; do
    case $1 in
        -p | --plattform )      shift
                                platt=$1
                                ;;
        -a | --arch )           shift
                                arch=$1
                                ;;
         * )                    usage
                                exit 1
    esac
    shift
done

if [ -z $platt ] 
then
	echo no plattform specifcied
	exit 1
fi

dst="../../src/$src"

#plugBox.py testplugs.xml sync . --plattform $platt

if [ "$platt" = "mac" ] 
then
    #mda mac has five plugs more than win
    rm -r "$src/mda/mda Ambience.vst"
    rm -r "$src/mda/mda RezFilter.vst"
    rm -r "$src/mda/mda Degrade.vst"
    rm -r "$src/mda/mda Image.vst"
    rm -r "$src/mda/mda Shepard.vst"
    ex="vst"
    cp -r "$src/mda/mda Overdrive.vst" noplug.vst
    echo "no binary" > "noplug.vst/Contents/MacOS/mda Overdrive"
else
    echo "no binary" > noplug.dll
    ex="dll"
fi

if [ -d $dst ]; then
  rm -r $dst
fi


cp -r $src $dst
mv  $dst/mda $dst/A
cp -r $dst/A $dst/B
cp -r $dst/A $dst/B/B2
mv  "$dst/B/B2/mda Overdrive.$ex" "$dst/B/B2/mda Overdrive_renamed.$ex"
cp -r noplug.$ex $dst/B/noplug01.$ex
#cp -r noplug.$ex $dst/B/noplug02.$ex
mkdir "$dst/C'"
mkdir "$dst/B/B1"
mkdir "$dst/B/B3"
touch $dst/B/B1/B1_1
#cp -r noplug.$ex $dst/B/B3/noplug01.$ex
cp -r noplug.$ex $dst/B/B3/noplug02.$ex
cp -r "$src/mda/mda Overdrive.$ex" "$dst/C'/'\".$ex"
cp vstforx_plugin_db_dump $dst/corrupt_database

#rm -r $src
rm -r noplug.$ex
#mda BeatBox.vst
