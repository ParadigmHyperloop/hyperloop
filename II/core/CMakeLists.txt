set(_NAME core)
set(_LIB_NAME libcore)

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

# For libcore.a used for testing with ctest
add_library (${_LIB_NAME} ${_SRCS} ${_HDRS})

target_link_libraries (${_LIB_NAME} LINK_PUBLIC libhw)
target_link_libraries (${_LIB_NAME} LINK_PUBLIC libimu)

target_include_directories (${_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories (${_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${_HDRS} DESTINATION include/${_LIB_NAME})
install(FILES ${_HDRS} DESTINATION include/${_NAME})

install(TARGETS ${_NAME} DESTINATION bin)
install(TARGETS ${_LIB_NAME} DESTINATION lib)
