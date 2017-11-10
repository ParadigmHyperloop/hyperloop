#!/bin/bash
: "${BBB_USER:=root}"
: "${BBB_HOST:=192.168.0.10}"
: "${BBB_FOLDER:=~/hyperloop}"


ssh "$BBB_USER@$BBB_HOST" "bash -c 'cd $BBB_FOLDER && ./run_core.sh'"
