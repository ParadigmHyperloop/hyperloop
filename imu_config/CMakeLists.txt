set(_NAME imu_config)

set(_SRCS
  imu_config.c
)

set(_HDRS

)

add_executable (${_NAME} ${_SRCS} ${_HDRS})

target_link_libraries (${_NAME} LINK_PUBLIC libimu)

install(TARGETS ${_NAME} DESTINATION bin)
