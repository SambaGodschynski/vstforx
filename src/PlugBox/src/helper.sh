#!/bin/sh
################################################################################
# plugBox helper functions
# init. with: source helper.sh
# pb_use_rep file                   : set repository file
# pb_add_vendor name url            : adds vendor
# pb_add_plugin name                : adds plugin to previous added vendor 
#                                     uses vendor as install location
# pb_add_file url os [install loc]  : downloads file and adds file to previous 
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
    plugBox.py $PB_REP add-vendor $1 --url $2 --location $1
}             

function pb_add_plugin() 
{
    if [ -z $PB_VENDOR ]
    then
	echo "no vendor"
	return
    fi
    export PB_PLUGIN=$1
    echo adding $1 to $PB_VENDOR
    plugBox.py $PB_REP add-plugin $1 $PB_VENDOR
}

function pb_add_file() 
{
    if [ -z $PB_VENDOR ] || [ -z $PB_PLUGIN ]
    then
	echo "no vendor or no plugin"
	return
    fi
    echo using $PB_VENDOR:$PB_PLUGIN
}

export -f pb_use_rep
echo pb_use_rep exported

export -f pb_add_vendor
echo pb_add_vendor exported

export -f pb_add_plugin
echo pb_add_plugin exported

export -f  pb_add_file
echo pb_add_file exported
