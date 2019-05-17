#!/bin/bash

set -e

function blue-bar () {
    echo -e "\r\x1B[1;37;44m$*\x1B[K\x1B[0m" 1>&2
}

function unsymlink () {
	if [ -h "$1" ]
	then
		echo Deleting symlink "$1"
		rm "$1"
	fi
}

function maybe-mkdir () {
	if [ ! -d "$1" ]
	then
		echo Creating directory "$1"
		mkdir "$1"
	fi
}

function unsymlink-and-maybe-mkdir () {
	unsymlink "$1"
	maybe-mkdir "$1"
}

##
## /usr/share/lightfield/libexec
##

rm -drv /usr/share/lightfield

##
## /var/cache/lightfield/print-jobs
##

symlink=""
if [ -h /var/cache/lightfield/print-jobs ]
then
    symlink=$(readlink -e /var/cache/lightfield/print-jobs)
fi

unsymlink-and-maybe-mkdir /var/cache/lightfield
unsymlink-and-maybe-mkdir /var/cache/lightfield/print-jobs

if [ -n "${symlink}" -a -d "${symlink}" ]
then
    echo Relocating cached print jobs from "${symlink}" to /var/cache/lightfield/print-jobs
    mv "${symlink}"/* /var/cache/lightfield/print-jobs/
fi

##
## /var/lib/lightfield/model-library
##

symlink=""
if [ -h /var/lib/lightfield/model-library ]
then
    symlink=$(readlink -e /var/lib/lightfield/model-library)
fi

unsymlink-and-maybe-mkdir /var/lib/lightfield
unsymlink-and-maybe-mkdir /var/lib/lightfield/model-library

if [ -n "${symlink}" -a -d "${symlink}" ]
then
    echo Relocating models from "${symlink}" to /var/lib/lightfield/model-library
    mv "${symlink}"/* /var/lib/lightfield/model-library/
fi

mkdir /var/lib/lightfield/software-updates

##
## /var/log/lightfield
##

symlink=""
if [ -h /var/log/lightfield ]
then
    symlink=$(readlink -e /var/log/lightfield)
fi

unsymlink-and-maybe-mkdir /var/log/lightfield

if [ -n "${symlink}" -a -d "${symlink}" ]
then
    echo Relocating debugging logs from "${symlink}" to /var/log/lightfield
    mv "${symlink}"/* /var/log/lightfield/
fi

##
## Permissions
##

chown -R lumen:lumen /var/cache/lightfield /var/lib/lightfield /var/log/lightfield

[ -f /home/lumen/.bashrc ] && mv /home/lumen/.bashrc /home/lumen/.real_bash_profile
