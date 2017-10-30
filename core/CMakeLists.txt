set(_NAME core)

set(_SRCS
  accel.c
  commander.c
  commands.c
  core.c
  core_checklists.c
  log.c
  logging.c
  main.c
  panic.c
  pod-helpers.c
  pod.c
  pru.c
  ring_buffer.c
  states.c
  telemetry.c
  tests.c
)

set(_HDRS
  accel.h
  cdefs.h
  commander.h
  commands.h
  config.h
  config_airsupply.h
  config_inventory.h
  config_overrides.h
  config_photo.h
  config_power.h
  config_scores.h
  core.h
  core_checklists.h
  debug.h
  log.h
  logging.h
  panic.h
  pod-defs.h
  pod-helpers.h
  pod.h
  pru.h
  ring_buffer.h
  states.h
  telemetry.h
)

add_executable (${_NAME} ${_SRCS} ${_HDRS})

target_link_libraries (${_NAME} LINK_PUBLIC libhw)
target_link_libraries (${_NAME} LINK_PUBLIC libimu)

install(TARGETS ${_NAME} DESTINATION bin)
