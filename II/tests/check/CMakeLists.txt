set(_NAME check_pod)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(Check REQUIRED)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(TEST_SOURCES
  check_states.c
)

add_executable(${_NAME} ${TEST_SOURCES})
target_link_libraries(${_NAME} libcore ${CHECK_LIBRARIES})

install(TARGETS ${_NAME} DESTINATION bin)

add_test(NAME ${_NAME} COMMAND ${_NAME})