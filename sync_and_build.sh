#!/bin/bash

rsync -av --exclude ./BUILD --exclude ./pod.xcodeproj/ --exclude ./DerivedData/ -e "ssh -q -T" ~/dev/hyperloop root@192.168.1.2:~/

ssh root@192.168.1.2 "cd ~/hyperloop; and make clean all install run"