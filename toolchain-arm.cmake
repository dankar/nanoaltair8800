set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

find_program(CMAKE_C_COMPILER "arm-none-eabi-gcc")
find_program(CMAKE_ASM_COMPILER "arm-none-eabi-gcc")
find_program(CMAKE_RANLIB "arm-none-eabi-gcc-ranlib")
find_program(CMAKE_AR "arm-none-eabi-gcc-ar")
find_program(CMAKE_SIZE "arm-none-eabi-size")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")
set(GEN_FLAGS "-Wall -fdata-sections -ffunction-sections -Werror")

set(CMAKE_C_FLAGS "${MCU_FLAGS} ${GEN_FLAGS}")
set(CMAKE_ASM_FLAGS "-x assembler-with-cpp ${MCU_FLAGS} ${GEN_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs -Wl,--gc-sections -u _printf_float")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
