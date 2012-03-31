#!/usr/bin/sh


# calculates the git commits from git-tag $1 until master
git log master...$1 --cherry-pick --pretty=oneline | wc -l
