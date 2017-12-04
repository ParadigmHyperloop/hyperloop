#!/bin/bash

if [ -z $1 ]; then
  echo "Usage: $0 <dir>"
  exit 1
fi
ROOT_DIR=$1

find "$ROOT_DIR" -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" | xargs clang-format -style=llvm 