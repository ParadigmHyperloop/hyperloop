set(_NAME libimu)

set(_SRCS
  crc.c
  imu.c
)

set(_HDRS
  crc.h
  imu.h
)

add_library (${_NAME} ${_SRCS} ${_HDRS})

target_include_directories (${_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS ${_NAME} DESTINATION lib)
install(FILES ${_HDRS} DESTINATION include/${_NAME})
