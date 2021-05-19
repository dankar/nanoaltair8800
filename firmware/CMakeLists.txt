add_executable(nanoaltair.elf
  altair_main.c
  callbacks.c)

target_link_libraries(nanoaltair.elf common altair8800 stm32cube c m nosys)
