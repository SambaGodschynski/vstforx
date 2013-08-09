#!/bin/sh
$(yes | hdiutil mount $1 -mountpoint $2 -noautoopen -nobrowse > /dev/null) &
sleep 5

