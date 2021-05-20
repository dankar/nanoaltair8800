cmake_minimum_required(VERSION 3.15)
project(nanoaltair-tests
  LANGUAGES C)

include(../version.cmake)
get_version_string(VERSION_STRING ..)

add_definitions(-DTEST_BUILD -DUINT=uint32_t -DBUILD_VERSION="${VERSION_STRING}")
add_compile_options(-Wall -Werror)

add_executable(nanoaltair-tester
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/88dcdd.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/serial.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/intel8080.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/88dcdd.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/serial.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/roms.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/roms.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/memory.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/memory.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/irq_controller.h
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/irq_controller.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/intel8080.c
  ${CMAKE_SOURCE_DIR}/../libs/altair8800/op_codes.h
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/pc/filesystem_pc.c
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/log.h
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/log.c
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/buffer.c
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/buffer.h
  ${CMAKE_SOURCE_DIR}/../libs/common/sys/filesystem.h
  testing.c
  testing.h
  altair_tests.c)

target_include_directories(nanoaltair-tester PUBLIC
  ${CMAKE_SOURCE_DIR}/../libs/
  ${CMAKE_SOURCE_DIR}/../libs/common/)
