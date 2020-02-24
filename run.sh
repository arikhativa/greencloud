#!/bin/bash

echo 4 | sudo tee /sys/block/nbd"$1"/queue/max_sectors_kb;
sudo ./greencloud.0.5.out /dev/nbd"$1";
