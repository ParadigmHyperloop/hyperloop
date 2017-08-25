#!/bin/bash

rm -rf BUILD DerivedData
rsync -av --exclude ./BUILD --exclude ./pod.xcodeproj --exclude ./hyperloop/DerivedData --exclude ./DerivedData -e "ssh -q -T" ~/dev/hyperloop root@192.168.0.10:~/

ssh root@192.168.0.10 "cd ~/hyperloop; and make clean all install run"