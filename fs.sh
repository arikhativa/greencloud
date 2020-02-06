#!/bin/bash

echo "308351766" | sudo -S mkfs.ext4 /dev/nbd$1
if [ ! -e /tmp/dir$1 ]
then
    echo "308351766" | sudo -S mkdir /tmp/dir$1
fi
echo "308351766" | sudo -S mount -o sync /dev/nbd$1 /tmp/dir$1
