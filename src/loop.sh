#!/bin/sh

for x in $(seq $2)
do
	echo run $x:
	echo "<<<<<<<<<<>>>>>>>>>>"
	$1
done
