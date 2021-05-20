cmake_minimum_required(VERSION 3.1)

option(BUILD_WITH_PROFILER "Build the firmware with profiler enabled" OFF)

include(toolchain-arm.cmake)

project(nanoaltair8800
  LANGUAGES C ASM)

include(version.cmake)
get_version_string(VERSION_STRING .)

include(ExternalProject)
ExternalProject_Add(basic_tests
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/tests
  BINARY_DIR ${CMAKE_BINARY_DIR}/tests
  STEP_TARGETS build
  BUILD_COMMAND make test_run_basic
  EXCLUDE_FROM_ALL YES)

ExternalProject_Add(full_tests
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/tests
  BINARY_DIR ${CMAKE_BINARY_DIR}/tests
  STEP_TARGETS build
  BUILD_COMMAND make test_run_full
  EXCLUDE_FROM_ALL YES)

add_custom_target(tester ALL)
add_dependencies(tester basic_tests-build)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUG_BUILD)
  set(VERSION_STRING "${VERSION_STRING}-debug")
else()
  if(CMAKE_BUILD_TYPE STREQUAL Release)
	add_subdirectory(release)
  else()
	message(FATAL_ERROR "CMAKE_BUILD_TYPE must be either Release or Debug. If not specified Release will be used. You specified \"${CMAKE_BUILD_TYPE}\".")
  endif()
endif()

if(${BUILD_WITH_PROFILER})
  set(VERSION_STRING "${VERSION_STRING}-profiler")
  add_definitions(-DPROFILER_ENABLED=1)
endif()



set(LINKER_SCRIPT
  "${PROJECT_SOURCE_DIR}/libs/stm32cube/STM32L496RETx_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS
  "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

add_definitions(
  -DUSE_HAL_DRIVER
  -DSTM32L496xx
  -DBUILD_VERSION="${VERSION_STRING}")

add_subdirectory(libs)
add_subdirectory(firmware)


