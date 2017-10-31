#!/bin/bash
: "${BBB_USER:=root}"
: "${BBB_HOST:=192.168.0.10}"
: "${BBB_FOLDER:=~/hyperloop}"

rm -rf BUILD DerivedData

rsync -av --exclude ./BUILD --exclude ./pod.xcodeproj --exclude ./hyperloop-core.log --exclude hyperloop-telemetry.log.bin  -e "ssh -q -T" . "$BBB_USER@$BBB_HOST:$BBB_FOLDER"

ssh "$BBB_USER@$BBB_HOST" "bash -c 'cd $BBB_FOLDER/proj && rm -rf * && CC=/usr/bin/clang cmake .. && make DESTDIR=./BUILD/dst clean all install && ../run_core.sh'"
