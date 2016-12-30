#!/usr/bin/env bats

@test "Normally exits with exit code 1" {
  run core/core
  [ "$status" -eq 1 ]
}

