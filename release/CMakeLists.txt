
set(RELEASE_ARCHIVE "nanoaltair-${VERSION_STRING}.zip")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_ARCHIVE}
  COMMAND zip -j "${RELEASE_ARCHIVE}"
  "${CMAKE_BINARY_DIR}/firmware/nanoaltair.bin"
  "${CMAKE_BINARY_DIR}/firmware/nanoaltair.elf"
  "${CMAKE_SOURCE_DIR}/README.md"
  "${CMAKE_CURRENT_SOURCE_DIR}/flash_dfu.sh"
  DEPENDS generate_binary)

add_custom_target(create_release ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_ARCHIVE})
