set(_NAME libhw)

set(_SRCS
  adc.c
  bus_manager.c
  i2c.c
  libBBB.c
  mpye.c
  pins.c
  realtime.c
  relay.c
  shims.c
  solenoid.c
  bbb.c
  gpio.c
  gpio_shim.c
)

set(_HDRS
  adc.h
  bus_manager.h
  bus_internal.h
  hw.h
  i2c.h
  libBBB.h
  mpye.h
  pins.h
  realtime.h
  relay.h
  shims.h
  solenoid.h
  bbb.h
  gpio.h
)

find_package (Threads)

add_library (${_NAME} ${_SRCS} ${_HDRS})

# Find Blocks Support
if(UNIX AND NOT APPLE)
  find_library(BLOCKS_LIB BlocksRuntime)

  if(NOT BLOCKS_LIB)
    message(FATAL_ERROR "BlocksRuntime Not Found! https://goo.gl/zKS7jP")
  endif()
  target_link_libraries (${_NAME} ${BLOCKS_LIB})
endif()

target_link_libraries (${_NAME} ${CMAKE_THREAD_LIBS_INIT})

target_include_directories (${_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${_NAME} DESTINATION lib)
install(FILES ${_HDRS} DESTINATION include/${_NAME})
