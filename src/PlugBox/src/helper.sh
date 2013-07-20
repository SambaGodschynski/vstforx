#!/bin/sh
################################################################################
# plugBox helper functions
# init. with: source helper.sh
# pb_use_rep file                   : set repository file
# pb_add_vendor name url            : adds vendor
# pb_add_plugin name                : adds plugin to previous added vendor 
#                                     uses vendor as install location
# pb_add_vst_file url os [install loc]  : downloads file and adds file to previous 
#                                     added plugin
#                                     valid os args are : 
#                                     win32, win64, mac32, mac64, macUni
################################################################################

function pb_use_rep() 
{
    export PB_REP=$1
}


function pb_add_vendor() 
{
    export PB_VENDOR=$1
    plugBox.py $PB_REP add-vendor "$1" --url "$2" --location "$1"
}             

function pb_use_vendor()
{
    export PB_VENDOR=$1
}

function pb_add_plugin() 
{
    if [ -z "$PB_VENDOR" ]
    then
	echo "no vendor"
	return
    fi
    export PB_PLUGIN=$1
    echo adding "$1" to "$PB_VENDOR"
    plugBox.py $PB_REP add-plugin "$1" "$PB_VENDOR"
}

function pb_use_plugin()
{
  export PB_PLUGIN=$1
}

function pb_add_vst_file() 
{
    if [ -z "$PB_VENDOR" ] || [ -z "$PB_PLUGIN" ]
    then
	echo "no vendor or no plugin"
	return
    fi
    case $2 in
	win32)
	    pb_flags="--plattform windows --arch i386"
	    ;;
	win64)
	    pb_flags="--plattform windows --arch x64"
	    ;;
	mac32)
	    pb_flags="--plattform mac --arch i386"
	    ;;
	mac64)
	    pb_flags="--plattform mac --arch x64"
	    ;;
	macUni)
	    pb_flags="--plattform mac --arch i386,x64"
    esac
    if [ -z "$pb_flags" ]
    then
	echo missing os
	return
    fi
    if [ ! -z "$3" ]
    then
	pb_flags="$pb_flags --location $3"
    fi
    echo using "$PB_PLUGIN@$PB_VENDOR $pb_flags"
    mkdir $$
    cd $$
    curl -O $1
    cd ..
    echo add $1 to "$PB_PLUGIN@$PB_VENDOR"
    plugBox.py $PB_REP add-file $$/* "$1" "$PB_VENDOR" "$PB_PLUGIN" $pb_flags
    rm -r $$
    pb_flags=""
}

export PATH=$PATH:$(pwd)

export -f pb_use_rep
echo pb_use_rep exported

export -f pb_add_vendor
echo pb_add_vendor exported

export -f pb_use_vendor
echo pb_use_vendor exported

export -f pb_add_plugin
echo pb_add_plugin exported

export -f pb_use_plugin
echo pb_use_plugin_exported

export -f  pb_add_vst_file
echo pb_add_vst_file exported
