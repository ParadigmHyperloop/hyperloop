#!/bin/bash

if [ -z $1 ]; then
  echo "Usage: make_package.sh <output>"
  exit 1
fi

cat << EOF | cat > $1
Package: $PROJECT
Version: $VERSION
Section: base
Priority: optional
Architecture: any
Depends:
Maintainer: Eddie Hurtig <hurtige@ccs.neu.edu>
Description: Paradigm HyperLoop Core Software
 Software that controls the Paradigm Hyperloop Pod
EOF
