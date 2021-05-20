#!/bin/bash 

dfu-util -a 0 -R -i 0 -s 0x08000000:leave -D nanoaltair.bin
