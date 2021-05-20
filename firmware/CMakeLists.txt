add_executable(nanoaltair.elf
  altair_main.c
  callbacks.c)

target_link_libraries(nanoaltair.elf -Wl,--whole-archive common -Wl,--no-whole-archive altair8800 m c nosys -Wl,--whole-archive stm32cube -Wl,--no-whole-archive)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.bin"
  COMMAND "${CMAKE_SIZE}" "${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.elf"
  COMMAND "${CMAKE_OBJCOPY}" -O binary -S "${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.elf" "${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.bin"
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.elf)

add_custom_target(generate_binary ALL
  DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/nanoaltair.bin")
