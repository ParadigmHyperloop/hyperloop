cmake_minimum_required (VERSION 2.8.11)
project (hyperloop)

set(CMAKE_C_FLAGS_COMMON "-fblocks")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_COMMON} -g -O0")
set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_COMMON} ")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_COMMON} ")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_COMMON} ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_COMMON} ")

add_subdirectory (core)
add_subdirectory (libhw)
add_subdirectory (libimu)
add_subdirectory (imu_test)
add_subdirectory (imu_config)


add_custom_target(integration
                  COMMAND ${CMAKE_COMMAND} -E env CORE_LOCATION=${PROJECT_BINARY_DIR}/core/core pytest
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
