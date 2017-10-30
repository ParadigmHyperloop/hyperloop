set(_NAME imu_test)

set(_SRCS
  imu_test.c
)

set(_HDRS

)

add_executable (${_NAME} ${_SRCS} ${_HDRS})

target_link_libraries (${_NAME} LINK_PUBLIC libimu)

install(TARGETS ${_NAME} DESTINATION bin)
