cmake_minimum_required (VERSION 2.8.11)
project (hyperloop)

add_subdirectory (core)
add_subdirectory (libhw)
add_subdirectory (libimu)

add_custom_target(integration COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/test.sh ${PROJECT_BINARY_DIR}/core/core)
