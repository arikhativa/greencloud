#!/bin/bash

echo "308351766" | sudo -S nbd-client -d /dev/nbd$1
echo "308351766" | sudo -S umount /tmp/dir$1
echo "308351766" | sudo -S rm -rf /tmp/dir$1
