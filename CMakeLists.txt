cmake_minimum_required(VERSION 3.1)

project(nanoaltair8800
  LANGUAGES C ASM)

set(LINKER_SCRIPT "${PROJECT_SOURCE_DIR}/libs/stm32cube/STM32L496RETx_FLASH.ld")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")

add_definitions(-DUSE_HAL_DRIVER -DSTM32L496xx -DBUILD_VERSION="0.1")
add_subdirectory(libs)
add_subdirectory(firmware)
