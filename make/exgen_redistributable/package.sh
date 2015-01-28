#!/bin/bash
echo "Packaging exgen redistributable..."

ILLUTION_VERSION=2
EXGEN_VERSION=2

DIST_PATH=~/projects/c/make/exgen_redistributable/
RESOURCE_PATH=/usr/local/share/exgen/
EXECUTABLE_PATH=/bin/
CONFIG_PATH=/etc/

rm -r $RESOURCE_PATH/modules/illution_support/system/tmp/*

rm -r $DIST_PATH$RESOURCE_PATH
rm -r $DIST_PATH$EXECUTABLE_PATH
rm -r $DIST_PATH$CONFIG_PATH

mkdir -p $DIST_PATH$RESOURCE_PATH
mkdir -p $DIST_PATH$EXECUTABLE_PATH
mkdir -p $DIST_PATH$CONFIG_PATH

cp -R $RESOURCE_PATH $DIST_PATH$RESOURCE_PATH../
cp $EXECUTABLE_PATH/exgen $DIST_PATH$EXECUTABLE_PATH
cp $CONFIG_PATH/exgenrc $DIST_PATH$CONFIG_PATH
tar -cvzf exgen_`exgen -v`_redistributable.tar.gz ./etc ./usr ./bin

if [ -d /mnt/exgen/exgen ]; then
    echo -n "Copying files to exgen.illution.net..."
    cp exgen_`exgen -v`_redistributable.tar.gz /mnt/exgen/exgen/redistributable
    cp exgen_`exgen -v`_redistributable.tar.gz /mnt/exgen/exgen/redistributable/exgen_redistributable
    cp README /mnt/exgen/exgen/redistributable/README
    echo "done."
else 
    echo "Not copying files to exgen.illution.net because we are not connected."
fi

rm -r $DIST_PATH$RESOURCE_PATH
rm -r $DIST_PATH$EXECUTABLE_PATH
rm -r $DIST_PATH$CONFIG_PATH
rm -r $DIST_PATH/usr